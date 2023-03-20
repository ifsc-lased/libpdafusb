/**
 * @file callback.hh
 * @brief Este arquivo possui declarações das classes Callback e CallbackSerial
 * @author msobral
 * @date 2018-09-20
 *
 * Distribuído sob a licença MIT
 *
 */

#ifndef CALLBACK_HH_
#define CALLBACK_HH_

#include <stdexcept>
#include <pdafusb/pdaf_utils.hh>
#include <asio.hpp>

namespace pdafusb
{

    class Poller;

    // classe abstrata para os callbacks do poller
    class Callback
    {
    public:
        // cria um callback para um timer (equivalente ao construtor anterior com fd=-1)
        // out: timeout
        Callback(long tout);

        ~Callback();

        // ao especializar esta classe, devem-se implementar estas duas funções!
        // handle: trata o evento representado neste callback
        // handle_timeout: trata o timeout associado a este callback
        virtual void handle() = 0;
        virtual void handle_timeout() = 0;

        // obtém o descritor monitorado
        // boost::asio::windows::stream_handle::native_handle_type
        // virtual boost::asio::posix::stream_descriptor::native_handle_type filedesc() const {return -1;};
        // virtual boost::asio::windows::stream_handle::native_handle_type filedesc() const {return (void*)-1;};

        // obtém o valor atual de timeout ( quantos ms faltam para o timer disparar)
        asio::steady_timer::time_point timeout() const;

        // altera o valor do timeout base (aquele informado no construtor)
        void set_timeout(long timeout);

        // recarrega o timeout com o valor default (base)
        void reload_timeout();

        // define o valor do próximo timeout ... irá valer somente uma vez !
        void reload_timeout(long int t);

        // desativa timeout
        void disable_timeout();

        // ativa timeout
        void enable_timeout();

        // desativa monitoramento do descritor
        virtual void disable();

        // ativa monitoramento do descritor
        virtual void enable();

        // retorna true se monitoramento do descritor está ativado
        bool is_enabled() const { return enabled; }

        // retorna true se timeout está ativado
        bool timeout_enabled() const { return enabled_to; }

    protected:
        class TimeoutHandler
        {
        public:
            TimeoutHandler(Callback *ptr) : cb(ptr) {}
            void operator()(std::error_code err)
            {
                if (!err)
                {
                    cb->handle_timeout();
                    cb->reload_timeout();
                    cb->run();
                }
            }

        private:
            Callback *cb;
        };

        std::unique_ptr<asio::steady_timer> timer;
        TimeoutHandler timeout_handler;

        long tout;
        long base_tout; // milissegundos. Se <= 0, este callback não tem timeout
        bool enabled_to;
        bool enabled;

        friend class Poller;

        virtual void init(asio::io_context &io);
        virtual void run();
        // operator==: compara dois objetos callback
        // necessário para poder diferenciar callbacks ...
        virtual bool operator==(const Callback *o) const;
    };

    const int BufSize = PdafUtils::kBytesHeaders;

    class CallbackSerial : public Callback
    {
    public:
        // cria um callback para um Serial (equivalente ao construtor anterior com fd=-1)
        // out: timeout
        CallbackSerial(const std::string &path, long tout);
        CallbackSerial(long tout);

        ~CallbackSerial();

        // desativa monitoramento do descritor
        virtual void disable();
        // ativa monitoramento do descritor
        virtual void enable();

    protected:
        class Complete
        {
        public:
            Complete(CallbackSerial *ptr) : cb(ptr) {}
            void operator()(std::error_code err, std::size_t n)
            {
                if (!err)
                {
                    cb->buf_len = n;
                    cb->handle();
                    //cb->run();
                }
            }

        private:
            CallbackSerial *cb;
        };

        std::unique_ptr<asio::serial_port> descr;
        char data[BufSize];
        int buf_len;
        std::string dev_name;
        Complete handler;

        virtual void init(asio::io_context &io);
        virtual void run();

        // operator==: compara dois objetos callback
        // necessário para poder diferenciar callbacks ...
        virtual bool operator==(const CallbackSerial *o) const;
    };
}
#endif /* CALLBACK_HH_ */