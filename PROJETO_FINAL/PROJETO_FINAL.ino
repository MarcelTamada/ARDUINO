//{Projeto_Final.INO} Arduino MEGA2560 X 3.5" TFT DISPLAY ILI9486
//Interface gráfica desenvolvida para um Display TFT touchscreen - Driver: ILI9486
//Botões para acionar saídas do mega2560, definidas abaixo.
//@author Marcel Tamada
//@since 27/04/2025

// Se a mensagem do Serial Monitor estiver saindo com caracteres estranhos, no canto superior direito do terminal do serial monitor, tem a opção para escolher o baud e nesse projeto estou utilizando 115200.
// Conferir todos os #DEFINE se estão com a numeração de pinos corretas antes de compilar e testar o código
// Se a tela ficar de cabeça para baixo ou em uma orientação diferente da desejada, alterar os parâmetros das linhas 132 e 167
// O código está documentado com comentários, qualquer dúvida entre em contato comigo.

// ***** BAIXAR AS BIBLIOTECAS NO "Tools->Manage Libraries" ANTES DE COMPILAR E FAZER UPLOAD *****
#include <Arduino_GFX_Library.h>
#include <XPT2046_Touchscreen.h>

// SPI Padrão do Arduino Mega:
// SCK  = 52 (automático)
// MISO = 50 (automático)
// MOSI = 51 (automático)
// SS   = 53 (não usado diretamente)

// ******* CONFERIR OS PINOS QUE FAZEM CONEXÃO ENTRE O SHIELD E O MEGA2560 *******
//====  Pinos de conexão utilizados ====
#define TFT_CS    30  //CS ou SS
#define TFT_DC    28  //DC
#define TFT_RST   32  //RESET
#define TOUCH_CS  31  //T_CS ou T_SS

// ******* CONFERIR OS PINOS DISPONÍVEIS NO ARDUÍNO PORQUE DEPENDENDO DO SHIELD OS PINOS LIVRES PODEM MUDAR *******
//==== Pinos de saída para LED ====
#define LED1  33
#define LED2  34
#define LED3  35
#define LED4  36
#define LED5  37
#define LED6  38
#define LAZER  39
#define EMBASAMENTO  40

//==== Pinos de saída para pistão ====
//***** AVANÇO *****
#define P1_A  41
#define P2_A  42
#define P3_A  43
#define P4_A  44
#define P5_A  45
#define P6_A  46

//***** RECUO *****
#define P1_R  47
#define P2_R  48
#define P3_R  49
#define P4_R  50
#define P5_R  51
#define P6_R  52

// Inicializa e instanciao display passando os pinos como parametros
Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS);
Arduino_GFX *tft = new Arduino_ILI9486_18bit(bus, TFT_RST);

// Inicializa instancia o touchscreen
XPT2046_Touchscreen ts(TOUCH_CS);

// ******************* Configuração dos Botões *******************
#define NUM_BUTTONS 16
//                       [1] [2] [3]  [4]  [5]  [6] [LAZER] [EMBASAMENTO]    [1] [2]  [3] [4]  [5]   [6]    [SOBE][DESCE]
int btn_x[NUM_BUTTONS] = {5, 85, 165, 245, 325, 405,  83,     245,            5, 85, 165, 245, 325, 405,      83, 245};  // ===================== posições X =====================

int btn_y[NUM_BUTTONS] = {35, 35, 35, 35, 35, 35,     105,    105,           180, 180, 180, 180, 180, 180,   250, 250};  // ===================== posições Y =====================

int btn_w[NUM_BUTTONS] = {70, 70, 70, 70, 70, 70,     150,    150,           70, 70, 70, 70, 70, 70,         150, 150};  // ===================== largura =====================

int btn_h[NUM_BUTTONS] = {50, 50, 50, 50, 50, 50,     50,     50,             50, 50, 50, 50, 50, 50,         50, 50};   // ===================== altura =====================

// Cor normal (botões inativos) Hexadecimal: 0xFFE0 = Amarelo / 0x001F = Azul
uint16_t cor_n[NUM_BUTTONS] = {0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F, 0x001F}; 

// Cor ativa (botões pressionados) Hexadecimal: 0xF800 = Vermelho
uint16_t cor_a[NUM_BUTTONS] = {0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800}; 

// Escrita dos botões em ordem
const char* nome[NUM_BUTTONS] = {"1", "2", "3", "4", "5", "6", "LAZER", "EMBASAMENTO", "1", "2", "3", "4", "5", "6", "SOBE", "DESCE"};

