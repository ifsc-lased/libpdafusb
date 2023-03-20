/**
 * @file poller.hh
 * @brief A classe Poller atua como um despachador de eventos.
 * @author msobral
 * @date 2022-04-13
 *
 * Distribuído sob a licença MIT
 *
 */
#ifndef POLLERASIO_POLLER_HH_
#define POLLERASIO_POLLER_HH_

#include <list>
#include <pdafusb/callback.hh>

namespace pdafusb
{

    // Poller: um despachador de eventos
    // Um objeto poller é capaz de monitorar um conjunto de descritores de arquivos
    // e executar um callback para cada descritor pronto para acesso
    // Cada descritor pode especificar um timeout próprio
    class Poller
    {
    public:
        Poller();
        ~Poller();

        // adiciona um evento a ser vigiado, representado por um Callback
        void adiciona(Callback &cb);

        // remove callback
        void remove(Callback &cb);

        // remove todos callbacks
        void limpa();

        // vigia os descritores cadastrados e despacha um evento
        // para ser lido, ou até que expire o timeout  (chama seu callback)
        void despache_simples();

        // despacha eventos para os callbacks indefinidamente
        // retorna se nenhum novo evento puder ser gerado, o que acontece
        // se todos os callbacks estiverem desativados (incluindo seus timers)
        void despache();

        void stop();

        void restart();

    protected:
        std::list<Callback *> cbs;
        asio::io_context io;
    };
}
#endif // POLLERASIO_POLLER_HH_
