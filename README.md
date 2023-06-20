# Sistemas Digitais - IoT: A Internet das Coisas

* * *

## Autores: 
  * Carlos Valadão [@arlosValadao](https://github.com/arlosValadao)
  * Fernando Mota [@f3rn4nd0000](https://github.com/f3rn4nd0000)
  * Silas Silva [@silas-silva](https://github.com/silas-silva)

* * *

## Seções 

&nbsp;&nbsp;&nbsp;[**1.** Introdução](#introducao)

&nbsp;&nbsp;&nbsp;[**2.** Fundamentação Teórica](#fundamentacao_teorica)

&nbsp;&nbsp;&nbsp;[**3.** Caracterização da Solução](#caracterizacao_solucao)

&nbsp;&nbsp;&nbsp;[**4.** Casos de Teste](#testes)

&nbsp;&nbsp;&nbsp;[**5.** Conclusão](#conclusao)

&nbsp;&nbsp;&nbsp;[**6.** Referências](#referencias)


# <a id="introducao"></a>
## 1. Introdução


A comunicação entre dispositivos é algo essencial nos tempos modernos, contudo cada tipo de aplicação possui características intrínsecas e requerem uma carga de mensagem particular. Uma vez que a maioria expressiva dos computadores atuais utilizam o padrão de comunicação Ethernet, existem diferentes protocolos de comunicação para diferentes tipos de aplicação, por um lado existem os protocolos troca de informação HTTP, HTTPS, FTP idealmente desenvolvidos para aplicações que comportam uma carga de mensagem de baixa intensidade, todavia estes protocolos possuem demasiado overhead, que culminam em atraso na comunicação entre os dispositivos envolvidos - visto que existe delay em toda comunicação - por outro lado existem protocolos de mensageria que são dedicados à programas que nativamente adequam um fluxo intenso de mensagem entre os dispositivos. O protocolo de comunicação Transporte de Telemetria de Enfileiramento de Mensagens (MQTT), extremamente leve, é um destes, podendo operar de forma assíncrona ou síncrona, fora utilizado no desenvolvimento deste projeto, vista a necessidade de uma troca de mensagens de forma contínua, sendo que é possível gerenciar quantidades variadas de unidades de sensoriamento conectadas à um computador de placa única, tanto serial quanto MQTT.

Partindo desse pressuposto, este documento apresenta em detalhes o desenvolvimento de um sistema de comunicação sobre a Ethernet (Pilha TCP/IP), assíncrona half-duplex, entre um computador de placa única Orange PI PC Plus e até 32 unidades de sensoriamento conectadas a diversos sensores, sendo estas representadas por Node MCUs. Visto que este problema utiliza por completo o produto desenvolvido no problema anterior (problema 2), é fato que o protocolo de comunicação até então desenvolvido fora reaproveitado, contudo na comunicação MQTT entre a SBC Orange e demais ESP8266. A solução permite monitorar todos os sensores das respectivas unidades de sensoriamento de forma individual, bem como monitorá-los, além de permitir monitorar todas as unidades conectadas por meio da comunicação serial (meio físico) ou wireless (via MQTT), é parte do sistema desenvolvido uma Interface Homem Máquina, capaz de disponibilizar em forma de gráfico de linha o histórico de medição de todos os sensores conectados a uma respectiva unidade de sensoriamento, de forma individual, ou de todas as unidades gerenciadas pela unidade de centralizadora, todos os dados recebidos são exibidos em um display LCD 16x2 e em uma Interface Homem Máquina (Interface Gráfica), em forma de gráfico, o computador de placa única Orange PI, a única entidade capaz de inicializar uma comunicação neste projeto desenvolvido. Este trabalho está organizado da seguinte forma: A seção 2 discorre sobre os principais conceitos no qual esta solução está fundamentada, a seção 3 dispõe a descrição detalhada da solução desenvolvida, por sua vez a seção 3 apresenta os casos de testes possíveis sobre o produto final desenvolvido, e por fim, a seção 3 dispõe das conclusões tiradas após após a completa criação do produto.

# <a id="fundamentacao_teorica"></a>
## 2. Fundamentação Teórica
Nesta seção serão abordados Internet das Coisas (IOT), Transporte de Telemetria de Enfileiramento de Mensagens (MQTT) e Interface Homem Máquina (IHM), conceitos estes irrevogáveis para a construção da solução desenvolvida.

### 2.1. Internet das coisas
Faz-se necessário o conhecimento sobre Internet das Coisas para evoluir na solução deste problema, uma vez que os dispositivos envolvidos são nativamente dispositivos de internet das coisas e devem estar conectados, dada a arquitetura do problema.

A premissa básica do IOT é "connect the unnconnected", isto significa que objetos que não fazem parte de uma rede, comumente a Internet, podem se conectar e se comunicar e interagir com outras entidades. IoT pode ser entendida como uma tecnologia de transmissão ao qual os dispositivos controlados permitem serem controlados e habilita os seres humanos controlar o mundo físico por meio deles. O mundo IoT é multifacetado e diversificado, a Internet das Coisas conta com diversas tecnologias e protocolos de comunicação próprios, tais como o protocolo MQTT, WiFi, NFC dentre outros.

Dispositivos e "coisas" são conectados à internet e outras redes por décadas. Tecnologias como caixa eletrônico (ATM), redes de sensores sem fio (WSN), máquina para máquina (M2M) e outros dispositivos conectados já são conhecidos. Contudo, isso não significa que todos esses sistemas são por definição o que é atualmente a IoT, ou seja, nem todos os dispositivos conectados são IoT, mas todos dispositivos IoT são dispositivos conectados, atualmente dispositivos IoT utilizam Ethernet com IPV4. Comumente uma Interface Homem Máquina (IHM) é utilizada para  elevar o grau de conectividade de uma aplicação IoT ou não IoT, permitindo uma alta personalização de telas e uma facilidade de configuração.


### 2.2. Interface Homem Máquina (IHM)
Uma interface influencia a forma como os usuários manipulam o seu sistema, e uma interface bem projetada é fácil de utilizar e traz segurança aos usuaŕios que a utilizam. O objetivo de uma Interação Homem-Computador (IHC) é de produzir sistemas utilizáveis, funcionais e seguros. Sendo assim, o termo sistemas refere-se a hardware, software e todo ambiente que usa ou é afetado pelo uso de tecnologia computacional. Na figura 1 é exibida uma Interface Homem Máquina de Linha de Comando (CLI), do sistema operacional GNU/Linux, é possível observar que a CLI interpreta os comandos e repassa ele para o Kernel que logo transmite a mensagem para o hardware que finalmente executa o comando.

![cli_linux](https://github.com/arlosValadao/P3-SD/assets/61719667/1044d9c0-d1bf-4e2e-838f-bdb14c548e38)

### 2.3. Transporte de Telemetria de Enfileiramento de Mensagens (MQTT)

MQTT é um protocolo de conectividade IoT, MQTT é um protocolo de peso leve que funciona com um intermediador (broker), baseado em broker baseado em um mecanismo de publicação de subscrição e é construído sobre o Protocolo de Controle de Transmissão da família IP (TCP/IP). Conforme a figura 2, abaixo que exibe a pilha TCP/IP com o MQTT incluso.

![mqtt_stack_tcpip-removebg-preview](https://github.com/arlosValadao/P3-SD/assets/61719667/26c2bfd3-8f40-4729-906e-593e8112d68f)

Mais leve que o protocolo HTTP 1.1, e portanto uma opção muito interessante quando deseja-se receber e enviar dados ao longo do tempo em tempo real, e em um modelo de publicação e subscrição é necessário um esforço mínimo para tal. MQTT é muito popular na Internet das coisas, e projetos embarcados, mas também está presente em aplicações Web e Mobile que requisitam um alto fluxo de mensagens e uma distribuição eficiente destas mensagens. Logo o MQTT é uma ótima opção quando é preciso uma troca constante de informações.

Sendo assim o protocolo MQTT foi projetado para adequar-se aos mais diversos tipos de aplicação, ele foi feito para: ser leve e tornar possível um alto volume de mensagens e de dados sem grandes overheads, distribuir uma quantidade mínima de pacotes de dados em altos volumes de mensagens, suportar paradigmas orientados a eventos com entrega de mensagens assíncronas bidirecionais com baixa latência de entrega e trabalhar com eficiência em dispositivos com baixa capacidade de processamento e baixo consumo de bateria, desta forma o MQTT torna-se um dos protocolos mais adequados ao lidar com Internet das coisas.

Em um padrão de subscrição e publicação (pub-sub pattern), um cliente que publica uma mensagem é desacoplado de outro cliente ou o cliente que recebe a mensagem, e um cliente não sabe a existência do outro cliente. Um cliente pode publicar mensagens de um tipo específico e apenas os clientes que estão interessados naquele tipo de mensagem poderá recebê-la.

O pub-sub pattern requer a existência de um broker, também conhecido como um servidor, responsável por intermediar a comunicação entre os clientes MQTT. Todos os clientes devem estabelecer uma conexão com o broker, então o cliente que envia a mensagem por meio do broker é conhecido como publicador (publisher). O broker filtra as mensagens recebidas e as distribui para os clientes interessados nestas. O cliente que se registra em um broker interessado em uma mensagem de um tipo específico é chamado de subscriber (subscrito). É possível perceber o funcionamento do protocolo MQTT observando a figura 3. A Raspberry PI 3 deseja publicar os dados do seu sensor de altitude, e existem dois dispositivos interessados nessa mensagem, Smartphone IOS e um Tablet Android logo tanto a Raspberry, tanto o Smartphone quanto o tablet deve estabelecer uma conexão com o broker. Raspberry publicará a informação no tópico "sensor1/altitude", localizado no broker e os dois subscritores irão se inscrever no mesmo tópico onde a informação foi publicada "sensor1/altitude" e receberão a mensagem. Portanto, a informação publicada será entregue apenas àquelas interessados e subscritos no tópico ao qual a informação foi enviada. 

![mqtt_pub_sub_first_ex-removebg-preview](https://github.com/arlosValadao/P3-SD/assets/61719667/565a1ffd-4b51-4736-8488-fe97816af53d)

# <a id="caracterizacao_solucao"></a>
## 3. Caracterização da Solução


A solução é capaz de suportar até 32 unidades de sensoriamento simultaneamente, conectadas a um SBC via UART ou MQTT - somando um total de 32 unidades. As unidades de sensoriamento UART podem assumir endereços de 1 até 16 e as unidades de sensoriamento MQTT podem assumir endereços de 1 até 16. O computador de placa única Orange PI está conectado a todas as unidades de sensoriamento e apenas ele é capaz de inicializar uma comunicação, também uma Interface Homem máquina é capaz de exibir o histórico de medição dos sensores de todas as unidades onlines de forma individualizada ou não, além de poder exibir os valores dos sensores de cada unidade de forma individual. A figura 4 mostra como os dispositivos estão conectados, neste exemplo o SBC está conectado a uma unidade de medição por meio de comunicação serial - UART - e em outras duas unidades de sensoriamento via MQTT. A comunicação MQTT ocorre de acordo com o protocolo desenvolvido no problema anterior, de tal forma que as requisições a todas as Nodes  conectadas são feitas por meio de um tópico chamado “MQTTNode”, onde todas as ESP8266 são subscritas, enquanto as respostas de todas as Nodes são publicadas em um tópico 
“MQTTSBC”, onde a Orange PI é subscrita. Por outro lado a interface é subscrita no tópico “MQTTSBC” por onde obtém os dados para construir os gráficos dos sensores e das unidades disponíveis, além do LCD conectado ao SBC por meio dos seus pinos GPIO.

![diagrama_de_blocos_geral-removebg-preview](https://github.com/arlosValadao/P3-SD/assets/61719667/25cdafab-29c3-4c65-b9f7-108b759cdeb2)

Para a criação da solução foram utilizadas as linguagens de programação C e Python e o Ambiente de Desenvolvimento Integrado Arduino (IDE Arduino). A criação da IHM deu-se por meio do framework Flask da linguagem Python, enquanto para a elaboração da comunicação MQTT por parte do computador de placa única foi utilizada a biblioteca paho para a linguagem C, também fora utilizada a biblioteca wiringOP para a implementação da comunicação entre os pinos GPIO da Orange PI e os push buttons e o LCD 16x2, assim como a biblioteca Pubsub do arduino  IDE foi utilizada para implementar a comunicação MQTT por parte das unidades de sensoriamento ESP 8266.
Uma vez que todo o protocolo anteriormente desenvolvido fora aproveitado, todas as macros foram utilizadas para estabelecer uma comunicação entre as unidades de medição (ESP 8266) e o computador de placa única, assim como o menu desenvolvido no problema de interface de entrada e saída foi utilizado neste problema, permanecendo assim o mesmo layout. Contudo fez-se necessária a criação de uma Interface Homem Máquina


### 3.2 Menu Interativo

O menu desenvolvido é uma extensão do menu desenvolvido no problema anterior, sendo assim, este fork é capaz de exibir com uma indicação as unidades de medição MQTT, conforme a figura 5, abaixo, é possível perceber que existe uma unidade de sensoriamento com Id 1 e que está conectada por meio do protocolo de comunicação MQTT.

![menu_mqtt](https://github.com/arlosValadao/P3-SD/assets/61719667/eac25d72-8f94-4004-ae3d-560e32c05319)

Dessa forma, este menu goza de todas as mesmas possibilidades de consulta e monitoramento do problema de Interfaces de E/S [LINK].


### 3.2 Interface Homem Máquina

Para uma melhor visualização das informações provenientes das unidades de sensoriamento conectadas via MQTT, apenas, deu-se o desenvolvimento de uma interface gráfica capaz de exibir de forma clara e dinâmica os dados dos sensores de cada NodeMCU, de forma individual ou coletiva, simultaneamente ou não. A figura 6 ilustra de forma clara o funcionamento da interface gráfica, nela é possível analisar a curva de crescimento do sensor analógico (Potenciômetro B20k), bem como a de 2 push buttons.

![swappy-20230619_225559](https://github.com/arlosValadao/P3-SD/assets/61719667/ae84c666-e739-4388-a4e4-83a07172aa86)

A interface é acessada por meio do Browser, nela é possível analisar 32 unidades - somando MQTT e UART - bastando selecionar a unidade desejada, conforme a figura 7, é possível perceber a existência de 3 unidades de sensoriamento online, “node 1”, “node 2” e “node 3”.

[ IMAGEM DA PAGINA PRINCIPAL DA INTERFACE ]


Na imagem 8 é possível notar o gráfico dos sensores analógicos e digitais da “node 1”.

![swappy-20230619_225738](https://github.com/arlosValadao/P3-SD/assets/61719667/986de18f-e7a8-4c44-877e-2844aa701576)

As informações representadas na interface não possuem relação com as informações exibidas no menu, uma vez que as informações exibidas no display LCD 16x2 (menu) não são sincronizadas com a interface, sendo assim as informações contidas na interface provém de quando as unidades de medição não estão sendo consultadas pelo usuário por meio do menu do LCD. Destarte as informações são colhidas de todas as unidades conectadas via MQTT, e enviadas via MQTT para a IHM em formato JSON de forma constante, por meio de uma Thread, suspendendo-a ao selecionar qualquer placa por meio do LCD. A colheita dos dados é feita da seguinte forma: A unidade cujo o dado a ser extraído é selecionada, consultada e logo após desselecionada, estes passos se repetem até que não restem unidades “nã-visidatas”. 


# <a id="testes"></a>
## 4. Casos de Teste

A fim de averiguar o funcionamento do sistema desenvolvido foram realizados 3 testes que submetem o sistema a 3 cenários distintos. Todos os testes foram realizados por meio do menu disponível no LCD e unidades de sensoriamento MQTT e UART.

### 4.1 Primeiro Teste

No primeiro teste o monitoramento de todas as unidades disponíveis, tanto UART quanto MQTT foi feito. Como é possível ver no vídeo 1, por meio do menu, no menu principal é selecionada a opção “Monitor All”, logo após o processo de monitoramento inicia-se, logo após a única unidade UART disponível é selecionada, e uma mensagem “NODE SELECTED” é exibida no display LCD, simbolizando sucesso ao selecionar tal unidade, logo após as informações dos 3 sensores são exibidas em sequência, “D0”, o sensor digital 0, (push button esquerdo), “D1” (push button direito) e o sensor analógico - A0, após a exibição das informações da unidade UART o sistema parte para as outras unidades de sensoriamento MQTT, uma mensagem “DESELECT NODE” é mostrada indicando sucesso ao deselecionar a unidade UART previamente selecionada. Logo o LED da unidade MQTT é aceso, indicando que ele foi selecionada, a mensagem “NODE SELECTED” é exibida no display LCD indicando sucesso ao selecionar a unidade, feito isso as informações da unidade MQTT são exibidas no display, seguindo o mesmo padrão da(s) unidade(s) UART. O mesmo processo se mantém para as outras unidades MQTT conectadas.

[ MONITOR ALL ]

Ao analisar, os valores de D0 igual a 0,  e D1 igual a 1, da única unidade de medição UART - de id 1 - reiterando o correto funcionamento do sistema nesse aspecto, levando em consideração que os botões conectados à unidade de medição funcionam em lógica inversa, ou seja quando pressionados assumem nível lógico baixo (0), bem como o valor correspondente em “A0”: 703, é um valor inteiro sem convertido pela própria unidade de medição e pode variar de 0-1023.


### 4.2 Segundo Teste
Enquanto no segundo teste é utilizada uma unidade MQTT, bem como o menu LCD. Como pode-se notar no vídeo 2, a unidade MQTT de Id 2 é selecionada, e logo após uma mensagem é exibida “Success on selecting the unit” indicando sucesso ao selecionar a unidade, por conseguinte é navegado até a opção “Monitoring all of them” é escolhida, esta realiza o monitoramento de todos os 3 sensores conectados à placa ESP8266, “D0”, “D1” e “A0” possuem o mesmo significado do que foi explicado em 4.1. 

[ VIDEO MONITORING ALL MQTTI ]

Os valores de “D0” e “D1” permanecem em nível lógico alto, reiterando o funcionamento do sistema, como discutido na seção anterior [4.1], enquanto “A0”: oscila o seu valor de inicialmente 354 para 0, após o potenciômetro ser ajustado para o valor mínimo que pode assumir, ratificando novamente a corretude do sistema.

### Terceiro Teste

Neste teste uma NodeMCU Id 2 é selecionada, e seguidamente o monitoramento do sensor analógico - A0 - é feito. Inicialmente o sensor possui o valor 706, variando de 1024 e em ato contínuo para 0.

[ MONITORING A0 MQTT ]


# <a id="conclusao"></a>
## 5. Conclusão

Diante do que foi exposto neste documento, é perceptível que o problema em questão caracteriza-se em um sistema digital cuja a função principal é estabelecer uma comunicação MQTT entre dispositivos MQTT, bem como dispositivos UART, capaz de exibir as informações de todas as unidades em forma de gráfico em uma Interface Homem 
Máquina ou em um display LCD.
Logo nota-se a partir dos testes na seção anterior a correta função do sistema. Visto que  a solução resolve de forma quase completa o problema abordado, podendo receber melhorias de performance entre o tráfego de informações e a melhoria da IHM em futuras versões deste sistema.

# <a id="referencias"></a>
## 6. Referências

EU PRECISO
das imagens sobre a interface
das imagens que mostram o menu identificando uma node MQTT
PRINT DO MENU LCD
