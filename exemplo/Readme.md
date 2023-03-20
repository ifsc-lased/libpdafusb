> # Aviso de isenção de responsabilidade
> **Atenção:** Aqui são apresentados os passos para demonstrar o uso da aplicação exemplo disponibilizada neste diretório. O ambiente aqui apresentado, com as soluções de contorno quando não se tem um DAF certificado, não deve ser usado como referência para o desenvolvimento do PAF-DAF.
> 
> A SEF e o IFSC eximem-se de qualquer responsabilidade, direta ou indireta, por perdas ou danos, comprovadamente ou alegadamente, causados pelos artefatos disponibilizados neste repositório. Caso deseje usar os componentes e softwares aqui disponibilizados, você estará fazendo isto exclusivamente por sua conta e risco.

# Aplicação exemplo com a biblioteca libpdafusb

Neste diretório tem-se um pequeno exemplo de como usar a biblioteca disponibilizada neste repositório. A aplicação permite enviar pedidos da API DAF que dependem dos comandos de transporte do protocolo PDAF-USB `enviarMensagem` ou  `enviarBinário` (veja Seção 6.2 da [Especificação Técnica de Requisitos do DAF - revisão 2.0.1](https://www.sef.sc.gov.br/arquivos_portal/servicos/159/DAF_Especificacao_de_Requisitos_2.0.1.pdf.pdf)). 

Sintaxe: `app-exemplo <comando> <arquivo> <porta serial> [<arquivo reposta>]`

Para executar a aplicação exemplo disponibilizada neste diretório é necessário que possua um **Dispositivo Autorizador Fiscal (DAF)** conectado na porta USB do computador onde será executada a aplicação. Caso não tenha um DAF, você poderá usar o [DAF-pi](https://github.com/ifsc-lased/daf-pi), que consiste de uma implementação em Python3 para ser executada em uma placa [Raspberry Pi Zero W](https://www.raspberrypi.com/products/raspberry-pi-zero-w/). 


## Como executar o DAF-pi em um computador com Linux

Caso não tenha uma Raspberry Pi Zero W, é possível executar o DAF-pi em um computador com Linux e fazendo uso do aplicativo [Socat](http://www.dest-unreach.org/socat) (*Multipurpose relay (SOcket CAT)*). 


1. Instale os pacotes Python3 e socat
    ```bash
    sudo apt install python3 python3-venv socat
    ```
2. Executando o `socat` para criar um *relay* entre dois pseudoterminais (`/dev/pts`). 
    ```bash
    socat -d -d pty,raw,echo=0 pty,raw,echo=0
    ```
    - Após executar o código acima será impresso os pseudoterminais que foram associados. No exemplo abaixo foi criado um *relay* entre os pseudoterminais `/dev/pts/2` e `/dev/pts/3`.
        ```bash
        2022/06/03 15:24:55 socat[544737] N PTY is /dev/pts/2
        2022/06/03 15:24:55 socat[544737] N PTY is /dev/pts/3
        2022/06/03 15:24:55 socat[544737] N starting data transfer loop with FDs [5,5] and [7,7]
        ```
    - Um dos pseudoterminais será usado pelo DAF-pi (i.e. `/dev/pts/2`) e o outro deverá ser informado como argumento de linha de comando para aplicação exemplo disponível neste diretório (i.e. `/dev/pts/3`).
    - Atenção, é necessário deixar o socat em execução.
3. Em um outro terminal baixe o código do DAF-pi e baixe as dependências do projeto
    ```bash
    git clone https://github.com/ifsc-lased/daf-pi.git
    cd daf-pi
    python3 -m venv venv
    source venv/bin/activate
    pip install --upgrade pip
    pip install -r requirements.txt
    ```
4. Edite o arquivo [`app.py`](https://github.com/ifsc-lased/daf-pi/blob/bb25e7e9233667f11d449bee811db8cb671fc096/app.py#L17) e substitua o valor `/dev/ttyGS0` que está associado ao atributo `port` por um dos pseudoterminais que o socat está usando. Neste exemplo será usado `/dev/pts/2`.
5. Execute a aplicação DAF-pi
    ```bash
    python3 app.py
    ```

## Executando a aplicação para demonstrar o uso da biblioteca

1. Baixe o código e compile
   ```bash
   git clone https://github.com/ifsc-lased/libpdafusb.git
   cd libpdafusb
   cmake -S . -B build
   cmake --build build
   ```
2. Execute a aplicação para enviar o comando `enviarMensagem` ao DAF que, neste exemplo, estará no pseudoterminal associado ao `/dev/pts/3`

    A biblioteca permite que a resposta seja salva em disco. A aplicação de exemplo externaliza essa funcionalidade com a inclusão do caminho do arquivo para resposta como último argumento do comando. Caso não seja informado, a resposta será salva em memória (buffer) e impressa no console.

   * Comando com a resposta armazenada em buffer.

        ```bash
        ./build/exemplo/app-exemplo mensagem exemplo/mensagens/pedido.json /dev/pts/3
        ```
        Se a comunicação acontecer com sucesso com o DAF, então no console deverá ser impresso o resultado do consultar informações. Veja exemplo abaixo:
        ```
        Mensagem enviada ao DAF: {"msg":8}
        Resposta do DAF: {"res":0,"daf":"xyFtiTZWQaq_czmVKfBsqA","mop":0,"vsb":2,"hsb":"MoxgVDBf9MdYEQtb97TGchVvA8mh3gkjBE-C2-mzviU","fab":"86096781000185","mdl":"daf-pi","cnt":0,"crt":"-----BEGIN CERTIFICATE-----\nMIICJjCCAaugAwIBAgIUaS3sIKXSF6A7DJbWAJefB3sn95gwCgYIKoZIzj0EAwIw\ndDEMMAoGA1UECgwDU0VGMQ4wDAYDVQQLDAVHRVNBQzELMAkGA1UEBhMCQlIxFzAV\nBgNVBAgMDlNhbnRhIENhdGFyaW5hMRYwFAYDVQQHDA1GbG9yaWFub3BvbGlzMRYw\nFAYDVQQDDA1zZWYuc2MuZ292LmJyMB4XDTIxMDUyODE0MzUxOFoXDTMxMDUyNzE0\nMzUxOFowdDEMMAoGA1UECgwDU0VGMQ4wDAYDVQQLDAVHRVNBQzELMAkGA1UEBhMC\nQlIxFzAVBgNVBAgMDlNhbnRhIENhdGFyaW5hMRYwFAYDVQQHDA1GbG9yaWFub3Bv\nbGlzMRYwFAYDVQQDDA1zZWYuc2MuZ292LmJyMHYwEAYHKoZIzj0CAQYFK4EEACID\nYgAEJ2NZpBT8RNKxVuJoPDtRyHZwATAQCPLZJLpHBEfHYw34oqkoxa8rhWvHbkz0\nrEDO2U+aVEwtOGxChZqFtUTQ2a92sDuNtEscv8Kiq63A38vsBrdkEoXHQmAe5HU1\nPVwcMAoGCCqGSM49BAMCA2kAMGYCMQDpSFdJD8/VGq/yXYEEUoxr0AB8rjcwdL3S\nTQ3ZtdW4j6sam5CSdFhkUKSuvW17UMkCMQC+O8Srmy4sZVBlb1gCOxxEmmwxVxEc\nLPHYbf1rpnfzuq44vqAI8MnRcAnLgtQNmr8=\n-----END CERTIFICATE-----\n","est":"INATIVO","mxd":1000,"ndf":0,"rts":[]}
        ```

   * Comando com a resposta armazenada em arquivo.
        ```bash
        ./build/exemplo/app-exemplo mensagem exemplo/mensagens/pedido.json /dev/pts/3 resposta.json
        ```

        Se a comunicação acontecer com sucesso com o DAF, então no console será impresso uma mensagem informando que e o resultado do consultar informações foi escrito no arquivo informado. Veja exemplo abaixo:
        ```
        Mensagem enviada ao DAF: {"msg":8}
        Resposta do DAF: escrita no arquivo resposta.json
        ```