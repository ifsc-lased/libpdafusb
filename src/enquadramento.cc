/**
 * @file enquadramento.cc
 * @brief Este arquivo possui a implementação da classe Enquadramento
 * @date 2022-07-04
 *
 * Distribuído sob a licença MIT
 *
 */

#include <pdafusb/enquadramento.hh>

namespace pdafusb
{

    uint8_t Enquadramento::buffer_dados[PdafUtils::kMaxBuffer];
    uint8_t Enquadramento::buffer_enviado[PdafUtils::kMaxBuffer];

    Enquadramento::Enquadramento(const std::string &port, long tout) : LayerSerial(port, tout)
    {
        this->state = (int)Estados::kIdle;
        this->dados_sz = 0;
        this->atual_tamanho_do_campo_tamanho = 0;
        this->campo_tamanho = 0;
        this->campo_comando = PdafUtils::Comando_t::kInvalido;
        this->tamanho_do_campo_tamanho = PdafUtils::Tamanho_t::kTam2;
        this->campo_controle = 0x00;
        this->len_buffer_enviado = 0;
        this->comando_envia = 0x00;
        this->is_output_stream = false;
    }

    Enquadramento::~Enquadramento()
    {
    }

    void Enquadramento::notifica(uint8_t &comando, uint8_t *data, size_t data_sz)
    {
    }

    void Enquadramento::envia(uint8_t &comando, uint8_t *data, size_t data_sz)
    {
        enable_serial();
        switch (comando)
        {
        case PdafUtils::Comando_t::kEnviarMensagem:
            Enquadramento::envia_mensagem(data, data_sz);
            break;
        case PdafUtils::Comando_t::kEnviarBinario:
            Enquadramento::envia_binario(data, data_sz);
            break;
        default:
            break;
        }
    }

    void Enquadramento::envia_mensagem(uint8_t *data, size_t data_sz)
    {

        uint8_t buffer[PdafUtils::kMaxBuffer] = {0};
        buffer[0] = PdafUtils::Comando_t::kEnviarMensagem;
        buffer[1] = (data_sz & 0xff00) >> 8;
        buffer[2] = data_sz & 0xff;

        memcpy(&buffer[3], data, data_sz);
        this->escreve_usb(buffer, data_sz + 3);
    }
    void Enquadramento::envia_binario(uint8_t *data, size_t data_sz)
    {

        // recupera path do arquivo das camadas superiores
        std::string path = superior->get_arquivo_bin();

        std::ifstream arquivo(path, std::ios::ate | std::ios::binary);

        // Verifica se o arquivo foi encontrado
        if (!arquivo.is_open())
        {
            throw std::invalid_argument("Arquivo '" + path + "' não encontrado");
        }

        // recupera tamanho do arquivo em bytes
        size_t arquivo_size = arquivo.tellg();
        arquivo.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer_headers;
        size_t frame_size = arquivo_size + data_sz;

        // Adiciona campo comando e tamanho ao buffer.
        buffer_headers.push_back(PdafUtils::Comando_t::kEnviarBinario);
        buffer_headers.push_back((frame_size & 0xff000000) >> 24);
        buffer_headers.push_back((frame_size & 0x00ff0000) >> 16);
        buffer_headers.push_back((frame_size & 0x0000ff00) >> 8);
        buffer_headers.push_back(frame_size & 0x000000ff);

        // Adiciona campo controle ao buffer
        for (size_t i = 0; i < data_sz; i++)
        {
            buffer_headers.push_back(data[i]);
        }

        // escreve na serial os cabeçalhos
        descr->write_some(asio::buffer(buffer_headers));

        int atual_size = arquivo_size;
        int read_size = PdafUtils::kMaxRead;

        // escreve na serial os dados do arquivo de forma fragmentada 
        // em blocos de tamanho PdafUtils::kMaxRead
        do
        {
            read_size = (atual_size <= read_size) ? atual_size : PdafUtils::kMaxRead;
            std::vector<uint8_t> buffer_data(read_size, 0);
            arquivo.read((char *)&buffer_data[0], read_size);
            descr->write_some(asio::buffer(buffer_data));
            // atual_size = quantidade de bytes restantes para leitura
            atual_size -= read_size;
        } while (atual_size > 0);
        arquivo.close();
    }

    void Enquadramento::escreve_usb(uint8_t *dados, size_t len)
    {
        memcpy((void *)this->buffer_enviado, (const void *)dados, len);
        this->len_buffer_enviado = len;
        // descr = ponteiro para asio::serial_port presente no CallbackSerial
        descr->write_some(asio::buffer(dados, len));
    }

    void Enquadramento::handle()
    {
        disable();
        for (int i = 0; i < buf_len; i++)
        {
            this->handle_fsm(data[i]);
        }
        int atual_size = this->campo_tamanho;
        int read_size = PdafUtils::kMaxRead;
        do
        {
            // consome bloco de dados da serial e encaminha para máquina de estados
            read_size = (atual_size <= read_size) ? atual_size : PdafUtils::kMaxRead;
            std::vector<uint8_t> buffer_data(read_size, 0);
            descr->read_some(asio::buffer(buffer_data));

            for (int i = 0; i < buffer_data.size(); i++)
            {
                this->handle_fsm(buffer_data[i]);
            }

            // atual_size = quantidade de bytes restantes para leitura
            atual_size -= read_size;

        } while (atual_size > 0);
    }

