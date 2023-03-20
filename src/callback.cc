/**
 * @file callback.cc
 * @brief Este arquivo possui implementações das classes Callback e CallbackSerial
 * @author msobral
 * @date 2018-09-20
 *
 * Distribuído sob a licença MIT
 *
 */

#include <pdafusb/callback.hh>

namespace pdafusb
{

    Callback::Callback(long tout) : timeout_handler(this), tout(tout), base_tout(tout), enabled_to(true), enabled(false)
    {
        if (tout < 0)
            throw -1;
    }

    Callback::~Callback()
    {
        disable();
        disable_timeout();
    }

    void Callback::init(asio::io_context &io)
    {
        if (enabled_to)
        {
            timer.reset(new asio::steady_timer(io, asio::chrono::milliseconds(tout)));
        }
    }

    asio::steady_timer::time_point Callback::timeout() const { return timer->expiry(); }

    void Callback::reload_timeout()
    {
        tout = base_tout;
        run(); // para alteração já surtir efeito antes do próximo timeout com valor antigo
    }

    void Callback::reload_timeout(long int t)
    {
        tout = t;
        run();
    }

    void Callback::set_timeout(long timeout)
    {
        base_tout = timeout;
    }

    bool Callback::operator==(const Callback *o) const
    {
        return false;
    }

    void Callback::disable_timeout()
    {
        if (enabled_to)
        {
            enabled_to = false;
            if (timer.get())
            {
                timer->cancel();
            }
        }
    }

    void Callback::enable_timeout()
    {
        enabled_to = true;
    }

    void Callback::disable()
    {
        enabled = false;
    }

    void Callback::enable()
    {
        enabled = true;
    }

    void Callback::run()
    {
        if (enabled_to)
        {
            timer->expires_after(asio::chrono::milliseconds(tout));
            timer->async_wait(timeout_handler);
        }
    }

    // adaptador para callback da porta serial
    CallbackSerial::CallbackSerial(const std::string &path, long tout) : Callback(tout), dev_name(path), handler(this)
    {
        enabled = true;
        buf_len = 0;
    }

    CallbackSerial::CallbackSerial(long tout) : Callback(tout), handler(this)
    {
        buf_len = 0;
    }

    void CallbackSerial::disable()
    {
        if (enabled)
        {
            if (descr.get())
            {
                descr->cancel();
            }
            Callback::disable();
        }
    }

    void CallbackSerial::enable()
    {
        enabled = true;
        buf_len = 0;
    }

    void CallbackSerial::init(asio::io_context &io)
    {
        Callback::init(io);
        if (enabled)
        {
            try
            {
                if (!descr.get()){
                    descr.reset(new asio::serial_port(io, dev_name));
                }
            }
            catch (const std::exception &e)
            {
                if (std::strcmp(e.what(), "open"))
                {
                    throw std::invalid_argument("Porta não foi encontrada ou precisa de permissão de leitura e escrita.");
                }
            }

            descr->set_option(asio::serial_port_base::baud_rate(115200));
        }
    }

    void CallbackSerial::run()
    {
        Callback::run();
        if (enabled)
        {
            if (descr.get())
            {
                descr->cancel();
                asio::async_read(*descr, asio::buffer(data, BufSize), handler);
            }
        }
    }

    CallbackSerial::~CallbackSerial()
    {
    }

    bool CallbackSerial::operator==(const CallbackSerial *o) const
    {
        return this == o;
    }
}