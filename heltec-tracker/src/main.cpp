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
#include <TinyGPSPlus.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Mensagem a ser transmitida
const char mensagem[] = "Testando LoRa... Podemos enviar mensagens com 255 bytes!";

const float FREQ = 904.0F; // Frequência (Brasil: 902-907.5/915-928 MHz)
const float BW = 125.0F;   // Largura de banda (62.5, 125.0, 250.0, 500.0 kHz)
const uint8_t SF = 9;      // Fator de espalhamento (6 a 12)
const uint8_t CR = 6;      // Taxa de codificação (5 a 8)
const uint8_t SW = 0x34;   // Sync word
const uint8_t POT = 1;     // Potência de transmissão (22 é o máximo para SX1262. Aceita valores negativos)
const uint8_t PRE_AMB = 5; // Número de símbolos do preambulo
const uint8_t CRC = 0;     // Habilita ou desabilita o CRC (0 para desabilitar, 1 para 1 byte, 2 para 2 bytes)

SX1262 Radio = new Module(CS_LoRa, DIO1_LoRa, RST_LoRa, BUSY_LoRa);
Adafruit_ST7735 Tela = Adafruit_ST7735(CS_TFT, DC_TFT, MOSI_TFT, SCLK_TFT, RST_TFT);
TinyGPSPlus Gps;

volatile bool msgRecebida = false;

ICACHE_RAM_ATTR
void flagMsgRecebida(void)
{
  msgRecebida = true;
}

void iniciaGps()
{
  Serial1.begin(115200, SERIAL_8N1, TX_GPS, RX_GPS);
}

void iniciaTela()
{
  pinMode(21, OUTPUT);
  Tela.initR(INITR_MINI160x80_PLUGIN);
  Tela.fillScreen(ST77XX_BLACK);
  Tela.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  Tela.setRotation(1);
  digitalWrite(21, HIGH);
}

void iniciaLoRa()
{
  int16_t estado = Radio.begin(FREQ, BW, SF, CR, SW, POT, PRE_AMB);

  if (estado != RADIOLIB_ERR_NONE)
  {
    Tela.println("Erro iniciando LoRa");
    for (;;)
      vTaskDelay(pdMS_TO_TICKS(1000));
  }

  Radio.setCRC(CRC);
  Radio.setDio1Action(flagMsgRecebida);
}

void piscarLed()
{
  digitalWrite(LED, HIGH);
  vTaskDelay(pdMS_TO_TICKS(50));
  digitalWrite(LED, LOW);
}

bool verificaStatusCanal(int tempoMaximoMs = 1000)
{
  unsigned long inicioVerificacao = millis();

  msgRecebida = false;
  int16_t estado = Radio.startChannelScan();
  if (estado != RADIOLIB_ERR_NONE)
  {
    return false;
  }

  while (millis() - inicioVerificacao < tempoMaximoMs)
  {
    if (msgRecebida)
    {
      msgRecebida = false;
      int16_t resultado = Radio.getChannelScanResult();

      if (resultado == RADIOLIB_CHANNEL_FREE)
      {
        return true;
      }
      else if (resultado == RADIOLIB_LORA_DETECTED)
      {
        vTaskDelay(pdMS_TO_TICKS(50));
        Radio.startChannelScan();
      }
      else
      {
        return false;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  return false;
}

int16_t transmite(const char *msg, size_t msgTamanho)
{
  if (!verificaStatusCanal(500))
  {
    return RADIOLIB_LORA_DETECTED;
  }
  int16_t estado = Radio.transmit(msg, msgTamanho);

  return estado;
}

void taskGPS(void *parametros)
{
  for (;;)
  {
    unsigned long start = millis();
    do
    {
      while (Serial1.available())
      {
        Gps.encode(Serial1.read());
      }
    } while (millis() - start < 100);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void taskTela(void *parametros)
{
  for (;;)
  {
    Tela.setCursor(0, 0);
    Tela.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    Tela.setTextSize(1);

    if (Gps.location.isValid())
    {
      Tela.printf("\n Lat: %.6f          \n", Gps.location.lat());
      Tela.printf(" Lng: %.6f          \n", Gps.location.lng());
      Tela.printf(" Sats: %d               \n", Gps.satellites.value());
      Tela.printf(" Vel: %.1f km/h     \n", Gps.speed.kmph());
      Tela.printf(" Dir: %d            \n\n", Gps.course.deg());
      Tela.printf(" Msg: %d bytes     ", sizeof(mensagem) - 1);
    }
    else
    {
      Tela.printf("\n Aguarde sinal GPS...    \n");
      Tela.printf(" Sats: %d               \n", Gps.satellites.value());
      Tela.printf(" Vel: 0.0 km/h          \n");
      Tela.printf(" Dir: 0                 \n\n");
      Tela.printf(" Msg: %d bytes     ", sizeof(mensagem) - 1);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void taskLoRa(void *parametros)
{
  unsigned long proximaTransmissao = millis() + 5000;

  for (;;)
  {
    unsigned long agora = millis();

    if (agora >= proximaTransmissao)
    {
      proximaTransmissao = agora + 5000; // Próxima transmissão em 5 segundos

      int16_t estado = transmite(mensagem, sizeof(mensagem) - 1);

      if (estado == RADIOLIB_ERR_NONE)
      {
        piscarLed();
      }
      else if (estado == RADIOLIB_LORA_DETECTED)
      {
        Tela.setCursor(10, 70);
        Tela.setTextColor(ST77XX_RED, ST77XX_BLACK);
        Tela.print("Canal ocupado, aguardando...");
        Tela.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
      }
      else
      {
        Tela.setCursor(10, 70);
        Tela.setTextColor(ST77XX_RED, ST77XX_BLACK);
        Tela.print("Erro ao transmitir");
        Tela.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup()
{
  pinMode(LED, OUTPUT); // Define o pino do led como saida
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH); // Liga a tela

  iniciaLoRa();
  iniciaGps();
  iniciaTela();

  xTaskCreate(taskGPS, "GPS Task", 4096, NULL, 2, NULL);
  xTaskCreate(taskTela, "Tela Task", 4096, NULL, 1, NULL);
  xTaskCreate(taskLoRa, "LoRa Task", 8192, NULL, 3, NULL);
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}