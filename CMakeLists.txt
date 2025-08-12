#Versão mínima do CMake necessária
cmake_minimum_required(VERSION 3.13)

#Importa o SDK do Pico
include(pico_sdk_import.cmake)

#Define o nome do projeto e linguagens utilizadas
project(lora_pico C CXX ASM)

#Padrões das linguagens C e C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#Inicializa o SDK do Pico
pico_sdk_init()

#Adiciona diretório de bibliotecas ao caminho de includes
include_directories(lib)

#Seleção do código-fonte principal (TX ou RX)
set(MAIN_SOURCE_FILE
    #Descomente para compilar como transmissor
    lora_tx.c
    #Descomente para compilar como receptor
    #lora_rx.c
)

#Lista de arquivos-fonte da biblioteca
set(LIB_SOURCES
    lib/rfm95_lora.c    #Driver LoRa RFM95
    lib/ssd1306.c       #Driver display OLED
    lib/mpu6050.c       #Driver acelerômetro
)

#Cria o executável do projeto
add_executable(${CMAKE_PROJECT_NAME}
    ${MAIN_SOURCE_FILE}
    ${LIB_SOURCES}
)

#Bibliotecas necessárias do Pico SDK
target_link_libraries(${CMAKE_PROJECT_NAME}
    pico_stdlib         #Biblioteca padrão do Pico
    hardware_spi        #Para comunicação LoRa
    hardware_i2c        #Para display e MPU6050
)

#Gera formatos de saída adicionais (.uf2, .hex)
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

#Configura saída de depuração (USB habilitado, UART desabilitado)
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)