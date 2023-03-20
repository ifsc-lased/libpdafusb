[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Ubuntu](https://github.com/ifsc-lased/libpdafusb-tentativa/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/ifsc-lased/libpdafusb-tentativa/actions/workflows/ubuntu.yml)
[![macOS](https://github.com/ifsc-lased/libpdafusb-tentativa/actions/workflows/macos.yaml/badge.svg)](https://github.com/ifsc-lased/libpdafusb-tentativa/actions/workflows/macos.yaml)
[![Windows](https://github.com/ifsc-lased/libpdafusb-tentativa/actions/workflows/windows.yaml/badge.svg)](https://github.com/ifsc-lased/libpdafusb-tentativa/actions/workflows/windows.yaml)

> # Aviso de isenção de responsabilidade
> O código presente nesse repositório está sendo disponibilizado publicamente para demonstrar uma forma de implementar, na linguagem C++, o protocolo PDAF-USB de acordo com a Especificação Técnica de Requisitos do Dispositivo Autorizador Fiscal (DAF) - revisão 2.0.1. Contudo, o código não foi amplamente testado e validado de forma a garantir um funcionamento correto, confiável e seguro. Sendo assim, o código aqui disponibilizado não deve ser considerado como uma implementação de referência para o PDAF-USB.
>
> Não cabe à SEF ou ao IFSC prestar suporte sobre os componentes, códigos ou excertos de código disponíveis nesse repositório, sendo a presente versão considerada final, sem previsão de alterações, correção de bugs ou melhorias.
>
> A SEF e o IFSC eximem-se de qualquer responsabilidade, direta ou indireta, por perdas ou danos, comprovadamente ou alegadamente, causados pelo código aqui disponibilizado. Caso deseje usar essa biblioteca em seu projeto ou produto, você estará fazendo isto exclusivamente por sua conta e risco.

# Biblioteca PDAF-USB em C++

Biblioteca desenvolvida na linguagem C++ que implementa o protocolo PDAF-USB de acordo com a [Especificação Técnica de Requisitos do Dispositivo Autorizador Fiscal (DAF) - revisão 2.0.1](https://www.sef.sc.gov.br/arquivos_portal/servicos/159/DAF_Especificacao_de_Requisitos_2.0.1.pdf.pdf). A biblioteca possui dependência da biblioteca padrão do C++ e da biblioteca [Asio](https://think-async.com/Asio/). Assim, é possível usá-la em aplicações desenvolvidas para os sistemas operacionais Linux, macOS ou Windows (com os compiladores [MinGW](https://www.mingw-w64.org) ou [MSVC](https://docs.microsoft.com/pt-br/cpp/build/reference/compiling-a-c-cpp-program?view=msvc-170)).

## Ferramentas e bibliotecas necessárias para compilação

- [Git](https://git-scm.com/downloads)
- [CMake](https://cmake.org) 3.14 ou superior
- Compilador C++11 (i.e. g++, msvc, clang)


## Como gerar o sistema de construção (i.e. *Makefile*)

Baixe o código presente neste repositório:
```bash
git clone https://github.com/ifsc-lased/libpdafusb.git
cd libpdafusb
``` 

E execute as instruções abaixo, de acordo com o sistema operacional que estiver usando.

- No Linux ou macOS:
  ```bash
  cmake -S . -B build
  ```
- No Windows com [MinGW](https://www.mingw-w64.org)
  ```bash
  cmake -S . -B build -G "MinGW Makefiles"
  ``` 

## Como compilar a biblioteca e a aplicação exemplo

Para construir o projeto:
```bash
cmake --build build
```

Serão gerados os seguintes arquivos:

- **Biblioteca estática** (*archive*)
  - No Linux ou macOS: 
    - `build/src/libpdafusb.a`
  - No Windows: 
    - `build\src\libpdafusb.a`
- **Aplicação exemplo** para interagir com um DAF  
  - No Linux ou macOS: 
    - `build/exemplo/app-exemplo`
  - No Windows: 
    - `build\exemplo\app-exemplo.exe`

## Para instalar a biblioteca compilada e os arquivos de cabeçalho

Caso queira instalar a biblioteca compilada e os arquivos de cabeçalho, faça:

```bash
cmake --install build --strip --config Release
```
  
## Aplicação para demonstrar o uso da biblioteca

No diretório [exemplo](exemplo/) deste repositório tem-se um pequeno exemplo de como usar esta biblioteca. A aplicação permite enviar pedidos da API DAF que dependem dos comandos de transporte do protocolo PDAF-USB `enviarMensagem` ou  `enviarBinário` (veja Seção 6.2 da [Especificação Técnica de Requisitos do DAF - revisão 2.0.1](https://www.sef.sc.gov.br/arquivos_portal/servicos/159/DAF_Especificacao_de_Requisitos_2.0.1.pdf.pdf)). 

Sintaxe: `app-exemplo <comando> <arquivo> <porta serial> [<arquivo resposta>]`

### Exemplos

Os exemplos abaixo foram escritos considerando o sistema operacional Linux. Faça os ajustes necessários caso esteja usando outro sistema operacional.

#### Comando `enviarMensagem`

O comando `enviarMensagem` do protocolo PDAF-USB é usado quando deseja-se enviar pedidos da API DAF (documentos JSON). Neste exemplo é enviado o pedido `consultarInformacoes` da API DAF para um DAF que está conectado na porta serial `/dev/ttyACM0`. O pedido `consultarInformacoes` consiste de um documento JSON com o conteúdo `{"msg":8}`, que neste exemplo está salvo dentro do arquivo [`pedido.json`](exemplo/mensagens/pedido.json).

```bash
./build/exemplo/app-exemplo mensagem exemplo/mensagens/pedido.json /dev/ttyACM0
```

Se a comunicação acontecer com sucesso com o DAF, então no console deverá ser impresso o resultado do consultar informações. Veja exemplo abaixo:
```
Mensagem enviada ao DAF: {"msg":8}
Resposta do DAF: {"res":0,"daf":"xyFtiTZWQaq_czmVKfBsqA","mop":0,"vsb":2,"hsb":"MoxgVDBf9MdYEQtb97TGchVvA8mh3gkjBE-C2-mzviU","fab":"86096781000185","mdl":"daf-pi","cnt":0,"crt":"-----BEGIN CERTIFICATE-----\nMIICJjCCAaugAwIBAgIUaS3sIKXSF6A7DJbWAJefB3sn95gwCgYIKoZIzj0EAwIw\ndDEMMAoGA1UECgwDU0VGMQ4wDAYDVQQLDAVHRVNBQzELMAkGA1UEBhMCQlIxFzAV\nBgNVBAgMDlNhbnRhIENhdGFyaW5hMRYwFAYDVQQHDA1GbG9yaWFub3BvbGlzMRYw\nFAYDVQQDDA1zZWYuc2MuZ292LmJyMB4XDTIxMDUyODE0MzUxOFoXDTMxMDUyNzE0\nMzUxOFowdDEMMAoGA1UECgwDU0VGMQ4wDAYDVQQLDAVHRVNBQzELMAkGA1UEBhMC\nQlIxFzAVBgNVBAgMDlNhbnRhIENhdGFyaW5hMRYwFAYDVQQHDA1GbG9yaWFub3Bv\nbGlzMRYwFAYDVQQDDA1zZWYuc2MuZ292LmJyMHYwEAYHKoZIzj0CAQYFK4EEACID\nYgAEJ2NZpBT8RNKxVuJoPDtRyHZwATAQCPLZJLpHBEfHYw34oqkoxa8rhWvHbkz0\nrEDO2U+aVEwtOGxChZqFtUTQ2a92sDuNtEscv8Kiq63A38vsBrdkEoXHQmAe5HU1\nPVwcMAoGCCqGSM49BAMCA2kAMGYCMQDpSFdJD8/VGq/yXYEEUoxr0AB8rjcwdL3S\nTQ3ZtdW4j6sam5CSdFhkUKSuvW17UMkCMQC+O8Srmy4sZVBlb1gCOxxEmmwxVxEc\nLPHYbf1rpnfzuq44vqAI8MnRcAnLgtQNmr8=\n-----END CERTIFICATE-----\n","est":"INATIVO","mxd":1000,"ndf":0,"rts":[]}
```

##### Reposta de escrita em arquivo

A biblioteca permite que a resposta seja salva em disco. Para isso, execute a aplicação de exemplo passando como último argumento o caminho do arquivo para escrita da resposta.

```bash
./build/exemplo/app-exemplo mensagem exemplo/mensagens/pedido.json /dev/ttyACM0 resposta.json
```

Se a comunicação acontecer com sucesso com o DAF, então no console será impresso uma mensagem informando que e o resultado do consultar informações foi escrito no arquivo informado. Veja exemplo abaixo:
```
Mensagem enviada ao DAF: {"msg":8}
Resposta do DAF: escrita no arquivo resposta.json
```

#### Comando `enviarBinário` 

O comando `enviarBinário` do protocolo PDAF-USB é usado quando deseja-se enviar uma nova imagem do *firmware* para o DAF. Neste exemplo será enviado um arquivo binário qualquer ao DAF que está conectado na porta serial `/dev/ttyACM0`.

```bash
./build/exemplo/app-exemplo binario imagem.bin /dev/ttyACM0
```

## Para fazer uso desta biblioteca em outros projetos 

Se está escrevendo um projeto com [CMake](https://cmake.org/cmake/help/latest/index.html), é possível baixar e usar esta biblioteca por meio do [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html). Inclua as linhas abaixo dentro do arquivo `CMakeLists.txt` de seu projeto:

```cmake
include(FetchContent)

FetchContent_Declare(pdafusb
    GIT_REPOSITORY https://github.com/ifsc-lased/libpdafusb.git
    GIT_TAG 0.1.0
)

FetchContent_MakeAvailable(pdafusb)

# considerando que appexemplo é o nome do alvo (target)
target_link_libraries(appexemplo PRIVATE pdafusb)
```

<!-- ## Limitações conhecidas

- ? -->

## Facilidades da biblioteca

- A biblioteca possui a funcionalidade de executar a retransmissão do pedido até 3 vezes antes de considerar o DAF inacessível.

## Dependências e bibliotecas de terceiros

| Biblioteca | Uso | Licença |
|------------|-----|---------|
| [Asio C++ Library](https://think-async.com/Asio/) | Para comunicação assíncrona | [Boost Software License 1.0](https://github.com/boostorg/boost/blob/master/LICENSE_1_0.txt) |