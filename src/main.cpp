#ifdef ARDUINO_M5Stick_C_Plus
  #include <M5StickCPlus.h>
#elif ARDUINO_M5Stick_C
  #include <M5StickC.h>
#else
  #include <Arduino.h>
#endif

#ifdef ARDUINO_ARCH_ESP8266
  #include <SoftwareSerial.h>
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif

#include <HardwareSerial.h>

#include <PubSubClient.h>
#include <ArduinoOTA.h>

#include "setup.h" // <-- configure your setup here
#include "mqttserial.h"
#include "eeprom.h"
#include "mqtt.h"
#include "restart.h"


bool arduino_ota_busy = false;

#if defined(ARDUINO_M5Stick_C) || defined(ARDUINO_M6Stick_C_Plus)
  long LCDTimeout = 40000; // keep screen on for 40s then turn off. button A will turn it on again.
#endif


void setupWifi() {
  delay(10);
  // we start by connecting to a WiFi network
  mqttSerial.printf("Connecting to %s... ", WIFI_SSID);
  #if defined(WIFI_IP) && defined(WIFI_GATEWAY) && defined(WIFI_SUBNET)
    IPAddress local_IP(WIFI_IP);
    IPAddress gateway(WIFI_GATEWAY);
    IPAddress subnet(WIFI_SUBNET);

    #ifdef WIFI_PRIMARY_DNS
      IPAddress primaryDNS(WIFI_PRIMARY_DNS);
    #else
      IPAddress primaryDNS();
    #endif

    #ifdef WIFI_SECONDARY_DNS
      IPAddress secondaryDNS(WIFI_SECONDARY_DNS);
    #else
      IPAddress secondaryDNS();
    #endif

    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      mqttSerial.prrint("Failed to set static ip! ");
    }
  #endif
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i++ == 100) {
      restart_board();
    }
  }
  mqttSerial.printf("Connected. IP Address: %s. ", WiFi.localIP().toString().c_str());
}


void setupScreen() {
  #if defined(ARDUINO_M5Stick_C) || defined(ARDUINO_M5Stick_C_Plus)
    M5.begin();
    M5.Axp.EnableCoulombcounter();
    M5.Lcd.setRotation(1);
    M5.Axp.ScreenBreath(12);
    M5.Lcd.fillScreen(TFT_WHITE);
    M5.Lcd.setFreeFont(&FreeSansBold12pt7b);
    m5.Lcd.setTextDatum(MC_DATUM);
    int xpos = M5.Lcd.width() / 2; // half the screen width
    int ypos = M5.Lcd.height() / 2; // half the screen width
    M5.Lcd.setTextColor(TFT_DARKGREY);
    M5.Lcd.drawString("ESPLyfterl", xpos, ypos, 1);
    delay(2000);
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextColor(TFT_GREEN);
  #endif
}


void setup() {
  Serial.begin(115200);
  setupScreen();

  // relay pins: initial set to inactive state before configuring as outputs (avoid false triggering when initializing)
  digitalWrite(PIN_STEP_2, RELAY_INACTIVE_STATE);
  digitalWrite(PIN_STEP_3, RELAY_INACTIVE_STATE);
  pinMode(PIN_STEP_2, OUTPUT);
  pinMode(PIN_STEP_3, OUTPUT);
  #ifdef ARDUINO_M5Stick_C_Plus
    gpio_pulldown_dis(GPIO_NUM_25);
    gpio_pullup_dis(GPIO_NUM_25);
  #endif

  EEPROM.begin(10);
  restoreFromEEPROM(); // restore previous state
  setupWifi();
  ArduinoOTA.setHostname("esplyfterl");
  ArduinoOTA.onStart([]() {
    arduino_ota_busy = true;
  });

  ArduinoOTA.onError([](ota_error_t error) {
    mqttSerial.print("Error on OTA - restarting... ");
    restart_board();
  });
  ArduinoOTA.begin();

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setBufferSize(MQTT_BUFFER_SIZE); // to support large message
  client.setCallback(callback);
  client.setServer(MQTT_SERVER, MQTT_PORT);
  mqttSerial.print("Connecting to MQTT server... ");
  mqttSerial.begin(&client, "esplyfterl/log");
  reconnect();
  mqttSerial.print("ESPLyfterl started! ");
}


void loop() {
  unsigned long start = millis();
  if (!client.connected()) { // (re)connect to MQTT if needed
    reconnect();
  }
  while (millis() < start + 60 * 10 * 1000) { // block for 10 minutes
    client.loop();
    do {
      ArduinoOTA.handle();
    } while (arduino_ota_busy); // stop processing during OTA

    #if defined(ARDUINO_M5Stick_C) || defined(ARDUINO_M5Stick_C_Plus)
      if (M5.BtnA.wasPressed()) { // turn on screen
        M5.Axp.ScreenBreath(12);
        LCDTimeout = millis() + 30000;
      } else if (LCDTimeout < millis()) { // turn off screen
        M5.Axp.ScreenBreath(0);
      }
      M5.update();
    #endif
  }
  publishEepromState();
}
