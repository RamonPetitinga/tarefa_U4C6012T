#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "inc/ws2812.pio.h"

// Definição dos pinos para conexão com os LEDs RGB
#define LED_G_PIN 11
#define LED_B_PIN 12

// Definição dos pinos para conexão com os botões
#define BTN_A_PIN 5
#define BTN_B_PIN 6

// Definições para uso dos LEDs na Matriz 5x5
#define LED_MTX_COUNT 25
#define LED_MTX_LEVEL 20 // A intensidade está baixa para não causar incômodo (0-255, caso deseje alterar)
#define LED_MTX_PIN 7

// Definições para o uso da comunicação serial I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADDRESS 0x3C

#define DEBOUNCE_DELAY_MS 200
volatile uint32_t last_interrupt_time = 0;

// Pix GRB
typedef struct pixel_t
{
    uint8_t G, R, B;
} led_t;

led_t led_matrix[LED_MTX_COUNT]; // Buffer de pixels que compõem a matriz

uint32_t led_number_pattern[10] = {
    0xe5294e, // Número 0
    0x435084, // Número 1
    0xe4384e, // Número 2
    0xe4390e, // Número 3
    0xa53902, // Número 4
    0xe1390e, // Número 5
    0xe4394e, // Número 6
    0xe40902, // Número 7
    0xe5394e, // Número 8
    0xe5390e  // Número 9
};

/*
 * Inicialização das GPIOs
 */
void init_gpio()
{
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_init(BTN_A_PIN);
    gpio_init(BTN_B_PIN);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);

    gpio_pull_up(BTN_A_PIN);
    gpio_pull_up(BTN_B_PIN);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// Variáveis para uso da PIO
PIO pio;
uint sm;

/*
 * Inicialização da PIO
 */
void init_pio(uint pin)
{
    uint offset = pio_add_program(pio0, &ws2812_program);
    pio = pio0;

    sm = pio_claim_unused_sm(pio, false);

    ws2812_program_init(pio, sm, offset, pin);
}

ssd1306_t ssd; // Inicializa a estrutura do display
void init_display()
{
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                        // Configura o display
    ssd1306_send_data(&ssd);                                     // Envia os dados para o display

    // Limpa o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Valores iniciais
    ssd1306_draw_string(&ssd, "Digite o que deseja!", 8, 10);
    ssd1306_draw_string(&ssd, "G OFF", 8, 48);
    ssd1306_draw_icon(&ssd, 2, 58, 48);
    ssd1306_draw_string(&ssd, "B OFF", 80, 48);
    ssd1306_send_data(&ssd);
}

/*
 * Atribuição de cor a um LED
 */
void set_led(const uint id, const uint8_t R, const uint8_t G, const uint8_t B)
{
    led_matrix[id].R = R;
    led_matrix[id].G = G;
    led_matrix[id].B = B;
}

/*
 * Limpeza do buffer de LEDs
 */
void clear_leds()
{
    for (uint i = 0; i < LED_MTX_COUNT; i++)
    {
        led_matrix[i].R = 0;
        led_matrix[i].G = 0;
        led_matrix[i].B = 0;
    }
}

// Formatação do valor GRB
uint32_t rgb_value(uint8_t B, uint8_t R, uint8_t G)
{
    return (G << 24) | (R << 16) | (B << 8);
}

/*
 * Transferência dos valores do buffer para a matriz de LEDs
 */
void write_leds()
{
    uint32_t value;
    for (uint i = 0; i < LED_MTX_COUNT; ++i)
    {
        value = rgb_value(led_matrix[i].B, led_matrix[i].R, led_matrix[i].G);
        pio_sm_put_blocking(pio, sm, value);
    }
}

/*
 * Decodificação do padrão binário para LEDs da matriz
 */

volatile bool green_led_on = false;
volatile bool blue_led_on = false;
volatile int number_id = -1;

