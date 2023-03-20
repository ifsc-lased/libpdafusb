/**
 * @file pdaf.cc
 * @brief Este arquivo possui a implementação da classe Pdaf.
 * @date 2022-07-04
 *
 * Distribuído sob a licença MIT
 *
 */

#include <pdafusb/pdaf.hh>

namespace pdafusb
{

    uint8_t Pdaf::buffer_recebido[PdafUtils::kMaxBuffer];
    uint8_t Pdaf::msg_enviada[PdafUtils::kMaxBuffer];

    Pdaf::Pdaf(long tout, std::string port_usb) : LayerTimeout(tout), poller(), arq(kTimeoutArqMs), enq(port_usb, kTimeoutEnqMS)
    {
        this->buffer_recebido_len = 0;
        this->msg_enviada_sz = 0;
        this->tentativas_de_retransmissao = 0;
        this->comando_envia = 0x00;

        if(tout < kTimeoutPdafMs){
            set_timeout(kTimeoutPdafMs);
        }

        arq.set_layer(this);
        enq.set_layer(&arq);
        
    }

    Pdaf::~Pdaf()
    {
    }

    void Pdaf::handle()
    {
    }

    void Pdaf::disable_callbacks()
    {
        disable_timeout();
        disable();
    }

    void Pdaf::handle_timeout()
    {
        
        if (this->tentativas_de_retransmissao == 0)
        {
            reload_timeout();
            inferior->envia(this->comando_envia, this->msg_enviada, this->msg_enviada_sz);
            this->tentativas_de_retransmissao++;
        }
        else
        {
            this->mecanismo_de_retransmissao();
        }
    }

    void Pdaf::mecanismo_de_retransmissao()
    {
        if (this->tentativas_de_retransmissao < this->kTentativasMax)
        {
            this->tentativas_de_retransmissao++;
            reload_timeout();
            inferior->envia(this->comando_envia, this->msg_enviada, this->msg_enviada_sz);
        }
        else
        {
            this->tentativas_de_retransmissao = 0;
            disable_timeout();
        }
    }

    void Pdaf::notifica(uint8_t &comando, uint8_t *data, size_t data_sz)
    {
        disable_timeout();
        this->arq.disable_callbacks();
        this->enq.disable_callbacks();

        memcpy((void *)this->buffer_recebido, (const void *)data, data_sz - 1);
        this->buffer_recebido_len = data_sz;

        this->msg_enviada_sz = 0;
        memset(this->msg_enviada, '\0', PdafUtils::kMaxBuffer);
        this->tentativas_de_retransmissao = 0;
    }

    void Pdaf::envia(uint8_t &comando, uint8_t *data, size_t data_sz)
    {
        memcpy((void *)msg_enviada, (const void *)data, data_sz);
        this->msg_enviada_sz = data_sz;
        this->comando_envia = comando;

        this->buffer_recebido_len = 0;
        memset(this->buffer_recebido, '\0', PdafUtils::kMaxBuffer);
        config_camadas();

    }

    size_t Pdaf::envia_texto(std::string msg, uint8_t *data)
    {
        this->enq.is_output_stream = false;
        // Copiando mensagem para o buffer;
        uint8_t buffer[pdafusb::PdafUtils::kMaxBuffer] = {0};
        std::copy(msg.begin(), msg.end(), buffer);

        // Comando do tipo de mensagem
        uint8_t comando = pdafusb::PdafUtils::Comando_t::kEnviarMensagem;

        this->envia(comando, buffer, msg.length());

        return this->get_buffer(data);
    }

    size_t Pdaf::envia_texto(std::string msg, std::ostream *out)
    {
        this->out_stream = out;
        this->enq.is_output_stream = true;

        // Copiando mensagem para o buffer;
        uint8_t buffer[pdafusb::PdafUtils::kMaxBuffer] = {0};
        std::copy(msg.begin(), msg.end(), buffer);

        // Comando do tipo de mensagem
        uint8_t comando = pdafusb::PdafUtils::Comando_t::kEnviarMensagem;

        this->envia(comando, buffer, msg.length());

        return this->out_stream->tellp();
    }

    size_t Pdaf::envia_binario(std::string path, uint8_t *data){
        this->arquivo_binario = path;
        uint8_t comando = PdafUtils::Comando::kEnviarBinario;
        uint8_t data_vazio[PdafUtils::kMaxBuffer];
        memset(data_vazio, '\0', PdafUtils::kMaxBuffer);
        this->envia(comando, data_vazio, 0);
        return this->get_buffer(data);
    }

    std::string Pdaf::get_arquivo_bin(){
        return this->arquivo_binario;
    }

    std::ostream * Pdaf::get_out_stream()
    {
        return this->out_stream;
    }

    size_t Pdaf::get_buffer(uint8_t *dados) const
    {

        memcpy((void *)dados, (const void *)this->buffer_recebido, this->buffer_recebido_len);
        return this->buffer_recebido_len;
    }

    void Pdaf::config_camadas()
    {
        //habilita callbacks das camadas para adicionar no poller        
        this->enable_timeout();
        arq.enable_timeout();
        enq.enable_timeout();
        enq.enable();
    

        poller.restart();
        poller.adiciona(*this);
        poller.adiciona(arq);
        poller.adiciona(enq);


        arq.disable_timeout();
        enq.disable_timeout();
        // timeout pdaf somente para primeiro envio (retransmissão = 0)
        this->reload_timeout(10);


        poller.despache();
    }
    void Pdaf::zera_variaveis()
    {
        this->buffer_recebido_len = 0;
        this->msg_enviada_sz = 0;
        memset(this->buffer_recebido, '\0', PdafUtils::kMaxBuffer);
        memset(this->msg_enviada, '\0', PdafUtils::kMaxBuffer);
    }
}