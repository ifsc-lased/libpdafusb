/**
 * @file enquadramento.hh
 * @brief A classe Enquadramento é responsável pelo encapsulamento dos pedidos ou respostas da API DAF.
 * @date 2022-07-04
 *
 * Distribuído sob a licença MIT
 *
 */

#ifndef COM_ENQUADRAMENTO_HH_
#define COM_ENQUADRAMENTO_HH_

#include <pdafusb/pdaf_utils.hh>
#include <pdafusb/layer.hh>
#include <fstream>

namespace pdafusb
{

    class Enquadramento : public LayerSerial
    {

    public:
        enum Estados
        {
            kIdle = 0,
            kTamanho = 1,
            kRx = 2
        };
        /**
         * @brief Construtor da subcamada enquadramento que é responsável
         * pelo encapsulamento dos pedidos ou respostas da API DAF.
         *
         * @param[in] port		caminho da porta USB
         * @param[in] tout		timeout da camada Enquadramento
         */
        Enquadramento(const std::string &port, long tout);
        ~Enquadramento();

        /**
         * @brief função para representar a máquina de estados da camada
         *
         * @param[in] byte		byte recebido pela interface usb
         *
         */
        void handle_fsm(uint8_t byte);

        /**
         * @brief implementa o comportamento do estado idle da máquina de estados
         *
         * @param[in] byte		byte recebido pela interface usb
         *
         */
        void handle_fsm_idle(uint8_t byte);

        /**
         * @brief implementa o comportamento do estado Tamanho da máquina de estados
         *
         * @param[in] byte		byte recebido pela interface usb
         *
         */
        void handle_fsm_tamanho(uint8_t byte);

        /**
         * @brief implementa o comportamento do estado rx da máquina de estados
         *
         * @param[in] byte		byte recebido pela interface usb
         *
         */
        void handle_fsm_rx(uint8_t byte);

        /**
         * @brief reinicia os atributos da classe
         */
        void zera_variaveis();

        /**
         * @brief envia dados para a interface usb
         *
         * @param[in] dados		informacoes a ser transmitida
         * @param[in] len		Tamanho das informacoes
         *
         */
        void escreve_usb(uint8_t *dados, size_t len);

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
         * @brief função chamada pela camada inferior para transferência dos dados recebidos
         *
         * @param[in] comando	byte de Comando da mensagem
         * @param[in] dados		buffer com os dados da mensagem
         * @param[in] dados_sz	Tamanho do buffer com os dados da mensagem
         *
         */
        void notifica(uint8_t &comando, uint8_t *data, size_t data_sz);

        /**
         * @brief desabilita callbacks (descritor e timeout) da camada
         *
         */
        void disable_callbacks();
        
        /**
         * @brief habilita callback (descritor serial)
         *
         */
        void enable_serial();

        /**
         * @brief prepara quadro para envio de mensagem
         *
         * @param[in] dados		buffer com os dados da mensagem
         * @param[in] dados_sz	Tamanho do buffer com os dados da mensagem
         *
         */
        void envia_mensagem(uint8_t *data, size_t data_sz);

        /**
         * @brief faz o envio da mensagem binária
         *
         * @param[in] dados		buffer com os dados da mensagem
         * @param[in] dados_sz	Tamanho do buffer com os dados da mensagem
         *
         */
        void envia_binario(uint8_t *data, size_t data_sz);

        /**
         * @brief retorna string com path do arquivo binario.
         * 
         * @return std::string
         */
        std::string get_arquivo_bin();

        /**
         * @brief retorna output stream para resposta.
         * 
         * @return std::ostream
         */
        std::ostream * get_out_stream();

        /**
         * @brief callback do timeout
         */
        void handle_timeout();

        /**
         * @brief callback da serial
         */
        void handle();

        bool is_output_stream;

    private:
        static uint8_t buffer_dados[PdafUtils::kMaxBuffer];
        static uint8_t buffer_enviado[PdafUtils::kMaxBuffer];
        
        int state;
        int atual_tamanho_do_campo_tamanho;

        uint8_t campo_comando;
        uint8_t buffer_tamanho[4];
        uint8_t campo_controle;

        uint32_t campo_tamanho;

        size_t dados_sz;
        size_t len_buffer_enviado;
        uint8_t comando_envia;

        PdafUtils::Tamanho_t tamanho_do_campo_tamanho;
    };
}
#endif /* COM_ENQUADRAMENTO_HH_ */
