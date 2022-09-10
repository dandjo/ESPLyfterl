/* copy this file to ./setup.h and setup your credentials */
#define WIFI_SSID "xxxxx" // **your SSID here**
#define WIFI_PWD "yyyyy" // **your password here**

/* uncomment this to set a static IP instead of DHCP for the ESP (separated by commas instead of dots) */
//#define WIFI_IP 192, 168, 0, 200 
//#define WIFI_SUBNET 255, 255, 255, 0
//#define WIFI_GATEWAY 192, 168, 0, 1
//#define WIFI_PRIMARY_DNS 9, 9, 9, 9 // optional
//#define WIFI_SECONDARY_DNS 1, 1, 1, 1 // optional

#define MQTT_SERVER "192.168.0.100" // **IP address or domain of your MQTT server**
#define MQTT_USERNAME "xxxxx" // leave empty if not set (bad!)
#define MQTT_PASSWORD "yyyyy" // leave empty if not set (bad!)
#define MQTT_PORT 1883 // default port: 1883

/* the buffer size for MQTT message: DO NOT CHANGE unless you know what you are doing */
#define MQTT_BUFFER_SIZE 4096

/* PINS for controlling your relays */
#define PIN_STEP_2 32 // PIN connected to dry contact step 2 relay (normally open)
#define PIN_STEP_3 33 // PIN connected to dry contact step 3 relay (normally open)
/* define if your relay board is LOW or HIGH triggered (signal pins)
   only uncomment one of them */
//#define RELAY_HIGH_TRIGGER
#define RELAY_LOW_TRIGGER

/* DO NOT CHANGE unless you know what you are doing:
   defines the active/inactive relay states,
   according to the definition of the trigger status */
#if defined(RELAY_LOW_TRIGGER)
  #define RELAY_ACTIVE_STATE LOW
  #define RELAY_INACTIVE_STATE HIGH
#else
  #define RELAY_ACTIVE_STATE HIGH
  #define RELAY_INACTIVE_STATE LOW
#endif