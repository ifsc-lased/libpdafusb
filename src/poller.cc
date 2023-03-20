/**
 * @file poller.cc
 * @brief Este arquivo possui a implementação da classe Poller.
 * @date 2022-04-13
 * @author msobral
 *
 * Distribuído sob a licença MIT
 *
 */

#include <pdafusb/poller.hh>

namespace pdafusb
{

    Poller::Poller() : io()
    {
    }

    Poller::~Poller()
    {
    }

    void Poller::adiciona(Callback &cb)
    {
        cb.init(io);
        cbs.push_back(&cb);
    }

    void Poller::remove(Callback &cb)
    {
        cb.disable();
        cb.disable_timeout();
        auto it = std::find(cbs.begin(), cbs.end(), &cb);
        if (it != cbs.end())
            cbs.erase(it);
    }

    void Poller::limpa()
    {
        for (auto &cb : cbs)
        {
            remove(*cb);
        }
    }

    void Poller::despache()
    {
        for (auto &cb : cbs)
            cb->run();
        io.run();
    }

    void Poller::despache_simples()
    {
        for (auto &cb : cbs)
            cb->run();
        io.run_one();
    }

    void Poller::stop()
    {
        io.stop();
    }

    void Poller::restart()
    {
        io.restart();
    }
}