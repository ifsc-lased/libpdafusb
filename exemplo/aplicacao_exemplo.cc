/**
 * @file aplicacao_exemplo.cc
 * @brief Este arquivo possui uma pequena aplicação para exemplificar o uso da biblioteca pdafusb.
 * Estão aqui apresentados dois tipos de comunicação com o DAF, binária e por mensagem de texto.
 * @date 2022-07-04
 *
 */

#include <pdafusb/pdaf.hh>
#include <iostream>
#include <string>
#include <fstream>

using std::cout;
using std::endl;
using std::string;

/**
 * @brief Função utilizada para enviar uma mensagem ao DAF. Ela recebe uma instância da classe Pdaf,
 * o caminho do arquivo que possui a mensagem que será enviada ao DAF e 
 * o caminho do arquivo para armazenar a resposta.
 * Por meio da função Pdaf::envia_texto é efetuado a transmissão da mensagem.
 *
 * @param daf Instância do Pdaf
 * @param path string com o caminho do arquivo JSON contendo o pedido
 * @param arquivo_resposta string com o caminho para arquivo que será armazenada a resposta,
 *                         caso a string for vazia, a resposta é armazenada no buffer.
 */
void enviar_mensagem(pdafusb::Pdaf *daf, string path, string arquivo_resposta)
{
    std::ifstream arquivo(path);
    if (!arquivo.is_open())
    {
        cout << "Arquivo '" << path << "' não foi encontrado" << endl;
        exit(-1);
    }

    // Lendo todas as linhas do arquivo texto
    // Não é feito nenhum tratamento para garantir que o arquivo
    // contém um documento JSON. Se for necessário fazer tal validação
    // veja a biblioteca: https://github.com/nlohmann/json
    std::ostringstream ss;
    ss << arquivo.rdbuf();
    string msg = ss.str();

    arquivo.close();

    cout << "Mensagem enviada ao DAF: " << msg << endl;

    size_t len = 0;
    string resposta;
    if (arquivo_resposta.empty())
    {
        uint8_t buffer_mensagem_recebida[pdafusb::PdafUtils::kMaxBuffer];
        // Enviando mensagem ao DAF;
        len = daf->envia_texto(msg, buffer_mensagem_recebida);
        resposta = (char *)buffer_mensagem_recebida;
    }
    else
    {
        std::ofstream ofs(arquivo_resposta, std::ofstream::out);
        // Enviando mensagem ao DAF;
        len = daf->envia_texto(msg, &ofs);
        ofs.close();
        resposta = "escrita no arquivo " + arquivo_resposta;
    }
    if (len > 0)
    {
        
        cout << "Resposta do DAF: " << resposta << endl;
    }
    else
    {
        cout << "Sem resposta do DAF" << endl;
        exit(-1);
    }
}

/**
 * @brief Função utilizada para enviar um arquivo binário ao DAF.
 * Ela recebe uma instância da classe Pdaf e o caminho do arquivo binário.
 * É utilizado a função Pdaf::envia_binario para enviar o arquivo.
 *
 * @param daf Instância Pdaf.
 * @param path Caminho do arquivo.
 */
void enviar_binario(pdafusb::Pdaf *daf, string path)
{

    uint8_t buffer_mensagem_recebida[pdafusb::PdafUtils::kMaxBuffer];
    size_t len = 0;
    // Envia arquivo binário ao DAF;
    len = daf->envia_binario(path, buffer_mensagem_recebida);
    
    string resposta;

    if (len > 0)
    {
        resposta = (char *)buffer_mensagem_recebida;
    }
    else
    {
        cout << "Sem resposta do DAF" << endl;
        exit(-1);
    }
    cout << "Resposta do DAF: " << resposta << endl;
}

/**
 * @brief Função para tratar erros de argumentos inválidos.
 *
 * @param argv Argumentos recebidos
 */
void sintaxeInvalida(char *argv[])
{
    cout << "Sintaxe: " << argv[0] << " <mensagem|binario> "
         << "<caminho do arquivo> <porta USB>\n"
         << endl;
    cout << "Exemplos:" << endl;
    cout << " - Enviar mensagem: \n   " << argv[0] << " mensagem arquivo.json /dev/ttyACM0\n"
         << endl;
    cout << " - Enviar mensagem (resposta em arquivo): \n   " << argv[0] << " mensagem arquivo.json /dev/ttyACM0 resposta.json\n"
         << endl;
    cout << " - Enviar binário: \n   " << argv[0] << " binario imagem.dat /dev/ttyACM0" << endl;
    exit(-1);
}

/**
 * @brief Inicialização da aplicação.
 *
 * @param argv[1] Comando do tipo da transmissão: <mensagem|binario>
 * @param argv[2] Caminho do arquivo: /caminho/do/arquivo
 * @param argv[3] Porta serial onde o DAF está conectado: /dev/ttyACM0
 * @param argv[4] (Opcional) - Caminho do arquivo para resposta do comando mensagem: /caminho/resposta.json
 */
int main(int argc, char *argv[])
{
    string port;
    string path;
    string arquivo_resposta;
    string mensagem_json;

    if (argc < 4)
    {
        sintaxeInvalida(argv);
    }
    path = argv[2];
    port = argv[3];
    arquivo_resposta = (argc == 5) ? argv[4] : "";

    // Inicializando o Pdaf que é o controlador das camadas Arq e Enquadramento.
    // O primeiro parâmetro é o timeout desta camada em milisegundos (mínimo 2000) 
    // para utilização da funcionalidade de retransmissão.
    // O segundo parâmetro é o caminho da porta onde está conectado o DAF.
    pdafusb::Pdaf daf(2000, port);

    if (strcmp(argv[1], "mensagem") == 0)
    {
        enviar_mensagem(&daf, path, arquivo_resposta);
    }
    else if (strcmp(argv[1], "binario") == 0)
    {
        enviar_binario(&daf, path);
    }
    exit(0);
}