/**
 * @file layer.hh
 * @brief Este arquivo possui declarações das classes Layer, LayerTimeout e LayerSerial
 * @author msobral
 * @date 2022-07-04
 *
 * Distribuído sob a licença MIT
 *
 */

#ifndef COM_LAYER_HH_
#define COM_LAYER_HH_

#include <pdafusb/poller.hh>

namespace pdafusb
{
    class Layer
    {
    public:
        Layer();
        ~Layer();

        void set_layer(Layer *sup);

        virtual void envia(uint8_t &comando, uint8_t *data, size_t data_sz) = 0;
        virtual void notifica(uint8_t &comando, uint8_t *data, size_t data_sz) = 0;
        virtual std::string get_arquivo_bin() = 0;
        virtual std::ostream * get_out_stream() = 0;

        virtual void disable_callbacks() = 0;
        void enable_serial();

    protected:
        Layer *inferior, *superior;
    };

    class LayerTimeout : public Layer, public Callback
    {
    public:
        LayerTimeout(long tout);
        ~LayerTimeout();


    };

    class LayerSerial : public Layer, public CallbackSerial
    {
    public:
        LayerSerial(const std::string &path, long tout);

        ~LayerSerial();

    };
}
#endif /* COM_LAYER_HH_ */
