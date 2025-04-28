// Bibliotecas necessárias
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>

// Configuração da tela TFT e toque
MCUFRIEND_kbv tft;
#define YP A3
#define XM A2
#define YM 9
#define XP 8
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define MINPRESSURE 200
#define MAXPRESSURE 1000
#define TS_MINX 100
#define TS_MAXX 920
#define TS_MINY 70
#define TS_MAXY 900
#define TFT_WIDTH  320
#define TFT_HEIGHT 480

// Pinos dos relés (ajuste conforme seu circuito)
const int relePins[15] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36};
bool releStates[15] = {false};

// Estrutura de botão
struct Botao {
  int x, y, w, h;
  uint16_t corOn, corOff;
  String label;
};

Botao botoes[15];

void desenhaBotao(int i) {
  Botao b = botoes[i];
  uint16_t cor = releStates[i] ? b.corOn : b.corOff;
  tft.fillRect(b.x, b.y, b.w, b.h, cor);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(b.x + 10, b.y + b.h / 2 - 8);
  tft.print(b.label + (releStates[i] ? " ON" : " OFF"));
}

void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Inicializa os pinos dos relés
  for (int i = 0; i < 15; i++) {
    pinMode(relePins[i], OUTPUT);
    digitalWrite(relePins[i], LOW);
  }

  // Configura os botões (6+3+6 layout)
  int espacoX = 10, espacoY = 10;
  int largura = 90, altura = 50;
  for (int i = 0; i < 6; i++) {
    botoes[i] = {espacoX + (largura + espacoX) * i, 30, largura, altura, TFT_GREEN, TFT_RED, String(i+1)};
  }
  for (int i = 6; i < 9; i++) {
    botoes[i] = {espacoX + (largura + espacoX) * (i-6) + largura, 100, largura, altura, TFT_GREEN, TFT_RED, String(i+1)};
  }
  for (int i = 9; i < 15; i++) {
    botoes[i] = {espacoX + (largura + espacoX) * (i-9), 170, largura, altura, TFT_GREEN, TFT_RED, String(i+1)};
  }

  for (int i = 0; i < 15; i++) desenhaBotao(i);
}

void loop() {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int x = map(p.y, TS_MINY, TS_MAXY, 0, TFT_WIDTH);
    int y = map(p.x, TS_MINX, TS_MAXX, 0, TFT_HEIGHT);

    for (int i = 0; i < 15; i++) {
      Botao b = botoes[i];
      if (x > b.x && x < (b.x + b.w) && y > b.y && y < (b.y + b.h)) {
        releStates[i] = !releStates[i];
        digitalWrite(relePins[i], releStates[i] ? HIGH : LOW);
        desenhaBotao(i);
        delay(300); // debounce
        break;
      }
    }
  }
}