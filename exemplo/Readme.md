# Aplicação exemplo com a biblioteca libpdafusb

> Essa aplicação requer um **Dispositivo Autorizador Fiscal (DAF)** conectado na porta USB do computador onde será executada a aplicação. Caso não tenha um DAF, mas queira testar essa aplicação, então você poderá usar o [DAF-pi](https://github.com/ifsc-lased/daf-pi).

A aplicação disponibilizada aqui permite enviar pedidos da API DAF que dependem dos comandos de transporte do protocolo PDAF-USB `enviarMensagem` ou  `enviarBinario` (veja Seção 6.2 da [Especificação Técnica de Requisitos do DAF - revisão 3.0.0](https://www.sef.sc.gov.br/arquivos_portal/servicos/159/Especificacao_de_Requisitos_do_DAF___versao_3.0.0.pdf)). 

Sintaxe: 

`app-exemplo <comando> <arquivo> <porta serial> [<arquivo para armazenar a resposta do DAF>]`

- O comando pode ser `mensagem` ou `binario`
- O arquivo pode ser um arquivo texto (documento JSON) ou binário
- A porta serial onde tem um DAF conectado
- A resposta do DAF pode ser impressa no console (comportamento padrão) ou salva em um arquivo

Os exemplos abaixo foram escritos considerando o sistema operacional Linux e com o DAF conectado na porta serial `/dev/ttyACM0`. Faça os ajustes necessários caso esteja usando outro sistema operacional ou caso o DAF esteja conectado em outra porta serial.

### Comando `enviarMensagem` da API DAF

O comando `enviarMensagem` do protocolo PDAF-USB é usado quando deseja-se enviar pedidos da API DAF (documentos JSON). Neste exemplo é enviado o pedido `consultarInformacoes`, que consiste de um documento JSON com o conteúdo `{"msg":8}` (veja o arquivo [`exemplo/mensagens/pedido.json`](exemplo/mensagens/pedido.json)). 

```bash
./build/exemplo/app-exemplo mensagem exemplo/mensagens/pedido.json /dev/ttyACM0
```

Se a comunicação acontecer com sucesso com o DAF, então no console deverá ser impresso o resultado do consultar informações. Veja exemplo abaixo:
```
Mensagem enviada ao DAF: {"msg":8}
Resposta do DAF: {"res":0,"daf":"pxZkogT1RBaGaKhn_4w_kQ","mop":0,"vsb":2,"sig":"MGUCMQCo6EID9wl4l0F8r4fnBfgGiLckVsqgoFd0zL00DC03Ny3e57X3f3l19r35L-XJFPoCMGRFJKIl399mDp7yH-4oyuy4VJ0kkaGOPt4AkXQ4zZF4aRQKzKzN4yuhBx48L705Sg","fab":"86096781000185","mdl":"daf-pi","cnt":0,"cfp":"SdFcN6lp3DImbcsgZBOPvXc2HMdBC3juLkD6ee4Fz5w","est":"INATIVO","mxd":1000,"ndf":0}
```
### Comando `enviarBinario` da API DAF

O comando `enviarBinario` do protocolo PDAF-USB é usado quando deseja-se enviar uma nova imagem do *firmware* para o DAF. Neste exemplo será enviado um arquivo binário qualquer ao DAF.

```bash
./build/exemplo/app-exemplo binario imagem.bin /dev/ttyACM0
```