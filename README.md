![Logo](https://raw.githubusercontent.com/dandjo/ESPLyfterl/main/doc/assets/logo.png)

Control your EuroAir/DomoAir ventilation system via ESP32.

Inspired by [Raomin](https://github.com/raomin) and his [ESPAltherma](https://github.com/raomin/ESPAltherma), I built this solution for home ventilation systems from the manufacturer [Swentibold](https://www.swentibold.com/). This includes the brands EuroAir and [DomoAir](https://www.domoair.at/) (Austria). ESPLyfterl works with all models that can be controlled via a 3 or 4 position switch (modes: low, medium, high, off).

# Features

* No need for extra hardware, just ESP32, two relays and some wires
* Control via MQTT and therefore all common home automation systems
* Supports update OverTheAir

# Prerequisites

## Hardware

* A [Swentibold](https://www.swentibold.com/) compatible home ventilation system
* An ESP32, my recommendation is the M5StickC Plus
* A 2 channel relay or 2 1 channel relays (low or high triggered)
* A 4 pin Grove Interface to JST (for M5StickC) or any other 4 pin connection to 2 relays
* A 5V power supply for ESP32 (USB-C for M5StickC)

![Equipment](https://raw.githubusercontent.com/dandjo/ESPLyfterl/main/doc/assets/img_equipment.jpg)

## Software

* [PlatformIO](https://platformio.org/)

# Getting started

## Step 1: Firmware

* Download the repository.
* Copy `src/setup.tpl.h` to `src/setup.h` and edit as follows:
  * Enter WiFi and MQTT settings
  * Select the GPIO pins to control the relays. For M5StickC and M5StickC Plus the pins of the Grove port are predefied (32 and 33) where 32 is to control relay 1 and 33 to control relay 2.
  * Uncomment wheter your relays are low or high triggered.
* Ready, connect the ESP32 to your computer and run the following command in your console (where `<your environment>` is e.g. `m5stickcplus`).
  ```sh
  pio run --environment <your environment> --target upload
  ```

## Step 2: Connection

* Turn OFF your home ventilation system at the circuit breaker.
* Find the appropriate connection on the board: SK6

  ![Circuit](https://raw.githubusercontent.com/dandjo/ESPLyfterl/main/doc/assets/schematic.png)
* Connect your relays (normally open) as follows (usually you have to bridge GND):
  * The GND to GND of your relays.
  * 2 to the first relay controlled by `PIN_STEP_2` (32 for M5StickC).
  * 3 to the second relay controlled by `PIN_STEP_3` (33 for M5StickC).
  * Table and picture:

    | SK6 | Relay   | Picture        |
    | --- | ------- | -------------- |
    | GND | GND     | Green/Yellow   |
    | 2   | Relay 1 | Brown          |
    | 3   | Relay 2 | White          |

    ![Connection](https://raw.githubusercontent.com/dandjo/ESPLyfterl/main/doc/assets/img_connection.jpg)

## Step 3: Integration

Almost there! To control the ESP32 via MQTT use the home automation of your choice.

### MQTT Topics

#### State Topic

`esplyfterl/step/state`

#### Command Topic

`esplyfterl/step/set`

#### openHAB Integration

I am using [openHAB](https://www.openhab.org/), so here's an example `Thing` configuration.

```yml
UID: mqtt:topic:XXXXXXXXXX:esplyfterl
label: ESPLyfterl
thingTypeUID: mqtt:topic
configuration: {}
bridgeUID: mqtt:broker:XXXXXXXXXX
channels:
  - id: esplyfterl_step
    channelTypeUID: mqtt:number
    label: ESPLyfterl Step
    configuration:
      commandTopic: esplyfterl/step/set
      stateTopic: esplyfterl/step/state
      min: 1
      max: 3
```

![openHAB channel](https://raw.githubusercontent.com/dandjo/ESPLyfterl/main/doc/assets/screenshot_openhab_channel.png)

When using sitemaps in openHAB, here's a config.

```yml
Setpoint icon="fan" label="Ventilation Step" item=esplyfterl_step minValue=1 maxValue=3 step=1
```

# Contributions

I am always happy to receive criticism, suggestions for improvement and participation. Feel free to send me bug reports or pull requests.

You can also buy me a beer via [PayPal](https://paypal.me/danielpernold).
Thanks! :-)

# License

ESPLyfterl is licensed under [MIT License](https://mit-license.org/).
