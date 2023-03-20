cmake_minimum_required(VERSION 3.14)

project(
    pdafusb 
    VERSION 0.1.0
    DESCRIPTION "Implementação do protocolo PDAF-USB da Especificação 2.0.1 do DAF"
    LANGUAGES CXX)

# Só faça isso se for o projeto principal e ignore quando for incluído por add_subdirectory
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # provendo suporte a pasta pelas IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Buscando por bibliotecas pré-requisitos
find_package(Threads REQUIRED)

# Adicionando dependências por meio do CPM.cmake 
# https://github.com/cpm-cmake/CPM.cmake
include(cmake/CPM.cmake)

CPMAddPackage("gh:chriskohlhoff/asio#asio-1-22-1@1.22.1")
include(cmake/asio.cmake)

# Código da biblioteca
add_subdirectory(src)

# Código com aplicação exemplo
add_subdirectory(exemplo)
