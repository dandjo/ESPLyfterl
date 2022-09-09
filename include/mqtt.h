#include <PubSubClient.h>
#include <EEPROM.h>
#include "restart.h"

#define EEPROM_CHK 0
#define EEPROM_STATE_2 1
#define EEPROM_STATE_3 2

WiFiClient espClient;
PubSubClient client(espClient);


void restoreFromEEPROM() {
  if ('R' == EEPROM.read(EEPROM_CHK)) {
    digitalWrite(PIN_STEP_2, EEPROM.read(EEPROM_STATE_2));
    digitalWrite(PIN_STEP_3, EEPROM.read(EEPROM_STATE_3));
    mqttSerial.printf(
      "Restoring previous states: %s | %s",
      (EEPROM.read(EEPROM_STATE_2) == RELAY_INACTIVE_STATE) ? "Off" : "On",
      (EEPROM.read(EEPROM_STATE_3) == RELAY_INACTIVE_STATE) ? "Off" : "On"
    );
  } else {
    mqttSerial.printf("EEPROM not initialized (%d). Initializing...", EEPROM.read(EEPROM_CHK));
    EEPROM.write(EEPROM_CHK, 'R');
    EEPROM.write(EEPROM_STATE_2, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_STATE_3, RELAY_INACTIVE_STATE);
    EEPROM.commit();
    digitalWrite(PIN_STEP_2, RELAY_INACTIVE_STATE);
    digitalWrite(PIN_STEP_3, RELAY_INACTIVE_STATE);
  }
}


void reconnect() {
  // loop until we're reconnected
  int i = 0;
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESPLyfterl", MQTT_USERNAME, MQTT_PASSWORD, "esplyfterl/lwt", 0, true, "Offline")) {
      Serial.println("connected!");
      client.publish("esplyfterl/lwt", "Online", true);
      client.subscribe("esplyfterl/reboot");
      client.subscribe("esplyfterl/step/set");
    } else {
      Serial.printf("failed, rc=%d, try again in 5 seconds", client.state());
      unsigned long start = millis();
      while (millis() < start + 5000) {
        ArduinoOTA.handle();
      }
      if (i++ == 100) {
        Serial.printf("Tried for 500 sec, rebooting now.");
        restart_board();
      }
    }
  }
}


void callbackReboot(byte *payload, unsigned int length) {
  mqttSerial.println("Rebooting");
  delay(100);
  restart_board();
}


void callbackStep(byte *payload, unsigned int length) {
  payload[length] = '\0';

  if (payload[0] == '1') {
    digitalWrite(PIN_STEP_2, RELAY_INACTIVE_STATE);
    digitalWrite(PIN_STEP_3, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_STATE_2, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_STATE_3, RELAY_INACTIVE_STATE);
    client.publish("esplyfterl/step/state", "1");
    Serial.println("Set state to 1");
  } else if (payload[0] == '2') {
    digitalWrite(PIN_STEP_2, RELAY_ACTIVE_STATE);
    digitalWrite(PIN_STEP_3, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_STATE_2, RELAY_ACTIVE_STATE);
    EEPROM.write(EEPROM_STATE_3, RELAY_INACTIVE_STATE);
    client.publish("esplyfterl/step/state", "2");
    Serial.println("Set state to 2");
  } else if (payload[0] == '3') {
    digitalWrite(PIN_STEP_2, RELAY_INACTIVE_STATE);
    digitalWrite(PIN_STEP_3, RELAY_ACTIVE_STATE);
    EEPROM.write(EEPROM_STATE_2, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_STATE_3, RELAY_ACTIVE_STATE);
    client.publish("esplyfterl/step/state", "3");
    Serial.println("Set state to 3");
  } else {
    Serial.printf("Unknown message: %s\n", payload);
  }
  EEPROM.commit();
}


void callback(char *topic, byte *payload, unsigned int length) {
  Serial.printf("Message arrived [%s] : %s\n", topic, payload);
  if (strcmp(topic, "esplyfterl/reboot") == 0) {
    callbackReboot(payload, length);
  } else if (strcmp(topic, "esplyfterl/step/set") == 0) {
    callbackStep(payload, length);
  } else {
    Serial.printf("Unknown topic: %s\n", topic);
  }
}