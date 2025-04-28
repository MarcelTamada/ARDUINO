//{Projeto_Final.INO} Arduino MEGA2560 X 3.5" TFT DISPLAY ILI9486
//Interface gráfica desenvolvida para um Display TFT touchscreen - Driver: ILI9486
//Botões para acionar saídas do mega2560, definidas abaixo.
//@author Marcel Tamada
//@since 27/04/2025

// Se a mensagem do Serial Monitor estiver saindo com caracteres estranhos, no canto superior direito do terminal do serial monitor, tem a opção para escolher o baud e nesse projeto estou utilizando 115200.
// Conferir todos os #DEFINE se estão com a numeração de pinos corretas antes de compilar e testar o código
// Se a tela ficar de cabeça para baixo ou em uma orientação diferente da desejada, alterar os parâmetros das linhas 54/55 e 88
// O código está documentado com comentários, qualquer dúvida entre em contato comigo.

// ***** BAIXAR AS BIBLIOTECAS NO "Tools->Manage Libraries" ANTES DE COMPILAR E FAZER UPLOAD *****
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000

// ******* CONFERIR OS PINOS DISPONÍVEIS NO ARDUÍNO PORQUE DEPENDENDO DO SHIELD OS PINOS LIVRES PODEM MUDAR *******
//==== Pinos de saída para LED ====
#define LED1  30
#define LED2  31
#define LED3  32
#define LED4  33
#define LED5  34
#define LED6  35
#define LAZER  36
#define EMBASAMENTO  37

// Valores obtidos pela calibração TouchScreen_Calibr_Native
const int XP = 8, XM = A2, YP = A3, YM = 9;
const int TS_LEFT = 122, TS_RT = 915, TS_TOP = 938, TS_BOT = 91;

#define NUM_BUTTONS 8
bool btn_state[NUM_BUTTONS] = {false, false, false, false, false, false, false, false}; // estado dos botões

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Criando 8 botões
Adafruit_GFX_Button btn1, btn2, btn3, btn4, btn5, btn6, btnL, btnE;

int pixel_x, pixel_y;

bool Touch_getXY(void) {
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);

  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    pixel_x = map(p.y, TS_RT, TS_LEFT, 0, tft.width());
    pixel_y = map(p.x, TS_TOP, TS_BOT, 0, tft.height());
  }
  return pressed;
}

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

void setup(void) {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  Serial.print("TFT ID = 0x");
  Serial.println(ID, HEX);
  Serial.println("Calibrado para o seu Touch Panel");

  // Inicialização dos LEDs como saída
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LAZER, OUTPUT);
  pinMode(EMBASAMENTO, OUTPUT);

  if (ID == 0xD3D3) ID = 0x9486;
  tft.begin(ID);
  tft.setRotation(1); // retrato
  tft.fillScreen(BLACK);

  uint16_t backgroundColor = YELLOW;
  //btn*.initButton(&tft, centroX, centroY, largura, altura, cor_borda,     cor_fundo,      cor_texto, "texto",    tamanho_texto);
  btn1.initButton(&tft,   45,       70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "1",            4);     // 1
  btn2.initButton(&tft,   125,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "2",            4);     // 2
  btn3.initButton(&tft,   205,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "3",            4);     // 3
  btn4.initButton(&tft,   285,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "4",            4);     // 4
  btn5.initButton(&tft,   365,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "5",            4);     // 5
  btn6.initButton(&tft,   445,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "6",            4);     // 6
  btnL.initButton(&tft,   145,      210,   190,     100,     WHITE,     backgroundColor,    BLACK,      "LAZER",        3);     // LAZER
  btnE.initButton(&tft,   347,      210,   190,     100,     WHITE,     backgroundColor,    BLACK,      "TERREO",       3);     // TÉRREO

  // Desenha os botões na tela
  btn1.drawButton(false);
  btn2.drawButton(false);
  btn3.drawButton(false);
  btn4.drawButton(false);
  btn5.drawButton(false);
  btn6.drawButton(false);
  btnL.drawButton(false);
  btnE.drawButton(false);
}

