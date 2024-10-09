# Simulador de Tráfego Urbano com Controle de Semáforos (FreeRTOS)

## Descrição
Este projeto tem como objetivo simular o comportamento de veículos em um sistema de cruzamentos controlados por semáforos. A lógica de controle é implementada utilizando tarefas concorrentes, que definem o movimento dos veículos de acordo com as regras de trânsito estabelecidas e a sincronização dos semáforos. A simulação utiliza a biblioteca FreeRTOS para o gerenciamento das tarefas e semáforos para a sincronização entre as diferentes direções nos cruzamentos.

## Funcionalidades
- **Movimento de Veículos**: Cada veículo segue uma direção aleatória em um dos cruzamentos (A, B, C, D) e decide se segue em frente, vira à direita ou à esquerda, dependendo das condições de tráfego e do estado dos semáforos.
- **Controle de Semáforos**: O sistema de semáforos utiliza semáforos binários (`xSemaphoreTake`) para controlar o acesso dos veículos aos cruzamentos, garantindo que apenas um veículo passe por vez em uma determinada direção.
- **Animação de Tráfego**: Funções de animação como `animacaoViaNS`, `animacaoSairW`, e `animacaoViaEW` e outras, são usadas para representar visualmente o movimento dos veículos pelas interseções.
- **Aleatoriedade nas Direções**: As direções dos veículos são aleatórias após cada passagem por um cruzamento, criando uma dinâmica mais realista de tráfego.

## Estrutura do Código
O código é dividido em diferentes funções e tarefas que controlam o tráfego veicular:

- **TaskVeiculo**: Esta é a task principal de cada veículo, responsável por controlar o seu movimento de acordo com o cruzamento em que se encontra e o estado dos semáforos. Dependendo da direção que o veículo deve seguir (frente, direita, esquerda), ele tenta obter o semáforo correspondente para prosseguir. Nessa task, é implementado a lógica de direcionamento dos veiculos nos 4 cruzamentos definidos bem como a aleatoriedade de seus movimentos.
- **Funções de Animação**: Funções como `animacaoViaNS`, `animacaoSairW`, e outras semelhantes, controlam a simulação visual do tráfego em diferentes direções.
- **Funções de Controle de Tráfego**: Funções como `modificaTrafego` são usadas para alterar o estado do tráfego conforme os veículos se movem pelos cruzamentos.
- **Função de Inicializar o Tráfego**: Função`inicializaTrafego` inicializa o veículo em uma determinada posição na matriz que representa as vias dos veículos, ao todos temos 4 vias por cruzamento.

## Recursos utilizados
- **FreeRTOS**: Usado para o gerenciamento de tarefas concorrentes (veículos) e semáforos (sinalização de trânsito).
- **Semáforos Binários**: Controlam o acesso aos cruzamentos.
- **Funções Aleatórias**: `rand()` é utilizado para determinar a próxima direção do veículo (aleatória).

## Explicando o Projeto:
URL VIDEO AQUI


