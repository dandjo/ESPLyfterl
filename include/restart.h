#ifndef restart_h
  #define restart_h

  #include <Arduino.h>

  void restart_board() {
    #if defined(ARDUINO_ARCH_ESP8266)
      system_restart();
    #else
      esp_restart();
    #endif
  }
#endif