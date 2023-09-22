#include <EEPROM.h>

#define EEPROM_CHK 0
#define EEPROM_LEVEL_STATE 1
#define EEPROM_PIN_LEVEL_2 2
#define EEPROM_PIN_LEVEL_3 3


void restoreFromEEPROM() {
  if ('R' == EEPROM.read(EEPROM_CHK)) {
    mqttSerial.printf(
      "Restoring previous states: %s|%s... ",
      (EEPROM.read(EEPROM_PIN_LEVEL_2) == RELAY_INACTIVE_STATE) ? "Off" : "On",
      (EEPROM.read(EEPROM_PIN_LEVEL_3) == RELAY_INACTIVE_STATE) ? "Off" : "On"
    );
    digitalWrite(PIN_LEVEL_2, EEPROM.read(EEPROM_PIN_LEVEL_2));
    digitalWrite(PIN_LEVEL_3, EEPROM.read(EEPROM_PIN_LEVEL_3));
    mqttSerial.print("Done. ");
  } else {
    mqttSerial.printf("EEPROM not initialized (%d). Initializing... ", EEPROM.read(EEPROM_CHK));
    EEPROM.write(EEPROM_CHK, 'R');
    EEPROM.write(EEPROM_PIN_LEVEL_2, RELAY_INACTIVE_STATE);
    EEPROM.write(EEPROM_PIN_LEVEL_3, RELAY_INACTIVE_STATE);
    EEPROM.commit();
    digitalWrite(PIN_LEVEL_2, RELAY_INACTIVE_STATE);
    digitalWrite(PIN_LEVEL_3, RELAY_INACTIVE_STATE);
    mqttSerial.print("Done. ");
  }
}
