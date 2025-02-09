#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define WIDTH 128 // Largura do display OLED
#define HEIGHT 64 // Altura do display OLED

// Enumeração dos comandos suportados pelo display SSD1306
typedef enum
{
    SET_CONTRAST = 0x81,        // Define o contraste do display
    SET_ENTIRE_ON = 0xA4,       // Liga/desliga a exibição de todos os pixels
    SET_NORM_INV = 0xA6,        // Define a exibição normal ou invertida
    SET_DISP = 0xAE,            // Liga/desliga o display
    SET_MEM_ADDR = 0x20,        // Define o modo de endereçamento de memória
    SET_COL_ADDR = 0x21,        // Define o intervalo de colunas para escrita
    SET_PAGE_ADDR = 0x22,       // Define o intervalo de páginas para escrita
    SET_DISP_START_LINE = 0x40, // Define a linha inicial do display
    SET_SEG_REMAP = 0xA0,       // Define o mapeamento de segmentos (horizontal flip)
    SET_MUX_RATIO = 0xA8,       // Define a proporção de multiplexação
    SET_COM_OUT_DIR = 0xC0,     // Define a direção de saída dos pinos COM
    SET_DISP_OFFSET = 0xD3,     // Define o deslocamento vertical do display
    SET_COM_PIN_CFG = 0xDA,     // Configura os pinos COM
    SET_DISP_CLK_DIV = 0xD5,    // Define o divisor de clock do display
    SET_PRECHARGE = 0xD9,       // Define o tempo de pré-carga
    SET_VCOM_DESEL = 0xDB,      // Define o nível de desseleção VCOM
    SET_CHARGE_PUMP = 0x8D      // Configura a bomba de carga (necessário para alimentação externa)
} ssd1306_command_t;

// Estrutura para armazenar o estado e configurações do display SSD1306
typedef struct
{
    uint8_t width, height, pages, address; // Dimensões e endereço I2C do display
    i2c_inst_t *i2c_port;                  // Instância do barramento I2C
    bool external_vcc;                     // Indica se a alimentação é externa
    uint8_t *ram_buffer;                   // Buffer de memória para o conteúdo do display
    size_t bufsize;                        // Tamanho do buffer de memória
    uint8_t port_buffer[2];                // Buffer temporário para envio de comandos/dados
} ssd1306_t;

// Protótipos das funções para controle do display SSD1306

// Inicializa o display SSD1306
void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c);

// Configura o display com parâmetros padrão
void ssd1306_config(ssd1306_t *ssd);

// Envia um comando para o display
void ssd1306_command(ssd1306_t *ssd, uint8_t command);

// Envia o conteúdo do buffer de memória para o display
void ssd1306_send_data(ssd1306_t *ssd);

// Desenha um pixel na posição (x, y) com o valor especificado (ligado/desligado)
void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value);

// Preenche todo o display com o valor especificado (ligado/desligado)
void ssd1306_fill(ssd1306_t *ssd, bool value);

// Desenha um retângulo na posição (top, left) com as dimensões (width, height)
void ssd1306_rect(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill);

// Desenha uma linha entre os pontos (x0, y0) e (x1, y1)
void ssd1306_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool value);

// Desenha uma linha horizontal entre os pontos (x0, y) e (x1, y)
void ssd1306_hline(ssd1306_t *ssd, uint8_t x0, uint8_t x1, uint8_t y, bool value);

// Desenha uma linha vertical entre os pontos (x, y0) e (x, y1)
void ssd1306_vline(ssd1306_t *ssd, uint8_t x, uint8_t y0, uint8_t y1, bool value);

// Desenha um caractere na posição (x, y)
void ssd1306_draw_char(ssd1306_t *ssd, char c, uint8_t x, uint8_t y);

// Desenha uma string na posição (x, y)
void ssd1306_draw_string(ssd1306_t *ssd, const char *str, uint8_t x, uint8_t y);

// Desenha um ícone na posição (x, y) com base no ID fornecido
void ssd1306_draw_icon(ssd1306_t *ssd, const int id, uint8_t x, uint8_t y);