void set_led_by_pattern(uint32_t pattern)
{
    uint lvl = LED_MTX_LEVEL;
    for (uint i = 0; i < LED_MTX_COUNT; i++)
    {
        // Verifica se o bit é 1. Em casos positivos, acende o LED na cor branca com a intensidade setada
        if ((pattern >> i) & 1)
            set_led(i, lvl * (!blue_led_on && !green_led_on), lvl * green_led_on, lvl * blue_led_on);
        else
            set_led(i, 0, 0, 0); // Caso contrário, deixa o LED apagado
    }
}

/*
 * Limpa uma linha
 */
void clear_line(uint y)
{
    for (int x = 8; x <= 112; x += 8)
    {
        ssd1306_draw_char(&ssd, ' ', x, y);
    }
}

void button_callback(uint gpio, uint32_t events)
{
    uint32_t now = to_ms_since_boot(get_absolute_time()); // Obtém o tempo atual em ms

    if (now - last_interrupt_time > DEBOUNCE_DELAY_MS)
    {                              // Verifica se o botão foi pressionado após o tempo de debounce
        last_interrupt_time = now; // Atualiza o tempo da última interrupção

        // Alterna o estado do LED verde se o botão A for pressionado
        if (gpio == BTN_A_PIN)
        {
            green_led_on = !green_led_on;
            gpio_put(LED_G_PIN, green_led_on);
            ssd1306_draw_string(&ssd, green_led_on ? "G ON " : "G OFF", 8, 48); // Atualiza o display
            printf(green_led_on ? "LED Verde ON\n" : "LED Verde OFF\n");
        }
        // Alterna o estado do LED azul se o botão B for pressionado
        else if (gpio == BTN_B_PIN)
        {
            blue_led_on = !blue_led_on;
            gpio_put(LED_B_PIN, blue_led_on);
            ssd1306_draw_string(&ssd, blue_led_on ? "B ON " : "B OFF", 80, 48); // Atualiza o display
            printf(blue_led_on ? "LED Azul ON\n" : "LED Azul OFF\n");
        }

        // Atualiza o ícone no display de acordo com os LEDs ligados/desligados
        if (green_led_on && blue_led_on)
            ssd1306_draw_icon(&ssd, 0, 58, 48); // Ambos ligados
        else if (green_led_on || blue_led_on)
            ssd1306_draw_icon(&ssd, 1, 58, 48); // Apenas um ligado
        else
            ssd1306_draw_icon(&ssd, 2, 58, 48); // Ambos desligados

        // Atualiza os LEDs da matriz se um número válido estiver selecionado
        if (number_id >= 0 && number_id <= 9)
        {
            set_led_by_pattern(led_number_pattern[number_id]); // Define o padrão dos LEDs
            write_leds();                                      // Envia os dados para a matriz de LEDs
        }

        ssd1306_send_data(&ssd); // Envia os dados atualizados para o display
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(500);

    init_pio(LED_MTX_PIN); // Configura o PIO para controlar a matriz de LEDs
    clear_leds();
    write_leds();
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa o barramento I2C com frequência de 400 kHz
    init_gpio();
    init_display();

    // Configurando as interrupções para o pressionamento dos botões
    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BTN_B_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    char c;
    int number_x = 64, number_y = 23; // Posição fixa para exibir números

    while (true)
    {
        sleep_ms(100);
        scanf("%c", &c);

        if (c >= '0' && c <= '9')
        {
            // Limpa a área onde o número será exibido (desenha um espaço)
            ssd1306_draw_char(&ssd, ' ', number_x, number_y); // Apaga o número anterior
            ssd1306_send_data(&ssd);                          // Atualiza o display para refletir a limpeza

            // Exibe o novo número na posição fixa
            ssd1306_draw_char(&ssd, c, number_x, number_y);

            // Atualiza o LED
            number_id = c - '0';
            set_led_by_pattern(led_number_pattern[number_id]);
            write_leds();
        }

        ssd1306_send_data(&ssd); // Atualiza o display
    }
}