// Ações dos pistões
const int pinosAvanco[] = {P1_A, P2_A, P3_A, P4_A, P5_A, P6_A};
// Ações dos pistões
const int pinosRecuo[]  = {P1_R, P2_R, P3_R, P4_R, P5_R, P6_R};

// Seta todos os botões desligados quando liga o display
bool btn_state[NUM_BUTTONS] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}; // estado dos botões

// Desenha todos os botões
void drawButtons() {
  tft->setTextSize(2);        // Ajusta o tamanho do texto
  tft->setTextColor(BLACK);   //Cor do texto

  for (int i = 0; i < NUM_BUTTONS; i++) {
    // Desenha o botão (preenchido)
    if (btn_state[i]) {                                                 //Se estiver ativado usa a cor_a
      tft->fillRect(btn_x[i], btn_y[i], btn_w[i], btn_h[i], cor_a[i]);
    } else {                                                            //Se não estiver ativado usa a cor_n
      tft->fillRect(btn_x[i], btn_y[i], btn_w[i], btn_h[i], cor_n[i]);
    }

    // Borda branca
    tft->drawRect(btn_x[i], btn_y[i], btn_w[i], btn_h[i], WHITE);

    // Variáveis para escerver o texto centralizado
    int16_t x1, y1;
    uint16_t w, h;

    // Calcula tamanho d o texto
    tft->getTextBounds(nome[i], btn_x[i], btn_y[i], &x1, &y1, &w, &h);

    // Centraliza dentro do botão
    int text_x = btn_x[i] + (btn_w[i] - w) / 2;
    int text_y = btn_y[i] + (btn_h[i] - h) / 2;

    //Define a posição e imprime o texto
    tft->setCursor(text_x, text_y);
    tft->print(nome[i]);
  }
}

