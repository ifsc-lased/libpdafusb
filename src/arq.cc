/**
 * @file arq.cc
 * @brief Este arquivo possui a implementação da classe Arq.
 * @date 2022-07-04
 *
 * Distribuído sob a licença MIT
 *
 */

#include <pdafusb/arq.hh>

namespace pdafusb
{

    uint8_t Arq::buffer_mensagem_recebida[PdafUtils::kMaxBuffer] = {0};
    uint8_t Arq::buffer_mensagem_enviada[PdafUtils::kMaxBuffer];

    Arq::Arq(long tout) : LayerTimeout(tout)
    {
        this->seq_rx = false;
        this->seq_tx = false;
        this->tentativas_de_retransmissao = 0;
        this->state = (int)Arq::Estados::kIdle;
        this->mensagem_enviada_len = 0;
        this->dados_len = 0;
        this->dados = false;
        this->comando = 0x00;
        this->comando_envia = 0x00;
    }

    Arq::~Arq()
    {
    }


    void Arq::notifica(uint8_t &comando, uint8_t *data, size_t data_sz)
    {
        this->comando = comando;
        this->handle_fsm(data[0], &data[1], data_sz);
    }

    void Arq::envia(uint8_t &comando, uint8_t *data, size_t data_sz)
    {
        if (this->state == Arq::Estados::kIdle)
        {
            this->comando_envia = comando;
            this->idle(Arq::Controle::kNone, data, data_sz);
        }
    }

    void Arq::handle()
    {
    }

    void Arq::disable_callbacks()
    {
        disable_timeout();
        disable();
    }
    

    void Arq::handle_timeout()
    {
        disable_timeout();
        this->mecanismo_de_retransmissao();
    }
    void Arq::mecanismo_de_retransmissao()
    {
        if (this->tentativas_de_retransmissao < this->kTentativasMax)
        {
            inferior->envia(this->comando_envia, this->buffer_mensagem_enviada, this->mensagem_enviada_len);
            this->tentativas_de_retransmissao++;
            enable_timeout();
            reload_timeout();
        }
        else
        {
            disable_timeout();
            this->state = (int)Arq::Estados::kIdle;
            this->tentativas_de_retransmissao = 0;
            inferior->disable_callbacks();
        }
    }

    void Arq::handle_fsm(uint8_t controle, uint8_t *dados, size_t dados_sz)
    {
        switch (this->state)
        {
        case Arq::Estados::kIdle:
            this->idle(controle, dados, dados_sz);
            break;
        case Arq::Estados::kWait:
            this->wait(controle, dados, dados_sz);
            break;
        }
    }

    void Arq::wait(uint8_t controle, uint8_t *dados, size_t dados_sz)
    {
        if ((this->seq_tx == true && controle == Arq::Controle::kAck1) || (this->seq_tx == false && controle == Arq::Controle::kAck0))
        {
            this->seq_tx = !this->seq_tx;
            this->state = (int)Arq::Estados::kIdle;
            this->tentativas_de_retransmissao = 0;
            disable_timeout();
            inferior->enable_serial();
        }
        else if ((this->seq_tx == true && controle == Arq::Controle::kAck0) || (this->seq_tx == false && controle == Arq::Controle::kAck1))
        {
            this->mecanismo_de_retransmissao();
        }
        else if ((this->seq_rx == true && controle == Arq::Controle::kData1) || (this->seq_rx == false && controle == Arq::Controle::kData0))
        {
            this->ack(this->seq_rx);
            this->seq_rx = !this->seq_rx;

            this->dados = true;
            this->dados_len = dados_sz;
            memcpy((void *)this->buffer_mensagem_recebida, (const void *)dados,
                   dados_sz);

            superior->notifica(this->comando, dados, dados_sz);
        }
        else if ((this->seq_rx == false && controle == Arq::Controle::kData1) || (this->seq_rx == true && controle == Arq::Controle::kData0))
        {
            this->ack(!this->seq_rx);
        }
    }

    void Arq::idle(uint8_t controle, uint8_t *dados, size_t dados_sz)
    {
        if (controle == Arq::Controle::kNone)
        {
            if (this->seq_tx == false)
            {
                controle = 0x00;
            }
            else
            {
                controle = 0x08;
            }
            this->state = (int)Arq::Estados::kWait;

            uint8_t buffer[PdafUtils::kMaxBuffer] = {0};
            buffer[0] = controle;
            if (dados_sz > sizeof(buffer))
            {

                dados_sz = sizeof(buffer);
            }
            memcpy(&buffer[1], dados, dados_sz);

            size_t buffer_sz = dados_sz + 1;

            memcpy((void *)this->buffer_mensagem_enviada, (const void *)buffer, buffer_sz);
            this->mensagem_enviada_len = buffer_sz;
            
            inferior->envia(this->comando_envia, buffer, buffer_sz);

            if(comando_envia == PdafUtils::Comando_t::kEnviarMensagem){
                enable_timeout();
                reload_timeout();
            }
        }
        else
        {
            
            if ((this->seq_rx == true && controle == Arq::Controle::kData1) || (this->seq_rx == false && controle == Arq::Controle::kData0))
            {
                this->state = (int)Arq::Estados::kIdle;

                this->ack(this->seq_rx);
                this->seq_rx = !this->seq_rx;

                this->dados = true;
                this->dados_len = dados_sz;

                memcpy((void *)this->buffer_mensagem_recebida, (const void *)dados,
                       dados_sz);

                superior->notifica(this->comando, dados, dados_sz);
            }
            else if ((this->seq_rx == false && controle == Arq::Controle::kData1) || (this->seq_rx == true && controle == Arq::Controle::kData0))
            {
                this->state = Arq::Estados::kIdle;
                this->ack(!this->seq_rx);
            }
        }
    }

    void Arq::ack(bool seq)
    {
        uint8_t ack;
        if (seq)
        {
            ack = Arq::Controle::kAck1;
        }
        else
        {
            ack = Arq::Controle::kAck0;
        }
        inferior->envia(this->comando, &ack, 1);
    }

    std::string Arq::get_arquivo_bin(){
        return superior->get_arquivo_bin();
    }

    std::ostream * Arq::get_out_stream()
    {
        return superior->get_out_stream();
    }
}