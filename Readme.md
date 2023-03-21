[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Ubuntu](https://github.com/ifsc-lased/libpdafusb/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/ifsc-lased/libpdafusb/actions/workflows/ubuntu.yml)
[![macOS](https://github.com/ifsc-lased/libpdafusb/actions/workflows/macos.yaml/badge.svg)](https://github.com/ifsc-lased/libpdafusb/actions/workflows/macos.yaml)
[![Windows](https://github.com/ifsc-lased/libpdafusb/actions/workflows/windows.yaml/badge.svg)](https://github.com/ifsc-lased/libpdafusb/actions/workflows/windows.yaml)

> # Aviso de isenção de responsabilidade
> O código presente nesse repositório está sendo disponibilizado publicamente para demonstrar uma forma de implementar, na linguagem C++, o protocolo PDAF-USB de acordo com a Especificação Técnica de Requisitos do Dispositivo Autorizador Fiscal (DAF) - revisão 2.0.1. Contudo, o código não foi amplamente testado e validado de forma a garantir um funcionamento correto, confiável e seguro. Sendo assim, o código aqui disponibilizado não deve ser considerado como uma implementação de referência para o PDAF-USB.
>
> Não cabe à SEF ou ao IFSC prestar suporte sobre os componentes, códigos ou excertos de código disponíveis nesse repositório, sendo a presente versão considerada final, sem previsão de alterações, correção de bugs ou melhorias.
>
> A SEF e o IFSC eximem-se de qualquer responsabilidade, direta ou indireta, por perdas ou danos, comprovadamente ou alegadamente, causados pelo código aqui disponibilizado. Caso deseje usar essa biblioteca em seu projeto ou produto, você estará fazendo isto exclusivamente por sua conta e risco.

# Biblioteca PDAF-USB em C++

Biblioteca desenvolvida na linguagem C++ que implementa o protocolo PDAF-USB de acordo com a [Especificação Técnica de Requisitos do Dispositivo Autorizador Fiscal (DAF) - revisão 2.0.1](https://www.sef.sc.gov.br/arquivos_portal/servicos/159/DAF_Especificacao_de_Requisitos_2.0.1.pdf.pdf). A biblioteca possui dependência da biblioteca padrão do C++ e da biblioteca [Asio](https://think-async.com/Asio/). Assim, é possível usá-la em aplicações desenvolvidas para os sistemas operacionais Linux, macOS ou Windows (com os compiladores [MinGW](https://www.mingw-w64.org) ou [MSVC](https://docs.microsoft.com/pt-br/cpp/build/reference/compiling-a-c-cpp-program?view=msvc-170)).

No diretório [exemplo](exemplo/) deste repositório tem-se uma aplicação para demonstrar o uso dessa biblioteca. A aplicação permite enviar pedidos da API DAF que dependem dos comandos de transporte do protocolo PDAF-USB `enviarMensagem` ou  `enviarBinário` (veja Seção 6.2 da [Especificação Técnica de Requisitos do DAF - revisão 2.0.1](https://www.sef.sc.gov.br/arquivos_portal/servicos/159/DAF_Especificacao_de_Requisitos_2.0.1.pdf.pdf)). 

## Ferramentas e bibliotecas necessárias para compilação

- [Git](https://git-scm.com/downloads)
- [CMake](https://cmake.org) 3.14 ou superior
- Compilador C++11 (i.e. g++, msvc, clang)

## Como compilar e instalar

1. Baixar o código presente neste repositório:
    ```bash
    git clone https://github.com/ifsc-lased/libpdafusb.git
    cd libpdafusb
    ``` 
1. Gerar o sistema de construção (i.e. *Makefile*)
   - No Linux ou macOS:
    ```bash
    cmake -S . -B build
    ```
   - No Windows com [MinGW](https://www.mingw-w64.org)
    ```bash
    cmake -S . -B build -G "MinGW Makefiles"
    ``` 
1. Compilar a biblioteca e a [aplicação exemplo](exemplo/) (veja as funcionalidades da aplicação [aqui](exemplo/Readme.md))
    ```bash
    cmake --build build
    ```
   - Serão gerados os seguintes arquivos:
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

### Como instalar a biblioteca

Caso queira instalar a biblioteca, bem como os seus respectivos os arquivos de cabeçalho, faça:

```bash
cmake --install build --strip --config Release
```
## Como fazer uso desta biblioteca no seu projeto

Se está escrevendo um projeto com [CMake](https://cmake.org/cmake/help/latest/index.html), é possível baixar e usar esta biblioteca por meio do [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html). Inclua as linhas abaixo dentro do arquivo `CMakeLists.txt` do seu projeto:

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

Caso no seu projeto você não faça uso do [CMake](https://cmake.org/cmake/help/latest/index.html), ainda assim é possível usar essa biblioteca, porém terá que copiar manualmente os arquivos presentes nesse repositório.

## Dependências e bibliotecas de terceiros

| Biblioteca | Uso | Licença |
|------------|-----|---------|
| [Asio C++ Library](https://think-async.com/Asio/) | Para comunicação assíncrona | [Boost Software License 1.0](https://github.com/boostorg/boost/blob/master/LICENSE_1_0.txt) |