void loop(void) {
  bool down = Touch_getXY();

  btn1.press(down && btn1.contains(pixel_x, pixel_y));
  btn2.press(down && btn2.contains(pixel_x, pixel_y));
  btn3.press(down && btn3.contains(pixel_x, pixel_y));
  btn4.press(down && btn4.contains(pixel_x, pixel_y));
  btn5.press(down && btn5.contains(pixel_x, pixel_y));
  btn6.press(down && btn6.contains(pixel_x, pixel_y));
  btnL.press(down && btnL.contains(pixel_x, pixel_y));
  btnE.press(down && btnE.contains(pixel_x, pixel_y));

  // Reage quando algum botão é pressionado
  if (btn1.justPressed()) {
    delay(20);
    btn_state[0] = !btn_state[0]; // Muda o estado do botão 1
    digitalWrite(LED1, btn_state[0] ? HIGH : LOW);
    btn1.drawButton(true);
  }
  if (btn2.justPressed()) {
    delay(20);
    btn_state[1] = !btn_state[1]; // Muda o estado do botão 2
    digitalWrite(LED2, btn_state[1] ? HIGH : LOW);
    btn2.drawButton(true);
  }
  if (btn3.justPressed()) {
    delay(20);
    btn_state[2] = !btn_state[2]; // Muda o estado do botão 3
    digitalWrite(LED3, btn_state[2] ? HIGH : LOW);
    btn3.drawButton(true);
  }
  if (btn4.justPressed()) {
    delay(20);
    btn_state[3] = !btn_state[3]; // Muda o estado do botão 4
    digitalWrite(LED4, btn_state[3] ? HIGH : LOW);
    btn4.drawButton(true);
  }
  if (btn5.justPressed()) {
    delay(20);
    btn_state[4] = !btn_state[4]; // Muda o estado do botão 5
    digitalWrite(LED5, btn_state[4] ? HIGH : LOW);
    btn5.drawButton(true);
  }
  if (btn6.justPressed()) {
    delay(20);
    btn_state[5] = !btn_state[5]; // Muda o estado do botão 6
    digitalWrite(LED6, btn_state[5] ? HIGH : LOW);
    btn6.drawButton(true);
  }
  if (btnL.justPressed()) {
    delay(20);
    btn_state[6] = !btn_state[6]; // Muda o estado do botão LAZER
    digitalWrite(LAZER, btn_state[6] ? HIGH : LOW);
    btnL.drawButton(true);
  }
  if (btnE.justPressed()) {
    delay(20);
    btn_state[7] = !btn_state[7]; // Muda o estado do botão EMBASAMENTO
    digitalWrite(EMBASAMENTO, btn_state[7] ? HIGH : LOW);
    btnE.drawButton(true);
  }
  delay(120);
  // Atualiza visual dos botões ao soltar
  if (btn1.justReleased()){
    uint16_t backgroundColor = btn_state[0] ? RED : YELLOW;
    btn1.initButton(&tft,   45,       70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "1",            4);
    btn1.drawButton();
  }
  if (btn2.justReleased()){
    uint16_t backgroundColor = btn_state[1] ? RED : YELLOW;
    btn2.initButton(&tft,   125,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "2",            4);
    btn2.drawButton();
  }
  if (btn3.justReleased()) {
    uint16_t backgroundColor = btn_state[2] ? RED : YELLOW;
    btn3.initButton(&tft,   205,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "3",            4);
    btn3.drawButton();
  }
  if (btn4.justReleased()) {
    uint16_t backgroundColor = btn_state[3] ? RED : YELLOW;
    btn4.initButton(&tft,   285,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "4",            4);
    btn4.drawButton();
  }
  if (btn5.justReleased()) {
    uint16_t backgroundColor = btn_state[4] ? RED : YELLOW;
    btn5.initButton(&tft,   365,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "5",            4);
    btn5.drawButton();
  }
  if (btn6.justReleased()) {
    uint16_t backgroundColor = btn_state[5] ? RED : YELLOW;
    btn6.initButton(&tft,   445,      70,     70,     100,     WHITE,     backgroundColor,    BLACK,      "6",            4);
    btn6.drawButton();
  }
  if (btnL.justReleased()) {
    uint16_t backgroundColor = btn_state[6] ? RED : YELLOW;
    btnL.initButton(&tft,   145,      210,   190,     100,     WHITE,     backgroundColor,    BLACK,      "LAZER",        3);
    btnL.drawButton();
  }
  if (btnE.justReleased()) {
    uint16_t backgroundColor = btn_state[7] ? RED : YELLOW;
    btnE.initButton(&tft,   347,      210,   190,     100,     WHITE,     backgroundColor,    BLACK,      "TERREO",       3);
    btnE.drawButton();
  }
}
