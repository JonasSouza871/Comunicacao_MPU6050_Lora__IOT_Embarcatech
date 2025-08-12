#ifndef RFM95_LORA_H
#define RFM95_LORA_H
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdbool.h>

//Definições de Pinos e SPI
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_RST  20

//Frequência de operação (915 MHz para o Brasil)
#define LORA_FREQUENCY_HZ 915E6

//Funções Públicas da Biblioteca
bool lora_init();                    //Inicializa hardware SPI e módulo RFM95
void lora_idle();                    //Modo standby (baixo consumo, pronto para TX/RX)
void lora_sleep();                   //Modo hibernação (menor consumo)
void lora_set_frequency(long frequency); //Configura frequência em Hz
void lora_set_power(uint8_t power);  //Configura potência em dBm (2-17)
void lora_send_packet(const uint8_t* buffer, uint8_t size); //Envia pacote
int lora_receive_packet(uint8_t* buffer, int max_size);     //Recebe pacote (não-bloqueante)
int lora_packet_rssi();              //RSSI do último pacote em dBm
float lora_packet_snr();              //SNR do último pacote em dB

#endif //RFM95_LORA_H