#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "mpu6050.h"
#include "rfm95_lora.h"

// Definições do display OLED
#define I2C_PORT_DISP i2c1      // Porta I2C para o display
#define I2C_SDA_DISP 14         // Pino SDA do display
#define I2C_SCL_DISP 15         // Pino SCL do display
#define ENDERECO_DISP 0x3C      // Endereço I2C do display
#define DISP_W 128              // Largura do display
#define DISP_H 64               // Altura do display

// Pinos do I²C para o sensor MPU6050
#define I2C_SENSOR_PORTA i2c0   // Porta I2C do sensor
#define I2C_SENSOR_SDA 0        // Pino SDA do sensor
#define I2C_SENSOR_SCL 1        // Pino SCL do sensor

// Variáveis globais
ssd1306_t ssd;                  // Estrutura de controle do display
static mpu6050_data_t dados_sensor_atuais; // Dados atuais do sensor

// Configura e inicializa o display OLED
void setup_display() {
    i2c_init(I2C_PORT_DISP, 400 * 1000);
    gpio_set_function(I2C_SDA_DISP, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DISP, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_DISP);
    gpio_pull_up(I2C_SCL_DISP);
    
    // Inicializa display com parâmetros corretos
    ssd1306_init(&ssd, DISP_W, DISP_H, false, ENDERECO_DISP, I2C_PORT_DISP);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "TX LoRa", 10, 10, false);
    ssd1306_send_data(&ssd);
}

// Exibe os valores dos sensores no display OLED
static void mostrar_tela_valores_sensores(void) {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "VALORES", 46, 1, false);
    
    char linha[30];
    int y = 10;
    
    // Exibe dados do acelerômetro
    snprintf(linha, sizeof(linha), "ax: %.2f", dados_sensor_atuais.accel_x);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    y += 9;
    snprintf(linha, sizeof(linha), "ay: %.2f", dados_sensor_atuais.accel_y);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    y += 9;
    snprintf(linha, sizeof(linha), "az: %.2f", dados_sensor_atuais.accel_z);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    
    // Exibe dados do giroscópio
    y += 9;
    snprintf(linha, sizeof(linha), "gx: %.2f", dados_sensor_atuais.gyro_x);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    y += 9;
    snprintf(linha, sizeof(linha), "gy: %.2f", dados_sensor_atuais.gyro_y);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    y += 9;
    snprintf(linha, sizeof(linha), "gz: %.2f", dados_sensor_atuais.gyro_z);
    ssd1306_draw_string(&ssd, linha, 0, y, false);
    
    ssd1306_send_data(&ssd);
}

int main() {
    stdio_init_all();
    sleep_ms(2000); // Aguarda inicialização do sistema
    printf("Iniciando Transmissor LoRa (TX)...\n");
    
    // Configura comunicação I²C para o sensor MPU6050
    i2c_init(I2C_SENSOR_PORTA, 400 * 1000);
    gpio_set_function(I2C_SENSOR_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SENSOR_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SENSOR_SDA);
    gpio_pull_up(I2C_SENSOR_SCL);
    
    // Inicializa periféricos
    mpu6050_init(I2C_SENSOR_PORTA);
    setup_display();
    
    // Verifica comunicação com o módulo LoRa
    if (!lora_init()) {
        printf("Falha na comunicacao com o RFM95. Travando. ❌\n");
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "RFM95 FALHOU!", 10, 20, false);
        ssd1306_send_data(&ssd);
        while(1); // Trava em caso de falha
    }
    
    printf("Comunicacao com RFM95 OK! ✅\n");
    lora_set_power(17); // Potência máxima de transmissão
    
    char message_buffer[256];
    while (1) {
        // Lê dados do sensor
        mpu6050_read_data(&dados_sensor_atuais);
        mostrar_tela_valores_sensores();
        
        // Formata mensagem com os dados do sensor
        snprintf(message_buffer, sizeof(message_buffer),
                "%.2f %.2f %.2f %.2f %.2f %.2f",
                dados_sensor_atuais.accel_x, 
                dados_sensor_atuais.accel_y,
                dados_sensor_atuais.accel_z,
                dados_sensor_atuais.gyro_x,
                dados_sensor_atuais.gyro_y,
                dados_sensor_atuais.gyro_z
        );
        
        // Envia dados via LoRa
        lora_send_packet((uint8_t*)message_buffer, strlen(message_buffer));
        printf("Pacote enviado: '%s'\n", message_buffer);
        
        sleep_ms(200); // Pequeno delay entre transmissões
    }
    return 0;
}