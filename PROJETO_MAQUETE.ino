#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>

MCUFRIEND_kbv tft;
#define MINPRESSURE 200
#define MAXPRESSURE 1000

// Define pinos do touchscreen (ajuste conforme necessário)
#define YP A3
#define XM A2
#define YM 9
#define XP 8

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Mapeia a tela
#define TS_MINX 100
#define TS_MAXX 920
#define TS_MINY 70
#define TS_MAXY 900

// Tamanho do display (ajustar se for diferente)
#define TFT_WIDTH  320
#define TFT_HEIGHT 480

// Botões
#define BTN_W 50
#define BTN_H 50
#define SPACING 10

// Estados dos relés
bool relayState[15] = {false};
int relayPins[15] = {
  22, 23, 24, 25, 26, 27,  // Linha 1
  28, 29, 30,              // Linha 2 (centro)
  31, 32, 33, 34, 35, 36   // Linha 3
};

// Posições dos botões (6 + 3 + 6 layout)
int btnX[15], btnY[15];

void drawButton(int i, bool pressed) {
  uint16_t color = pressed ? TFT_GREEN : TFT_RED;
  tft.fillRect(btnX[i], btnY[i], BTN_W, BTN_H, color);
  tft.drawRect(btnX[i], btnY[i], BTN_W, BTN_H, TFT_BLACK);
  tft.setCursor(btnX[i] + 15, btnY[i] + 20);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.print(i + 1);
}

void setupButtons() {
  int rowOffsets[] = {SPACING, 3 * (BTN_H + SPACING), 6 * (BTN_H + SPACING)};
  int btnPerRow[] = {6, 3, 6};

  int index = 0;
  for (int row = 0; row < 3; row++) {
    int totalBtns = btnPerRow[row];
    int y = rowOffsets[row];

    int rowWidth = totalBtns * BTN_W + (totalBtns - 1) * SPACING;
    int xStart = (TFT_WIDTH - rowWidth) / 2;

    for (int col = 0; col < totalBtns; col++) {
      btnX[index] = xStart + col * (BTN_W + SPACING);
      btnY[index] = y;
      drawButton(index, false);
      index++;
    }
  }
}

void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);

  setupButtons();

  for (int i = 0; i < 15; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW); // Inicialmente desligado
  }
}

void loop() {
  TSPoint p = ts.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int x = map(p.y, TS_MINY, TS_MAXY, 0, TFT_WIDTH);
    int y = map(p.x, TS_MINX, TS_MAXX, 0, TFT_HEIGHT);

    for (int i = 0; i < 15; i++) {
      if (x > btnX[i] && x < (btnX[i] + BTN_W) &&
          y > btnY[i] && y < (btnY[i] + BTN_H)) {
        relayState[i] = !relayState[i];
        digitalWrite(relayPins[i], relayState[i] ? HIGH : LOW);
        drawButton(i, relayState[i]);
        delay(300);  // Debounce
      }
    }
  }
}