void setup() {
  Serial.begin(115200);

  // Inicializa display
  tft->begin();
  tft->fillScreen(BLACK);
  // ORIENTAÇÃO DA TELA, SE FICAR DE CABEÇA PARA BAIXO PODE MUDAR ESSE VALOR setRotation PARA 1
  tft->setRotation(3); // RETRATO

  // Inicializa touchscreen
  ts.begin();

  //Declara a saída do arduíno, especificada nos defines, como output
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LAZER, OUTPUT);
  pinMode(EMBASAMENTO, OUTPUT);
  pinMode(P1_A, OUTPUT);
  pinMode(P2_A, OUTPUT);
  pinMode(P3_A, OUTPUT);
  pinMode(P4_A, OUTPUT);
  pinMode(P5_A, OUTPUT);
  pinMode(P6_A, OUTPUT);
  pinMode(P1_R, OUTPUT);
  pinMode(P2_R, OUTPUT);
  pinMode(P3_R, OUTPUT);
  pinMode(P4_R, OUTPUT);
  pinMode(P5_R, OUTPUT);
  pinMode(P6_R, OUTPUT);
  
  // Desenha os botões
  drawButtons();
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();

    int touch_x = p.x;
    int touch_y = p.y;

    // Ajusta p o seu display (pode mudar dependendo da tela)
    touch_x = map(touch_x, 0, 4095, 0, 480);
    touch_y = map(touch_y, 0, 4095, 0, 320);

    // Essas 4 linhas são apenas para visualizar no Serial Monitor a coordenada do touch
    Serial.print("Toque detectado em: ");
    Serial.print(touch_x);
    Serial.print(", ");
    Serial.println(touch_y);

    if (touch_x > 0 && touch_y > 0){
      // Verifica qual botão foi tocado com base nas coordenadas
      //============================================================= ILUMINAÇÃO ===================================================================================
      if (touch_x > 5 && touch_x < 70 && 
          touch_y > 35 && touch_y < 80) {
        btn_state[0] = !btn_state[0]; // Muda o estado do botão 1
        if (btn_state[0]){
          digitalWrite(LED1, HIGH);
        }
        else{
          digitalWrite(LED1, LOW);
        }
      }
      else if (touch_x > 85 && touch_x < 150 && 
              touch_y > 35 && touch_y < 80) {
        btn_state[1] = !btn_state[1]; // Muda o estado do botão 2
        if (btn_state[1]){
          digitalWrite(LED2, HIGH);
        }
        else{
          digitalWrite(LED2, LOW);
        }
      }
      else if (touch_x > 165 && touch_x < 235 && 
              touch_y > 35 && touch_y < 80) {
        btn_state[2] = !btn_state[2]; // Muda o estado do botão 3
        if (btn_state[2]){
          digitalWrite(LED3, HIGH);
        }
        else{
          digitalWrite(LED3, LOW);
        }
      }
      else if (touch_x > 245 && touch_x < 315 && 
              touch_y > 35 && touch_y < 80) {
        btn_state[3] = !btn_state[3]; // Muda o estado do botão 4
        if (btn_state[3]){
          digitalWrite(LED4, HIGH);
        }
        else{
          digitalWrite(LED4, LOW);
        }
      }
      else if (touch_x > 325 && touch_x < 395 && 
              touch_y > 35 && touch_y < 80) {
        btn_state[4] = !btn_state[4]; // Muda o estado do botão 5
        if (btn_state[4]){
          digitalWrite(LED5, HIGH);
        }
        else{
          digitalWrite(LED5, LOW);
        }
      }
      else if (touch_x > 405 && touch_x < 475 && 
              touch_y > 35 && touch_y < 80) {
        btn_state[5] = !btn_state[5]; // Muda o estado do botão 6
        if (btn_state[5]){
          digitalWrite(LED6, HIGH);
        }
        else{
          digitalWrite(LED6, LOW);
        }
      }
      else if (touch_x > 83 && touch_x < 233 && 
              touch_y > 105 && touch_y < 155) {
        btn_state[6] = !btn_state[6]; // Muda o estado do botão LAZER
        if (btn_state[6]){
          digitalWrite(LAZER, HIGH);
        }
        else{
          digitalWrite(LAZER, LOW);
        }
      }
      else if (touch_x > 245 && touch_x < 395 && 
              touch_y > 105 && touch_y < 155) {
        btn_state[7] = !btn_state[7]; // Muda o estado do botão EMBASAMENTO
        if (btn_state[7]){
          digitalWrite(EMBASAMENTO, HIGH);
        }
        else{
          digitalWrite(EMBASAMENTO, LOW);
        }
      }//============================================================= Acionadores ===================================================================================
      else if (touch_x > 5 && touch_x < 75 && 
              touch_y > 180 && touch_y < 230) {
        btn_state[8] = !btn_state[8]; // Muda o estado do botão 1 (Acionador)
      }
      else if (touch_x > 85 && touch_x < 155 && 
              touch_y > 180 && touch_y < 230) {
        btn_state[9] = !btn_state[9]; // Muda o estado do botão 2 (Acionador)
      }
      else if (touch_x > 165 && touch_x < 235 && 
              touch_y > 180 && touch_y < 230) {
        btn_state[10] = !btn_state[10]; // Muda o estado do botão 3 (Acionador)
      }
      else if (touch_x > 245 && touch_x < 315 && 
              touch_y > 180 && touch_y < 230) {
        btn_state[11] = !btn_state[11]; // Muda o estado do botão 4 (Acionador)
      }
      else if (touch_x > 325 && touch_x < 395 && 
              touch_y > 180 && touch_y < 230) {
        btn_state[12] = !btn_state[12]; // Muda o estado do botão 5 (Acionador)
      }
      else if (touch_x > 405 && touch_x < 475 && 
              touch_y > 180 && touch_y < 230) {
        btn_state[13] = !btn_state[13]; // Muda o estado do botão 6 (Acionador)
      }
      else if (touch_x > 83 && touch_x < 233 && 
              touch_y > 250 && touch_y < 300 &&
              !(btn_state[15])                ) {
        btn_state[14] = !btn_state[14]; // Muda o estado do botão SOBE (Acionador)
        for (int i = 8; i < 14; i++) {
          if ((btn_state[14]) && !(btn_state[15])){ //Se o SOBE estiver pressionado
            if (btn_state[i]){
              digitalWrite(pinosRecuo[i-8], LOW);
              digitalWrite(pinosAvanco[i-8], HIGH);
            }
          }
          else{ // Se soltar o botão SOBE, desliga todos os pinos de avanço
            digitalWrite(pinosAvanco[i-8], LOW);
          }
        }
      }
      else if (touch_x > 245 && touch_x < 395 && 
              touch_y > 250 && touch_y < 300  &&
              !(btn_state[14])                ) {
        btn_state[15] = !btn_state[15]; // Muda o estado do botão DESCE (Acionador)
        for (int i = 8; i < 14; i++) {
          if ((btn_state[15]) && !(btn_state[14])){ //Se o DESCE estiver pressionado
            if (btn_state[i]){
              digitalWrite(pinosAvanco[i-8], LOW);
              digitalWrite(pinosRecuo[i-8], HIGH);
            }
          }
          else{ // Se soltar o botão DESCE, desliga todos os pinos de recuo
            digitalWrite(pinosRecuo[i-8], LOW);
          }
        }
      }

      // Atualiza a tela
      drawButtons();
    }
  }
}
