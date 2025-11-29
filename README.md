# Projeto de Reconhecimento de LIBRAS com IoT

Este projeto é um sistema completo que utiliza visão computacional para reconhecer gestos da Língua Brasileira de Sinais (LIBRAS) e exibe a letra correspondente em uma matriz de LED, criando uma solução de Internet das Coisas (IoT).

## Visão Geral e Arquitetura

O sistema é dividido em três módulos principais que se comunicam em sequência:

1.  **Frontend (Detector de Gestos):** Uma página da web que usa a câmera do seu computador para capturar vídeo e, com a ajuda da biblioteca MediaPipe, detecta gestos específicos da mão.
2.  **Backend (API de Comunicação):** Um servidor em Python (Flask) que recebe a letra detectada pelo frontend e atua como uma ponte, publicando essa informação em um tópico MQTT.
3.  **Hardware (Dispositivo IoT):** Um microcontrolador ESP32 conectado a uma matriz de LED. Ele se inscreve no tópico MQTT para receber a letra enviada pelo backend e exibi-la em tempo real.

### Fluxo de Dados

O fluxo de informação acontece da seguinte forma:

`Câmera (Frontend)` → `Detecção com MediaPipe (Frontend)` → `Requisição HTTP (Frontend → Backend)` → `API Flask (Backend)` → `Publicação MQTT (Backend → Broker)` → `Subscrição MQTT (Hardware)` → `Display na Matriz de LED (Hardware)`

---

## Módulos do Projeto

### 1. Frontend (`MediaPipe/libras.html`)

Este é o ponto de entrada do sistema. É uma página HTML que não requer instalação e pode ser aberta em qualquer navegador moderno.

-   **Funcionalidade:** Utiliza a biblioteca `Holistic` do Google MediaPipe para analisar o feed da câmera em tempo real. Ele detecta os pontos de referência (landmarks) da mão direita ou esquerda.
-   **Lógica de Detecção:** Uma função JavaScript (`detectLetter`) calcula a distância entre os pontos dos dedos para determinar se eles estão abertos ou fechados, identificando as letras **A, B, L, V, Y, W**.
-   **Comunicação:** A cada segundo, se uma letra for reconhecida de forma consistente, o frontend envia uma requisição `POST` para a API do backend com a letra detectada no formato JSON.

### 2. Backend (`api/app.py`)

O coração da comunicação do projeto. É uma API simples construída com Flask.

-   **Funcionalidade:** Expõe um único endpoint (`/api/enviar-letra`) que aguarda receber dados via `POST`.
-   **Ponte para IoT:** Ao receber uma letra válida do frontend, a API não a processa diretamente. Em vez disso, ela se conecta a um broker MQTT público (`broker.hivemq.com`) e publica a letra recebida no tópico `esp32/display/letra`. Isso desacopla o frontend do hardware, permitindo que qualquer dispositivo IoT que "entenda" MQTT possa se conectar e receber os dados.

### 3. Hardware (`matriz/matriz.ino`)

O componente físico do projeto, responsável por exibir a informação para o usuário.

-   **Hardware:** Um microcontrolador ESP32, que possui conectividade Wi-Fi nativa, conectado a uma matriz de LED controlada pelo driver MAX7219.
-   **Funcionalidade:** Ao ser ligado, o ESP32 se conecta à rede Wi-Fi especificada no código e se inscreve no tópico MQTT `esp32/display/letra`.
-   **Ação:** Ele fica constantemente "ouvindo" o tópico. Quando uma nova mensagem (uma letra) é publicada pelo backend, o ESP32 a recebe imediatamente e utiliza a biblioteca `MD_Parola` para rolar ou exibir a letra na matriz de LED.

---

## Como Executar o Projeto

Siga os passos abaixo para executar cada módulo na ordem correta.

### Pré-requisitos

-   **Python 3:** [Instale o Python](https://www.python.org/downloads/).
-   **Bibliotecas Python:** Flask, Flask-CORS e Paho-MQTT.
-   **Arduino IDE:** [Instale a IDE do Arduino](https://www.arduino.cc/en/software) e configure-a para a placa ESP32.
-   **Bibliotecas Arduino:** `PubSubClient`, `MD_Parola`, `MD_MAX72XX`.
-   **Hardware:** Um ESP32, uma matriz de LED 8x8 com driver MAX7219 e fios para conexão.
-   Um navegador web moderno (Chrome, Firefox, etc.).

### Passo 1: Configurar e Executar o Hardware (ESP32)

1.  Abra o arquivo `matriz/matriz.ino` na Arduino IDE.
2.  **Configure o Wi-Fi:** Altere as variáveis `ssid` e `password` com o nome e a senha da sua rede Wi-Fi.
    ```cpp
    const char* ssid = "NOME_DA_SUA_REDE";
    const char* password = "SENHA_DA_SUA_REDE";
    ```
3.  Conecte o ESP32 ao seu computador, selecione a porta e a placa correta na IDE e carregue o código.
4.  Abra o Monitor Serial (`Tools > Serial Monitor`) com a velocidade de `115200` para acompanhar o status da conexão Wi-Fi e MQTT.

### Passo 2: Executar o Backend (API)

1.  Abra um terminal ou prompt de comando.
2.  Navegue até a pasta raiz do projeto.
3.  Instale as dependências do Python:
    ```bash
    pip install Flask flask-cors paho-mqtt
    ```
4.  Execute o servidor da API:
    ```bash
    python api/app.py
    ```
5.  O terminal deverá exibir mensagens indicando que o servidor está rodando em `http://127.0.0.1:5000` e que a conexão com o broker MQTT foi bem-sucedida.

### Passo 3: Abrir o Frontend

1.  Encontre o arquivo `MediaPipe/libras.html` no seu computador.
2.  Abra-o com um navegador de sua preferência (clique com o botão direito e "Abrir com...").
3.  Permita o acesso à sua câmera quando o navegador solicitar.
4.  Posicione sua mão direita ou esquerda em frente à câmera e faça um dos gestos suportados (A, B, L, V, Y, W).
5.  A letra detectada será exibida na tela e, em instantes, aparecerá na sua matriz de LED.

---

## Tecnologias Utilizadas

-   **Visão Computacional:**
    -   [MediaPipe](https://mediapipe.dev/) (para detecção de mãos em tempo real)
-   **Backend:**
    -   [Python](https://www.python.org/)
    -   [Flask](https://flask.palletsprojects.com/) (para a API REST)
    -   [Paho-MQTT](https://www.eclipse.org/paho/index.php?page=clients/python/index.php) (para comunicação MQTT)
-   **Hardware:**
    -   [ESP32](https://www.espressif.com/en/products/socs/esp32)
    -   [Arduino Framework](https://www.arduino.cc/)
-   **Protocolo de Comunicação:**
    -   HTTP
    -   MQTT
