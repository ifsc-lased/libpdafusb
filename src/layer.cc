/**
 * @file layer.cc
 * @brief Este arquivo possui implementações das classes Layer, LayerTimeout e LayerSerial
 * @author msobral
 * @date 2022-07-04
 *
 * Distribuído sob a licença MIT
 *
 */

#include <pdafusb/layer.hh>

namespace pdafusb
{
    Layer::Layer() : inferior(nullptr), superior(nullptr)
    {
    }
    Layer::~Layer()
    {
    }

    void Layer::set_layer(Layer *sup)
    {
        superior = sup;
        superior->inferior = this;
    }

    void Layer::enable_serial()
    {
    }

    LayerTimeout::LayerTimeout(long int tout) : Callback(tout)
    {
    }
    LayerTimeout::~LayerTimeout()
    {
    }

    LayerSerial::LayerSerial(const std::string &path, long int tout) : CallbackSerial(path, tout)
    {
    }

    LayerSerial::~LayerSerial()
    {
    }
}