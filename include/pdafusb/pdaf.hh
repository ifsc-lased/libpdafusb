/**
 * @file pdaf.hh
 * @brief A classe Pdaf deve ser usada pela aplicação (i.e. PAF-DAF) para
 * interagir com um DAF.
 * @date 2022-07-04
 *
 * Distribuído sob a licença MIT
 *
 */

#ifndef PDAF_HH_
#define PDAF_HH_

#include <pdafusb/arq.hh>
#include <pdafusb/enquadramento.hh>
#include <pdafusb/layer.hh>

namespace pdafusb
{
    class Pdaf : public LayerTimeout
    {

    public:
        /**
         * @brief Construtor do Pdaf, que é o controlador das subcamadas de enquadramento e arq. Nele
         * é configurado o poller(despachador de eventos) e faz o tratamento dos pedidos e
         * respostas da API DAF.
         *
         * @param[in] tout			timeout da camada Pdaf
         * @param[in] port_usb		caminho da porta USB
         *
         */
        Pdaf(long tout, std::string port_usb = "");
        ~Pdaf();

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
         * @brief função para enviar mensagem para camada inferior.
         * O parâmetro comando define se a mensagem deve ser enviada
         * como texto ou binário.
         *
         * @param[in] comando	byte de Comando da mensagem
         * @param[in] dados		buffer com os dados da mensagem
         * @param[in] dados_sz	Tamanho do buffer com os dados da mensagem
         *
         */
        void envia(uint8_t &comando, uint8_t *data, size_t data_sz);

        /**
         * @brief função para enviar texto.
         * O parâmetro data recebe a resposta do pedido
         *
         * @param[in] msg	string com os dados da mensagem de pedido
         * @param[out] data	buffer para armazenar a resposta
         * 
         * @return tamanho do buffer da resposta em bytes
         *
         */
        size_t envia_texto(std::string msg, uint8_t *data);

        /**
         * @brief função para enviar texto.
         * O parâmetro out recebe a resposta do pedido
         *
         * @param[in] msg	string com os dados da mensagem
         * @param[out] out	stream para resposta
         * 
         * @return tamanho da resposta em bytes
         *
         */
        size_t envia_texto(std::string msg, std::ostream *out);

        /**
         * @brief Função para tratar o envio de um aquivo binário ao daf.
         * 
         * @param[in] path	string com o path do arquivo binário
         * @param[out] data	buffer para armazenar a resposta
         * 
         * @return tamanho do buffer da resposta em bytes
         */
        size_t envia_binario(std::string path, uint8_t *data);

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
         * @brief callback do descritor
         */
        void handle();

        /**
         * @brief retransmite mensagem para camada inferior
         */
        void mecanismo_de_retransmissao(void);

        /**
         * @brief configura as camadas do protocolo
         */
        void config_camadas();

        /**
         * @brief função para receber a resposta do DAF
         *
         * @param dados buffer de dados com a resposta
         * @return tamanho do buffer
         */
        size_t get_buffer(uint8_t *dados) const;

        /**
         * @brief reinicia os atributos da classe
         */
        void zera_variaveis();

        /**
         * @brief desabilita calbacks (descritor e timeout) da camada
         */
        void disable_callbacks();

    private:
        static uint8_t buffer_recebido[PdafUtils::kMaxBuffer];
        static uint8_t msg_enviada[PdafUtils::kMaxBuffer];
        size_t buffer_recebido_len;
        size_t msg_enviada_sz;
        uint8_t comando_envia;
        std::ostream *out_stream;
        
        Poller poller;
        Arq arq;
        Enquadramento enq;

        static const int kTentativasMax = 4;
        int tentativas_de_retransmissao;

        // base segundo
        static const long int kSeg = 1000;
        static const long int kTimeoutPdafMs = 2 * kSeg;
        static const long int kTimeoutArqMs = 1 * kSeg;
        static const long int kTimeoutEnqMS = kSeg / 2;

        std::string arquivo_binario;
    };
}
#endif // PDAF_H
