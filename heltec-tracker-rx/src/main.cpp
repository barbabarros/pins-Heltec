// autor: Pedro Luiz de Barros Junior
// 16/07/2025

// Definições dos pinos
// Vext = 3;
// LED = 18; // LED_BUILTIN

// CS_LoRa = 8;    // LoRa_NSS
// SCK_LoRa = 9;   // LoRa_SCK
// MOSI_LoRa = 10; // LoRa_MOSI
// MISO_LoRa = 11; // LoRa_MISO
// RST_LoRa = 12;  // LoRa_RST
// BUSY_LoRa = 13; // LoRa_Busy
// DIO1_LoRa = 14; // LoRa_DIO1

// TX_GPS = 33;  // GNSS_TX
// RX_GPS = 34;  // GNSS_RX
// RST_GPS = 35; // GNSS_RST
// PPS_GPS = 36; // GNSS_PPS

// BKLIGHT_TFT = 21; // TFT_LED_K (backlight)
// CS_TFT = 38;      // TFT_CS
// RST_TFT = 39;     // TFT_RES
// DC_TFT = 40;      // TFT_RS
// SCLK_TFT = 41;    // TFT_SCLK
// MOSI_TFT = 42;    // TFT_SDIN

#include <RadioLib.h>
#include <Adafruit_ST7735.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

const float FREQ = 904.0F;
const float BW = 125.0F;
const uint8_t SF = 9;
const uint8_t CR = 6;
const uint8_t SW = 0x34;
const uint8_t POT = 1;
const uint8_t PRE_AMB = 5;
const uint8_t CRC = 0;

SX1262 Radio = new Module(CS_LoRa, DIO1_LoRa, RST_LoRa, BUSY_LoRa);
Adafruit_ST7735 Tela = Adafruit_ST7735(CS_TFT, DC_TFT, MOSI_TFT, SCLK_TFT, RST_TFT);
uint8_t rxBuffer[256];

volatile bool msgRecebida = false;

ICACHE_RAM_ATTR
void flagMsgRecebida(void)
{
  msgRecebida = true;
}

void iniciaTela()
{
  pinMode(BKLIGHT_TFT, OUTPUT);
  digitalWrite(BKLIGHT_TFT, HIGH);

  Tela.initR(INITR_MINI160x80_PLUGIN);
  Tela.fillScreen(ST77XX_BLACK);
  Tela.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  Tela.setRotation(1);
  Tela.setTextSize(1);
  Tela.setCursor(0, 0);

  Tela.println("Sistema iniciado!");
  Tela.println("Aguardando mensagens...");
}

void iniciaLoRa()
{
  int16_t estado = Radio.begin(FREQ, BW, SF, CR, SW, POT, PRE_AMB);

  if (estado != RADIOLIB_ERR_NONE)
  {
    Tela.fillScreen(ST77XX_BLACK);
    Tela.setCursor(0, 0);
    Tela.print("Erro LoRa: ");
    Tela.println(estado);
    for (;;)
      vTaskDelay(pdMS_TO_TICKS(1000));
  }

  Radio.setCRC(CRC);
  Radio.setPacketReceivedAction(flagMsgRecebida);

  estado = Radio.startReceive();
  if (estado != RADIOLIB_ERR_NONE)
  {
    Tela.fillScreen(ST77XX_BLACK);
    Tela.setCursor(0, 0);
    Tela.print("Erro RX: ");
    Tela.println(estado);
  }
}

void piscarLed()
{
  digitalWrite(LED, HIGH);
  vTaskDelay(pdMS_TO_TICKS(50));
  digitalWrite(LED, LOW);
}

void taskMensagem(void *parametros)
{
  static uint16_t msgCount = 0;

  for (;;)
  {
    if (msgRecebida)
    {
      msgRecebida = false;

      int16_t estado = Radio.readData(rxBuffer, 0);
      if (estado == RADIOLIB_ERR_NONE)
      {
        size_t tamanhoMsg = Radio.getPacketLength();

        if (msgCount % 3 == 0)
        {
          Tela.fillScreen(ST77XX_BLACK);
          Tela.setCursor(0, 0);
        }

        msgCount++;

        Tela.print("Msg ");
        Tela.print(msgCount);
        Tela.print(": ");

        rxBuffer[tamanhoMsg] = '\0';
        Tela.println((char *)rxBuffer);

        piscarLed();
      }
      else
      {
        Tela.print("Erro: ");
        Tela.println(estado);
      }

      Radio.startReceive();
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);

  iniciaTela();
  iniciaLoRa();

  xTaskCreate(taskMensagem, "Mensagem Task", 4096, NULL, 1, NULL);
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}