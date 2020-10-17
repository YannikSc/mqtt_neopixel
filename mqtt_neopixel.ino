#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#define MQTT_MAX_PACKET_SIZE 2048

#include <EspMQTTClient.h>
#include <ArduinoJson.h>
#include <Vector.h>
#include <Adafruit_NeoPixel.h>
#include "settings.h" // This file is meant to contain settings like WIFI settings

#ifndef DEVICE_NAME
#define DEVICE_NAME "DEVICE_NAME"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID "WLAN"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "Secret"
#endif

#ifndef MQTT_BROKER_HOST
#define MQTT_BROKER_HOST "mqtt-broker.lan"
#endif

#ifndef MQTT_BROKER_PORT
#define MQTT_BROKER_PORT 1883 // The default port
#endif

#ifndef MQTT_BROKER_USERNAME
#define MQTT_BROKER_USERNAME DEVICE_NAME
#endif

#ifndef MQTT_BROKER_PASSWORD
#define MQTT_BROKER_PASSWORD "Secret"
#endif

EspMQTTClient client(
    WIFI_SSID,
    WIFI_PASSWORD,
    MQTT_BROKER_HOST,
    MQTT_BROKER_USERNAME,
    MQTT_BROKER_PASSWORD,
    DEVICE_NAME,
    MQTT_BROKER_PORT);

Adafruit_NeoPixel *pixels = NULL;
byte pin = 255;
byte type = 3;
byte brightness = 255;
unsigned short delayMs = 40;
unsigned short ledCount = 0;
byte step = 1;
unsigned int offset = 0;
int lastUpdate = 0;

uint32_t container[2048];
Vector<uint32_t> ledValues(container);

void setup()
{
  Serial.begin(115200);
  Serial.print("Connecting as ");
  Serial.println(DEVICE_NAME);

#ifdef DEBUG
  client.enableDebuggingMessages();
#endif

  client.setMaxPacketSize(32768);
}

/**
 * Extracts the parts of the color and transforms them to the required NeoPixel format
 */
void addColor(uint32_t color)
{
  //                          RRGGBBWW
  uint32_t red = (color & 0xFF000000) >> 24;
  uint32_t green = (color & 0x00FF0000) >> 16;
  uint32_t blue = (color & 0x0000FF00) >> 8;
  uint32_t white = (color & 0x000000FF);

  ledValues.push_back(Adafruit_NeoPixel::Color(red, green, blue, white));
}

/**
 * Renders the colors to the strip
 */
void updateLeds()
{
  offset = offset % ledValues.size();
  pixels->clear();

  for (int i = 0; i < ledCount; i++)
  {
    uint32_t color = ledValues.at((i + offset) % ledValues.size());
    pixels->setPixelColor(i, color);
  }

  pixels->show();

  offset += step;
}

void updateBrightness(const String &payload)
{
  brightness = payload.toInt();

  if (pixels != NULL)
  {
    pixels->setBrightness(brightness);
  }
}

void updatePin(const String &payload)
{
  pin = payload.toInt();

  if (pixels != NULL)
  {
    pixels->setPin(pin);
  }
}

void updateCount(const String &payload)
{
  ledCount = payload.toInt();

  if (pixels != NULL)
  {
    pixels->updateLength(ledCount);
  }
}

void updateType(const String &payload)
{
  type = payload.toInt();

  if (pixels != NULL)
  {
    pixels->updateType(type);
  }
}

/**
 * The callback for the MQTT library.
 * Sebscribes to the MQTT events.
 */
void onConnectionEstablished()
{
  Serial.println("Ready");

  client.publish(DEVICE_NAME "/__application", "mqtt_neopixel", true);

  client.subscribe(DEVICE_NAME "/pin", updatePin);
  client.subscribe(DEVICE_NAME "/type", updateType);
  client.subscribe(DEVICE_NAME "/count", updateCount);

  client.subscribe(DEVICE_NAME "/delay", [](const String &payload) { delayMs = payload.toInt(); });
  client.subscribe(DEVICE_NAME "/step", [](const String &payload) { step = payload.toInt(); });
  client.subscribe(DEVICE_NAME "/offset", [](const String &payload) { offset = payload.toInt(); });
  client.subscribe(DEVICE_NAME "/brightness", updateBrightness);

  client.subscribe(DEVICE_NAME "/colors", [](const String &payload) {
    StaticJsonDocument<4096> doc;
    deserializeJson(doc, payload);
    memset(container, 0, sizeof(container));
    ledValues = Vector<uint32_t>(container);

    JsonArray colors = doc.as<JsonArray>();

    initializeStrip();

    for (JsonVariant value : colors)
    {
      addColor(value);
    }

    if (ledValues.size() == 0)
    {
      ledValues.push_back(0);
    }
  });
}

void initializeStrip()
{
  if (ledCount && pin && type && brightness)
  {
    pixels = new Adafruit_NeoPixel(ledCount, pin, type + NEO_KHZ800);
    pixels->begin();
    pixels->setBrightness(brightness);
  }
}

void loop()
{
  client.loop();

  if (pixels != NULL && lastUpdate > delayMs)
  {
    lastUpdate = 0;

    updateLeds();
  }

  delay(1);

  lastUpdate += 1;
}
