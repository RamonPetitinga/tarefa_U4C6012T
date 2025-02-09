### **Projeto: Controle de LEDs, Matriz 5x5 e Display SSD1306 com RP2040**

---

### **Demonstração do Projeto 🎥**

**Vídeo do Projeto:** [Link do Vídeo](https://youtube.com/shorts/5Nli-Nc5JzQ?si=yBXUn6NnR6jOYogj)

---

**Para executar clone o repositório, compile e grave para execultar os testes na placa BitDogLab**

---

### **Descrição do Projeto**

Este projeto implementa um sistema de controle de LEDs, uma matriz de LEDs WS2812 e um display SSD1306 utilizando a placa **BitDogLab** com o microcontrolador **RP2040**. O sistema emprega interrupções, debouncing via software, exibição de números de 0 a 9 na matriz de LEDs e interação com um display gráfico via I2C.

---

### **Funcionalidades Principais:**

1. **Exibição de números de 0 a 9 em uma matriz de LEDs WS2812 (5x5).**
2. **Alterar o número exibido ao pressionar os botões físicos (Botão A/B).**
3. **Aplicar debouncing via software** para evitar leituras incorretas dos botões.
4. **Exibição de caracteres e mensagens no display SSD1306** via comunicação I2C.
5. **Controle de LEDs RGB** (verde e azul) com interrupções.
6. **Piscar continuamente um LED RGB** para indicar que o código está em execução.

---

### **Tecnologias Utilizadas:**

- **C**
- **Pico SDK**
- **Interrupções (IRQ)**
- **GPIO (RP2040)**
- **WS2812 (LEDs)**
- **I2C (Comunicação com o display SSD1306)**

---

### **Objetivos:**

- ✅ Compreender e implementar interrupções no RP2040.
- ✅ Aplicar debouncing via software para evitar múltiplos acionamentos.
- ✅ Controlar LEDs individuais e matrizes WS2812.
- ✅ Utilizar comunicação I2C para controlar um display SSD1306.
- ✅ Criar um projeto funcional combinando hardware e software.

---

### **Hardware Utilizado:**

- **Placa:** BitDogLab com RP2040
- **Matriz de LEDs:** WS2812 (5x5)
- **LED RGB:** GPIOs 11 (verde) e 12 (azul)
- **Botão A:** GPIO 5
- **Botão B:** GPIO 6
- **Display SSD1306:** Conectado via I2C (GPIO 14 - SDA, GPIO 15 - SCL)

---

### **Funcionalidades Detalhadas:**

1. **Piscar o LED RGB:**

   - O LED RGB pisca continuamente para indicar que o código está em execução.

2. **Botão A:**

   - Incrementa o número exibido na matriz de LEDs (se o número for menor que 9).
   - Alterna o estado do LED RGB verde (ligado/desligado).
   - Atualiza o display SSD1306 com o estado do LED verde.

3. **Botão B:**

   - Decrementa o número exibido na matriz de LEDs (se o número for maior que 0).
   - Alterna o estado do LED RGB azul (ligado/desligado).
   - Atualiza o display SSD1306 com o estado do LED azul.

4. **Matriz WS2812:**

   - Exibe os números de 0 a 9, com padrões pré-definidos para cada número.
   - Os LEDs são controlados via PIO (Programmable I/O) para garantir precisão e eficiência.

5. **Display SSD1306:**

   - Exibe mensagens como "Digite o que deseja!" e o estado dos LEDs (G OFF, B OFF).
   - Atualiza dinamicamente o conteúdo com base nas interações do usuário (botões e entrada de caracteres).

6. **Entrada de Caracteres via Serial Monitor:**
   - Quando um caractere é digitado, ele é exibido no display SSD1306.
   - Se o caractere for um número entre 0 e 9, o padrão correspondente é exibido na matriz de LEDs WS2812.

---

### **Extras:**

- **Uso de interrupções:** Garante uma resposta rápida e eficiente aos botões.
- **Debouncing via software:** Evita leituras incorretas causadas por ruídos mecânicos dos botões.
- **Comunicação I2C:** Utilizada para controlar o display SSD1306, demonstrando o uso de protocolos de comunicação serial.
- **Controle de LEDs WS2812:** Demonstra o uso de PIO para controle preciso de LEDs endereçáveis.

---

### **Resumo Geral:**

O projeto integra hardware e software de forma eficiente, utilizando o RP2040 para controlar LEDs, uma matriz WS2812 e um display SSD1306. As interrupções e o debouncing garantem uma interação fluida com os botões, enquanto a comunicação I2C permite a exibição de informações no display. O código está bem organizado e comentado, facilitando a compreensão e manutenção.

### **Considerações Finais:**

Este projeto é uma excelente oportunidade para consolidar conhecimentos em programação de microcontroladores, manipulação de hardware e desenvolvimento de interfaces de comunicação. Certifique-se de seguir todos os requisitos e de entregar um código bem estruturado e comentado. Boa sorte! 🚀
