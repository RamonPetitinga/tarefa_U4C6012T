#include "ssd1306.h"
#include "font.h"

// Inicializa o display SSD1306
void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c)
{
    ssd->width = width;                                      // Define a largura do display
    ssd->height = height;                                    // Define a altura do display
    ssd->pages = height / 8U;                                // Calcula o número de páginas (cada página tem 8 linhas)
    ssd->address = address;                                  // Define o endereço I2C do display
    ssd->i2c_port = i2c;                                     // Define a instância do barramento I2C
    ssd->bufsize = ssd->pages * ssd->width + 1;              // Calcula o tamanho do buffer de memória
    ssd->ram_buffer = calloc(ssd->bufsize, sizeof(uint8_t)); // Aloca memória para o buffer
    ssd->ram_buffer[0] = 0x40;                               // Define o primeiro byte do buffer como 0x40 (comando de dados)
    ssd->port_buffer[0] = 0x80;                              // Define o primeiro byte do buffer de porta como 0x80 (comando)
}

// Configura o display SSD1306 com parâmetros padrão
void ssd1306_config(ssd1306_t *ssd)
{
    ssd1306_command(ssd, SET_DISP | 0x00);            // Desliga o display
    ssd1306_command(ssd, SET_MEM_ADDR);               // Define o modo de endereçamento de memória
    ssd1306_command(ssd, 0x01);                       // Modo de endereçamento de página
    ssd1306_command(ssd, SET_DISP_START_LINE | 0x00); // Define a linha inicial do display
    ssd1306_command(ssd, SET_SEG_REMAP | 0x01);       // Inverte o mapeamento de segmentos (horizontal flip)
    ssd1306_command(ssd, SET_MUX_RATIO);              // Define a proporção de multiplexação
    ssd1306_command(ssd, HEIGHT - 1);                 // Configura a altura do display
    ssd1306_command(ssd, SET_COM_OUT_DIR | 0x08);     // Inverte a direção dos pinos COM
    ssd1306_command(ssd, SET_DISP_OFFSET);            // Define o deslocamento vertical do display
    ssd1306_command(ssd, 0x00);                       // Sem deslocamento
    ssd1306_command(ssd, SET_COM_PIN_CFG);            // Configura os pinos COM
    ssd1306_command(ssd, 0x12);                       // Configuração específica para o display
    ssd1306_command(ssd, SET_DISP_CLK_DIV);           // Define o divisor de clock do display
    ssd1306_command(ssd, 0x80);                       // Frequência de clock padrão
    ssd1306_command(ssd, SET_PRECHARGE);              // Define o tempo de pré-carga
    ssd1306_command(ssd, 0xF1);                       // Configuração específica para o display
    ssd1306_command(ssd, SET_VCOM_DESEL);             // Define o nível de desseleção VCOM
    ssd1306_command(ssd, 0x30);                       // Configuração específica para o display
    ssd1306_command(ssd, SET_CONTRAST);               // Define o contraste do display
    ssd1306_command(ssd, 0xFF);                       // Contraste máximo
    ssd1306_command(ssd, SET_ENTIRE_ON);              // Liga a exibição de todos os pixels
    ssd1306_command(ssd, SET_NORM_INV);               // Define a exibição normal (não invertida)
    ssd1306_command(ssd, SET_CHARGE_PUMP);            // Configura a bomba de carga
    ssd1306_command(ssd, 0x14);                       // Habilita a bomba de carga para alimentação externa
    ssd1306_command(ssd, SET_DISP | 0x01);            // Liga o display
}

// Envia um comando para o display
void ssd1306_command(ssd1306_t *ssd, uint8_t command)
{
    ssd->port_buffer[1] = command; // Armazena o comando no buffer de porta
    i2c_write_blocking(
        ssd->i2c_port,
        ssd->address,
        ssd->port_buffer,
        2,
        false); // Envia o comando via I2C
}

// Envia o conteúdo do buffer de memória para o display
void ssd1306_send_data(ssd1306_t *ssd)
{
    ssd1306_command(ssd, SET_COL_ADDR);   // Define o intervalo de colunas
    ssd1306_command(ssd, 0);              // Coluna inicial
    ssd1306_command(ssd, ssd->width - 1); // Coluna final
    ssd1306_command(ssd, SET_PAGE_ADDR);  // Define o intervalo de páginas
    ssd1306_command(ssd, 0);              // Página inicial
    ssd1306_command(ssd, ssd->pages - 1); // Página final
    i2c_write_blocking(
        ssd->i2c_port,
        ssd->address,
        ssd->ram_buffer,
        ssd->bufsize,
        false); // Envia o buffer de memória via I2C
}

// Desenha um pixel na posição (x, y) com o valor especificado (ligado/desligado)
void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value)
{
    uint16_t index = (y >> 3) + (x << 3) + 1; // Calcula o índice no buffer de memória
    uint8_t pixel = (y & 0b111);              // Calcula o bit específico dentro do byte
    if (value)
        ssd->ram_buffer[index] |= (1 << pixel); // Liga o pixel
    else
        ssd->ram_buffer[index] &= ~(1 << pixel); // Desliga o pixel
}

