#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "rfm95_lora.h"

// --- Definições do Display OLED ---
#define I2C_PORT_DISP   i2c1
#define I2C_SDA_DISP    14
#define I2C_SCL_DISP    15
#define ADDR_DISP       0x3C

// --- Estrutura para dados do sensor ---
typedef struct {
    float accel_x, accel_y, accel_z; // Dados do acelerômetro
    float gyro_x, gyro_y, gyro_z;   // Dados do giroscópio
} SensorData;

// --- Variável global do display ---
ssd1306_t ssd;

// --- Funções auxiliares ---
// Converte a mensagem recebida em dados do sensor
int parse_sensor_message(const char *message, SensorData *dados) {
    if (!message || !dados) return -1; // Verifica se os ponteiros são válidos
    int qtd = sscanf(message, "%f %f %f %f %f %f",
                     &dados->accel_x, &dados->accel_y, &dados->accel_z,
                     &dados->gyro_x, &dados->gyro_y, &dados->gyro_z);
    return (qtd == 6) ? 0 : -2; // Retorna 0 se conseguiu ler todos os dados
}

// Inicializa e configura o display OLED
void setup_display() {
    i2c_init(I2C_PORT_DISP, 400 * 1000);
    gpio_set_function(I2C_SDA_DISP, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DISP, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_DISP);
    gpio_pull_up(I2C_SCL_DISP);
    ssd1306_init(&ssd, 128, 64, false, ADDR_DISP, I2C_PORT_DISP);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "RX LoRa", 10, 10, false);
    ssd1306_draw_string(&ssd, "Aguardando...", 10, 30, false);
    ssd1306_send_data(&ssd);
}

// Mostra dados do sensor no display
static void mostrar_tela_valores_recebidos(const SensorData *dados) {
    char linha[30];
    int y = 0;
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "RX LoRa", 40, y, false);
    y += 10;
    
    // Exibe dados do acelerômetro
    snprintf(linha, sizeof(linha), "ax: %.2f", dados->accel_x);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    y += 9;
    snprintf(linha, sizeof(linha), "ay: %.2f", dados->accel_y);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    y += 9;
    snprintf(linha, sizeof(linha), "az: %.2f", dados->accel_z);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    
    // Exibe dados do giroscópio
    y += 9;
    snprintf(linha, sizeof(linha), "gx: %.2f", dados->gyro_x);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    y += 9;
    snprintf(linha, sizeof(linha), "gy: %.2f", dados->gyro_y);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    y += 9;
    snprintf(linha, sizeof(linha), "gz: %.2f", dados->gyro_z);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    
    ssd1306_send_data(&ssd);
}

// Mostra qualidade do sinal no display
static void mostrar_tela_rssi_snr(const int rssi, const float snr) {
    char linha[30];
    int y = 0;
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "RX LoRa", 40, y, false);
    y += 10;
    
    snprintf(linha, sizeof(linha), "RSSI: %d", rssi);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    y += 9;
    snprintf(linha, sizeof(linha), "SNR: %.2f", snr);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    
    ssd1306_send_data(&ssd);
}

// --- Função principal ---
int main() {
    SensorData dados_recebidos;
    stdio_init_all();
    sleep_ms(2000); // Aguarda inicialização do sistema
    printf("Iniciando Receptor LoRa (RX)...\n");
    
    setup_display();
    
    // Inicializa módulo LoRa
    if (!lora_init()) {
        printf("Falha na comunicacao com o RFM95. Travando. ❌\n");
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "RFM95 FALHOU!", 10, 20, false);
        ssd1306_send_data(&ssd);
        while(1); // Trava em caso de falha
    }
    
    printf("Comunicacao com RFM95 OK! ✅\n");
    printf("Aguardando pacotes...\n");
    
    uint8_t buffer[256];
    while (1) {
        // Tenta receber pacote LoRa
        int packet_size = lora_receive_packet(buffer, sizeof(buffer));
        
        if (packet_size > 0) {
            buffer[packet_size] = '\0'; // Garante string terminada
            int rssi = lora_packet_rssi();
            float snr = lora_packet_snr();
            
            // Processa mensagem se válida
            if (parse_sensor_message((char*)buffer, &dados_recebidos) == 0) {
                printf("Dados do sensor recebidos:\n");
                printf("Acelerometro: X=%.2f, Y=%.2f, Z=%.2f\n", 
                       dados_recebidos.accel_x, dados_recebidos.accel_y, dados_recebidos.accel_z);
                printf("Giroscopio: X=%.2f, Y=%.2f, Z=%.2f\n", 
                       dados_recebidos.gyro_x, dados_recebidos.gyro_y, dados_recebidos.gyro_z);
                mostrar_tela_valores_recebidos(&dados_recebidos);
            } else {
                printf("Mensagem com formato invalido: '%s'\n", buffer);
            }
            
            // Exibe informações do pacote
            printf("--------------------------------\n");
            printf("Pacote Recebido!\n");
            printf("  Mensagem: '%s'\n", buffer);
            printf("  Tamanho: %d bytes\n", packet_size);
            printf("  Qualidade do sinal: RSSI=%d dBm, SNR=%.2f dB\n", rssi, snr);
            printf("--------------------------------\n");
            
            sleep_ms(2000);
            mostrar_tela_rssi_snr(rssi, snr);
            sleep_ms(2000);
        }
    }
    return 0; 
}