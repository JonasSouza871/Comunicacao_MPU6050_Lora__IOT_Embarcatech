# 🚀 Pico LoRa Telemetry – MPU6050 & RFM95

<div align="center">

![Linguagem](https://img.shields.io/badge/Linguagem-C%2FC%2B%2B-blue?style=for-the-badge)
![Hardware](https://img.shields.io/badge/Hardware-Raspberry%20Pi%20Pico-E01244?style=for-the-badge)
![Tecnologia](https://img.shields.io/badge/Tecnologia-LoRa-E43A47?style=for-the-badge)
![Sensor](https://img.shields.io/badge/Sensor-MPU6050-555555?style=for-the-badge)

</div>

Um sistema de telemetria sem fio que transmite dados de **aceleração** e **giroscópio** de um sensor MPU6050 via LoRa entre dois Raspberry Pi Pico.

---

### 📝 Descrição Breve

Este projeto implementa um sistema de telemetria completo utilizando a tecnologia LoRa para comunicação de longo alcance. O sistema é composto por duas unidades:

1.  **Transmissor (TX):** Um Raspberry Pi Pico conectado a um sensor MPU6050 e um display OLED. Ele lê continuamente os **dados de aceleração (X, Y, Z) e giroscópio (X, Y, Z)** do sensor, exibe-os localmente e os transmite sem fio usando um módulo LoRa RFM95.
2.  **Receptor (RX):** Um segundo Raspberry Pi Pico com um módulo RFM95 e um display OLED. Ele recebe os pacotes de telemetria contendo os **seis valores dos sensores** (3 eixos de aceleração + 3 eixos de giroscópio), decodifica as informações e as exibe em seu próprio display, juntamente com a qualidade do sinal (RSSI e SNR).

O projeto inclui bibliotecas modulares para os periféricos (RFM95, MPU6050, SSD1306), facilitando a reutilização e expansão.

---

### ✨ Funcionalidades Principais

-   **✅ Sistema de Telemetria Completo:** Transmissão e recepção de dados de **6 eixos** (3 eixos do acelerômetro + 3 eixos do giroscópio) em tempo real.
-   **✅ Sensoriamento Duplo:** Coleta simultânea de dados de **aceleração** (movimento linear) e **giroscópio** (movimento angular) do MPU6050.
-   **✅ Bibliotecas Modulares:** Drivers em C/C++ para os módulos RFM95 (LoRa), MPU6050 (IMU) e SSD1306 (OLED).
-   **✅ Feedback Visual Duplo:** Cada unidade (TX e RX) possui seu próprio display OLED para monitoramento independente e em tempo real.
-   **✅ Monitoramento de Sinal:** O receptor exibe os indicadores RSSI e SNR, permitindo avaliar a qualidade e o alcance da comunicação LoRa.
-   **✅ Configuração Flexível:** O `CMakeLists.txt` permite ao usuário escolher facilmente qual firmware (Transmissor ou Receptor) compilar, editando apenas uma seção do arquivo.

---

### 🖼 Galeria do Projeto

<img src="https://github.com/user-attachments/assets/81e44948-9525-4127-8402-3b2e69e83951" width="600" />

> **📊 Dados Transmitidos:** O sistema envia continuamente 6 valores em cada pacote: **Aceleração X, Y, Z** (em g) e **Giroscópio X, Y, Z** (em °/s).

---

### ⚙ Hardware Necessário

**Unidade Transmissora (TX):**
| Componente | Quant. | Função |
| :--- | :---: | :--- |
| Raspberry Pi Pico | 1 | Microcontrolador principal |
| Módulo LoRa RFM95 | 1 | Transmissão de dados |
| Sensor MPU6050 | 1 | **Coleta de aceleração e giroscópio** |
| Display OLED 128x64 | 1 | Visualização dos dados coletados |
| Antena para 915 MHz | 1 | Comunicação LoRa |

**Unidade Receptora (RX):**
| Componente | Quant. | Função |
| :--- | :---: | :--- |
| Raspberry Pi Pico | 1 | Microcontrolador principal |
| Módulo LoRa RFM95 | 1 | Recepção de dados |
| Display OLED 128x64 | 1 | **Exibição dos 6 valores recebidos** |
| Antena para 915 MHz | 1 | Comunicação LoRa |

---

### 🔌 Conexões e Configuração

**Pinagem do Transmissor (TX):**
-   **MPU6050 (I2C0):** `SDA` -> `GPIO 0`, `SCL` -> `GPIO 1` *(Leitura de aceleração e giroscópio)*
-   **RFM95 (SPI0):** `MISO` -> `GPIO 16`, `CS` -> `GPIO 17`, `SCK` -> `GPIO 18`, `MOSI` -> `GPIO 19`, `RST` -> `GPIO 20`
-   **Display OLED (I2C1):** `SDA` -> `GPIO 14`, `SCL` -> `GPIO 15`

**Pinagem do Receptor (RX):**
-   **RFM95 (SPI0):** `MISO` -> `GPIO 16`, `CS` -> `GPIO 17`, `SCK` -> `GPIO 18`, `MOSI` -> `GPIO 19`, `RST` -> `GPIO 20`
-   **Display OLED (I2C1):** `SDA` -> `GPIO 14`, `SCL` -> `GPIO 15`

> **⚠ Importante:** Garanta um `GND` comum entre todos os componentes em cada unidade. Não esqueça de conectar as antenas.

---

### 📡 Protocolo de Dados

O sistema transmite os seguintes dados em cada pacote:

```
Formato: "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
Conteúdo: AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ

Exemplo: "-0.12,0.98,0.05,2.34,-1.67,0.89"
```

- **Aceleração (X,Y,Z):** Valores em unidades de gravidade (g)
- **Giroscópio (X,Y,Z):** Valores em graus por segundo (°/s)

---

### 🚀 Começando

#### Pré-requisitos de Software

-   **SDK:** Raspberry Pi Pico SDK
-   **Linguagem:** C/C++
-   **Build System:** CMake
-   **IDE Recomendada:** VS Code com a extensão "CMake Tools"

#### Configuração e Compilação

O `CMakeLists.txt` está configurado para compilar um alvo de cada vez (Transmissor ou Receptor). Você precisa **editar o arquivo para escolher qual firmware gerar** antes de compilar.

**Passo Chave: Escolhendo o Alvo (TX ou RX)**

1.  Abra o arquivo `CMakeLists.txt` no seu editor.
2.  Localize a seção que começa com `set(MAIN_SOURCE_FILE`.
3.  **Para compilar o Transmissor (TX):** Deixe a linha `lora_tx.c` descomentada e certifique-se de que a linha `lora_rx.c` está comentada com um `#`.

    ```cmake
    set(MAIN_SOURCE_FILE
        #Descomente para compilar como transmissor
        lora_tx.c
        #Descomente para compilar como receptor
        #lora_rx.c
    )
    ```

4.  **Para compilar o Receptor (RX):** Faça o inverso. Comente a linha `lora_tx.c` e descomente a linha `lora_rx.c`.

    ```cmake
    set(MAIN_SOURCE_FILE
        #Descomente para compilar como transmissor
        #lora_tx.c
        #Descomente para compilar como receptor
        lora_rx.c
    )
    ```

**Compilando e Carregando o Firmware**

Após salvar sua escolha no `CMakeLists.txt`, siga os passos abaixo no terminal. Você precisará repetir o processo para cada firmware.

```bash
# 1. (Se for a primeira vez) Crie e acesse a pasta de build
mkdir build
cd build

# 2. Gere os arquivos de compilação com o CMake
# Execute este comando sempre que alterar o CMakeLists.txt
cmake ..

# 3. Compile o projeto
make -j$(nproc)

# 4. Carregue o firmware gerado (lora_pico.uf2) no Pico correspondente
# Conecte o Pico segurando BOOTSEL e copie o arquivo .uf2 para ele.
# Lembre-se de compilar e carregar o TX em um Pico e o RX no outro.
cp lora_pico.uf2 /media/user/RPI-RP2
```

---

### 🐛 Solução de Problemas

-   **"RFM95 FALHOU!" no display:**
    -   Erro na comunicação SPI. Verifique todas as conexões do RFM95 (MISO, MOSI, SCK, CS, RST).
-   **Receptor não recebe pacotes:**
    -   Confirme que as antenas estão conectadas.
    -   Verifique se a frequência (915 MHz) e as configurações do modem são idênticas em ambos os códigos.
    -   Aproxime os módulos para um teste inicial.
-   **Dados recebidos como "formato inválido":**
    -   Ocorreu um erro na transmissão ou o formato da mensagem `snprintf` no TX não corresponde ao formato `sscanf` no RX. **Verifique se ambos usam exatamente 6 valores float** (3 de aceleração + 3 de giroscópio).
-   **Display não liga:**
    -   Verifique as conexões I2C (SDA, SCL) e a alimentação do display.
-   **Valores dos sensores muito instáveis:**
    -   Considere implementar filtros de média móvel ou calibração do MPU6050.

---


