/**
 * @file pdaf_utils.hh 
 * @brief Classe utilitária com um conjunto de Enums para 
 * representar os comandos, tamanhos e códigos de erro.
 * @date 2022-07-04
 * 
 * Distribuído sob a licença MIT 
 * 
 */

#ifndef PDAF_UTILS_HH_
#define PDAF_UTILS_HH_

namespace pdafusb
{
    class PdafUtils
    {
    public:
        static const int kMaxBuffer = 4096;
        static const int kMaxRead = 1024;
        static const int kBytesHeaders = 3; //comando (1 byte) + tamanho (2 bytes)
        typedef enum Erro
        {
            kSemErro = 0x00,
            kComandoInvalido = 0x01,
            kTamanhoInvalido = 0x02,
            kTimeout = 0x03,
            kDesconhecido = 0x04

        } Erro_t;

        typedef enum Comando
        {
            kInvalido = 0x00,
            kEnviarMensagem = 0x01,
            kEnviarBinario = 0x02,
            kPing = 0x03,
            kErro = 0x04
        } Comando_t;

        typedef enum Tamanho
        {
            kTam2 = 2,
            kTam4 = 4
        } Tamanho_t;
    };
}
#endif