    void Enquadramento::handle_timeout()
    {
        disable_callbacks();
        this->zera_variaveis();
    }

    void Enquadramento::handle_fsm(uint8_t byte)
    {
        switch (this->state)
        {
        case Estados::kIdle:
            this->handle_fsm_idle(byte);
            break;
        case Estados::kTamanho:
            this->handle_fsm_tamanho(byte);
            break;
        case Estados::kRx:
            this->handle_fsm_rx(byte);
            break;
        }
    }

    void Enquadramento::handle_fsm_idle(uint8_t byte)
    {

        switch (byte)
        {
        case PdafUtils::Comando_t::kEnviarMensagem:
            this->campo_comando = byte;
            this->tamanho_do_campo_tamanho = PdafUtils::Tamanho_t::kTam2;
            this->state = (int)Estados::kTamanho;
            enable_timeout();
            reload_timeout();
            break;

        case PdafUtils::Comando_t::kEnviarBinario:
            this->campo_comando = byte;
            this->tamanho_do_campo_tamanho = PdafUtils::Tamanho_t::kTam4;
            this->state = (int)Estados::kTamanho;
            enable_timeout();
            reload_timeout();
            break;
        default:
            break;
        }
    }

    void Enquadramento::handle_fsm_tamanho(uint8_t byte)
    {
        if (this->atual_tamanho_do_campo_tamanho < (this->tamanho_do_campo_tamanho - 1))
        {
            this->buffer_tamanho[this->atual_tamanho_do_campo_tamanho] = byte;
            this->atual_tamanho_do_campo_tamanho++;
            reload_timeout();
        }
        else if (this->atual_tamanho_do_campo_tamanho == (this->tamanho_do_campo_tamanho - 1))
        {
            this->buffer_tamanho[this->atual_tamanho_do_campo_tamanho] = byte;
            this->state = (int)Estados::kRx;
            reload_timeout();

            if (this->tamanho_do_campo_tamanho == PdafUtils::Tamanho_t::kTam2)
            {
                this->campo_tamanho = (this->buffer_tamanho[0] << 8) + this->buffer_tamanho[1];
            }
            if (this->is_output_stream && (this->campo_tamanho > 1))
            {
                // em caso de restransmissão, limpa o out stream
                this->get_out_stream()->seekp(0, std::ios::beg);
            }
        }
    }

    void Enquadramento::handle_fsm_rx(uint8_t byte)
    {

        if (this->dados_sz < (this->campo_tamanho - 1))
        {
            if (this->is_output_stream && (this->dados_sz > 0))
            {
                this->get_out_stream()->put(byte);
            }
            else if (this->dados_sz < PdafUtils::kMaxBuffer)
            {
                this->buffer_dados[this->dados_sz] = byte;
            }
            this->dados_sz++;
            reload_timeout();
        }
        else if (this->dados_sz == (this->campo_tamanho - 1))
        {
            if (this->dados_sz == 0)
            {
                this->buffer_dados[this->dados_sz] = byte;
            }
            else
            {
                if (this->is_output_stream)
                {
                    this->get_out_stream()->put(byte);
                }
                else if (this->dados_sz < PdafUtils::kMaxBuffer)
                {
                    this->buffer_dados[this->dados_sz] = byte;
                }
            }

            this->state = (int)Estados::kIdle;

            uint8_t comando = this->campo_comando;
            size_t dados_arq_sz  = (this->campo_tamanho < PdafUtils::kMaxBuffer) ? this->campo_tamanho : PdafUtils::kMaxBuffer;
            uint8_t dados[PdafUtils::kMaxBuffer];
            memcpy((void *)dados, (const void *)this->buffer_dados, dados_arq_sz);
            enable_serial();
            superior->notifica(comando, dados, dados_arq_sz);
            disable_timeout();
            this->zera_variaveis();
        }
    }

    void Enquadramento::zera_variaveis()
    {
        this->state = (int)Estados::kIdle;
        memset(this->buffer_dados, '\0', PdafUtils::kMaxBuffer);
        memset(this->buffer_tamanho, '\0', 4);
        this->dados_sz = 0;
        this->atual_tamanho_do_campo_tamanho = 0;
        this->campo_tamanho = 0;
        this->campo_comando = PdafUtils::Comando_t::kInvalido;
        this->tamanho_do_campo_tamanho = PdafUtils::Tamanho_t::kTam2;
        this->campo_controle = 0x00;
    }

    void Enquadramento::disable_callbacks()
    {
        disable_timeout();
        disable();
    }

    void Enquadramento::enable_serial()
    {
        enable();
        run();
    }

    std::string Enquadramento::get_arquivo_bin()
    {
        return superior->get_arquivo_bin();
    }

    std::ostream *Enquadramento::get_out_stream()
    {
        return superior->get_out_stream();
    }
}