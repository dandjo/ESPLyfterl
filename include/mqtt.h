#include <PubSubClient.h>
#include "restart.h"

WiFiClient espClient;
PubSubClient client(espClient);


void reconnectMqtt() {
  // loop until we're reconnected
  int i = 0;
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");
    if (client.connect("ESPLyfterl", MQTT_USERNAME, MQTT_PASSWORD, "esplyfterl/LWT", 0, true, "Offline")) {
      mqttSerial.print("Connected! ");
      client.publish("esplyfterl/LWT", "Online", true);
      client.subscribe("esplyfterl/POWER");
      client.subscribe("esplyfterl/step/set");
    } else {
      Serial.printf("Failed, rc=%d, try again in 5 seconds. ", client.state());
      unsigned long start = millis();
      while (millis() < start + 5000) {
        ArduinoOTA.handle();
      }
      if (i++ == 100) {
        Serial.printf("Tried for 500 sec, rebooting now. ");
        restart_board();
      }
    }
  }
}


void callbackPower(byte *payload, unsigned int length) {
  payload[length] = '\0';
  if (payload[0] == 'R') {
    mqttSerial.print("Rebooting. ");
    delay(100);
    restart_board();
  } else {
    mqttSerial.printf("Unknown message: %s. ", payload);
  }
}


void callbackStep(byte *payload, unsigned int length) {
  payload[length] = '\0';
  if (payload[0] == '1') {
    digitalWrite(PIN_STEP_2, RELAY_INACTIVE_STATE);
    digitalWrite(PIN_STEP_3, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_PIN_STEP_2, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_PIN_STEP_3, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_STEP_STATE, 1);
    client.publish("esplyfterl/step/state", "1");
  } else if (payload[0] == '2') {
    digitalWrite(PIN_STEP_2, RELAY_ACTIVE_STATE);
    digitalWrite(PIN_STEP_3, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_PIN_STEP_2, RELAY_ACTIVE_STATE);
    EEPROM.write(EEPROM_PIN_STEP_3, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_STEP_STATE, 2);
    client.publish("esplyfterl/step/state", "2");
  } else if (payload[0] == '3') {
    digitalWrite(PIN_STEP_2, RELAY_INACTIVE_STATE);
    digitalWrite(PIN_STEP_3, RELAY_ACTIVE_STATE);
    EEPROM.write(EEPROM_PIN_STEP_2, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_PIN_STEP_3, RELAY_ACTIVE_STATE);
    EEPROM.write(EEPROM_STEP_STATE, 3);
    client.publish("esplyfterl/step/state", "3");
  } else {
    mqttSerial.printf("Unknown message: %s. ", payload);
  }
  EEPROM.commit();
}


void callback(char *topic, byte *payload, unsigned int length) {
  Serial.printf("Message arrived [%s]: %s. ", topic, payload);
  if (strcmp(topic, "esplyfterl/POWER") == 0) {
    callbackPower(payload, length);
  } else if (strcmp(topic, "esplyfterl/step/set") == 0) {
    callbackStep(payload, length);
  } else {
    mqttSerial.printf("Unknown topic: %s. ", topic);
  }
}


void publishEepromState() {
  char state[1];
  sprintf(state, "%d", EEPROM.read(EEPROM_STEP_STATE));
  client.publish("esplyfterl/step/state", state);
}
