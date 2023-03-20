/**
 * @file arq.hh
 * @brief A classe Arq é responsável para indicar ao emissor se um comando
 * foi entregue com sucesso ao destino
 * @date 2022-07-04
 *
 * Distribuído sob a licença MIT
 *
 */

#ifndef COM_ARQ_HH_
#define COM_ARQ_HH_

#include <pdafusb/layer.hh>
#include <pdafusb/pdaf_utils.hh>

namespace pdafusb
{

    class Arq : public LayerTimeout
    {

    public:
        enum Estados
        {
            kIdle = 0,
            kWait = 1
        };

        enum Controle
        {
            kData0 = 0x00,
            kData1 = 0x08,
            kAck0 = 0x80,
            kAck1 = 0x88,
            kNone = 0xff
        };

        /**
         * @brief Construtor da subcamada Arq que é responsável por garantir ao transmissor
         * que um comando foi entregue ou não ao destino.
         *
         * @param[in] tout			timeout da camada Arq
         */
        Arq(long tout);

        ~Arq();

        /**
         * @brief função para representar a máquina de estados da camada
         *
         * @param[in] comando	byte de Comando da mensagem
         * @param[in] dados		buffer com os dados da mensagem
         * @param[in] dados_sz	Tamanho do buffer com os dados da mensagem
         *
         */
        void handle_fsm(uint8_t controle, uint8_t *dados, size_t dados_sz);

        /**
         * @brief implementa o comportamento do estado idle da máquina de estados
         *
         * @param[in] controle  byte de controle da mensagem
         * @param[in] dados		buffer com os dados da mensagem
         * @param[in] dados_sz	Tamanho do buffer com os dados da mensagem
         *
         */
        void idle(uint8_t controle, uint8_t *dados, size_t dados_sz);

        /**
         * @brief implementa o comportamento do estado wait da máquina de estados
         *
         * @param[in] controle  byte de controle da mensagem
         * @param[in] dados		buffer com os dados da mensagem
         * @param[in] dados_sz	Tamanho do buffer com os dados da mensagem
         *
         */
        void wait(uint8_t controle, uint8_t *dados, size_t dados_sz);

        /**
         * @brief envia ack para camada inferior
         *
         * @param[in] seq 	numero de sequencia do ack
         */
        void ack(bool seq);

        /**
         * @brief retransmite mensagem para camada inferior
         */
        void mecanismo_de_retransmissao(void);

        /**
         * @brief função chamada pela camada inferior para transferência dos dados recebidos
         *
         * @param[in] comando	byte de Comando da mensagem
         * @param[in] dados		buffer com os dados da mensagem
         * @param[in] dados_sz	Tamanho do buffer com os dados da mensagem
         *
         */
        void notifica(uint8_t &comando, uint8_t *data, size_t data_sz);

        /**
         * @brief função chamada pela camada superior para transferência dos dados enviados
         *
         * @param[in] comando	byte de Comando da mensagem
         * @param[in] dados		buffer com os dados da mensagem
         * @param[in] dados_sz	Tamanho do buffer com os dados da mensagem
         *
         */
        void envia(uint8_t &comando_envia, uint8_t *data, size_t data_sz);

        /**
         * @brief retorna output stream para resposta.
         * 
         * @return std::ostream
         */
        std::ostream * get_out_stream();

        /**
         * @brief retorna string com path do arquivo binario.
         * 
         * @return std::string
         */
        std::string get_arquivo_bin();

        /**
         * @brief callback do timeout desta camada
         */
        void handle_timeout();

        /**
         * @brief callback do descritor desta camada
         */
        void handle();

        /**
         * @brief desativa callbacks (timeout e descritor) da camada
         */
        void disable_callbacks();

    private:
        static const int kTentativasMax = 3;
        static uint8_t buffer_mensagem_recebida[PdafUtils::kMaxBuffer];
        static uint8_t buffer_mensagem_enviada[PdafUtils::kMaxBuffer];

        bool seq_rx;
        bool seq_tx;
        bool dados;

        int tentativas_de_retransmissao;
        int state;
        int dados_len;

        uint8_t comando;
        uint8_t comando_envia;
        size_t mensagem_enviada_len = {0};
    };

}
#endif /* COM_ARQ_HH_ */