// Preenche todo o display com o valor especificado (ligado/desligado)
void ssd1306_fill(ssd1306_t *ssd, bool value)
{
    // Itera por todas as posições do display
    for (uint8_t y = 0; y < ssd->height; ++y)
    {
        for (uint8_t x = 0; x < ssd->width; ++x)
        {
            ssd1306_pixel(ssd, x, y, value); // Define o valor do pixel
        }
    }
}

// Desenha um retângulo na posição (top, left) com as dimensões (width, height)
void ssd1306_rect(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill)
{
    // Desenha as bordas do retângulo
    for (uint8_t x = left; x < left + width; ++x)
    {
        ssd1306_pixel(ssd, x, top, value);              // Borda superior
        ssd1306_pixel(ssd, x, top + height - 1, value); // Borda inferior
    }
    for (uint8_t y = top; y < top + height; ++y)
    {
        ssd1306_pixel(ssd, left, y, value);             // Borda esquerda
        ssd1306_pixel(ssd, left + width - 1, y, value); // Borda direita
    }

    // Preenche o retângulo, se necessário
    if (fill)
    {
        for (uint8_t x = left + 1; x < left + width - 1; ++x)
        {
            for (uint8_t y = top + 1; y < top + height - 1; ++y)
            {
                ssd1306_pixel(ssd, x, y, value); // Preenche o interior do retângulo
            }
        }
    }
}

// Desenha uma linha entre os pontos (x0, y0) e (x1, y1)
void ssd1306_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool value)
{
    int dx = abs(x1 - x0); // Diferença em x
    int dy = abs(y1 - y0); // Diferença em y

    int sx = (x0 < x1) ? 1 : -1; // Direção em x
    int sy = (y0 < y1) ? 1 : -1; // Direção em y

    int err = dx - dy; // Erro inicial

    while (true)
    {
        ssd1306_pixel(ssd, x0, y0, value); // Desenha o pixel atual

        if (x0 == x1 && y0 == y1)
            break; // Termina quando alcança o ponto final

        int e2 = err * 2;

        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

// Desenha uma linha horizontal entre os pontos (x0, y) e (x1, y)
void ssd1306_hline(ssd1306_t *ssd, uint8_t x0, uint8_t x1, uint8_t y, bool value)
{
    for (uint8_t x = x0; x <= x1; ++x)
        ssd1306_pixel(ssd, x, y, value); // Desenha cada pixel da linha
}

// Desenha uma linha vertical entre os pontos (x, y0) e (x, y1)
void ssd1306_vline(ssd1306_t *ssd, uint8_t x, uint8_t y0, uint8_t y1, bool value)
{
    for (uint8_t y = y0; y <= y1; ++y)
        ssd1306_pixel(ssd, x, y, value); // Desenha cada pixel da linha
}

// Desenha um caractere na posição (x, y)
void ssd1306_draw_char(ssd1306_t *ssd, char c, uint8_t x, uint8_t y)
{
    uint16_t index = 0;
    char ver = c;
    if (c >= 'A' && c <= 'Z')
    {
        index = (c - 'A' + 11) * 8; // Para letras maiúsculas
    }
    else if (c >= '0' && c <= '9')
    {
        index = (c - '0' + 1) * 8; // Para números
    }
    else if (c >= 'a' && c <= 'z')
    {
        index = (c - 'a' + 37) * 8; // Para letras minúsculas
    }

    // Desenha cada linha do caractere
    for (uint8_t i = 0; i < 8; ++i)
    {
        uint8_t line = font[index + i];
        for (uint8_t j = 0; j < 8; ++j)
        {
            ssd1306_pixel(ssd, x + i, y + j, line & (1 << j)); // Desenha cada pixel do caractere
        }
    }
}

// Desenha um ícone na posição (x, y) com base no ID fornecido
void ssd1306_draw_icon(ssd1306_t *ssd, const int id, uint8_t x, uint8_t y)
{
    // Desenha cada linha do ícone
    for (uint8_t i = 0; i < 8; ++i)
    {
        uint8_t line = icon[id * 8 + i]; // Obtém a linha do ícone
        for (uint8_t j = 0; j < 8; ++j)
        {
            ssd1306_pixel(ssd, x + i, y + j, line & (1 << j)); // Desenha cada pixel do ícone
        }
    }
}

// Desenha uma string na posição (x, y)
void ssd1306_draw_string(ssd1306_t *ssd, const char *str, uint8_t x, uint8_t y)
{
    while (*str)
    {
        ssd1306_draw_char(ssd, *str++, x, y); // Desenha cada caractere da string
        x += 8;                               // Avança para a próxima posição
        if (x + 8 >= ssd->width)              // Verifica se ultrapassou a largura do display
        {
            x = 0;
            y += 8; // Move para a próxima linha
        }
        if (y + 8 >= ssd->height) // Verifica se ultrapassou a altura do display
        {
            break;
        }
    }
}