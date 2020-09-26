# MQTT NeoPixel

This is a MQTT based controller for NeoPixels with the ESP8266/ESP32 (and propably others as well).

## Prerequisites

- A MQTT broker
- A set-up Arduino IDE for flashing ESP82xx or ESP32 controllers

## Setup

1. Clone the repository
2. Run the `./configure` (do a `chmod +x` before if it failes)
    1. (alternative) copy or create a settings.h file. You can use the settings.h.dist as a template
3. Flash the sketch trough the Arduino IDE

! Note that you have to run those commands in the shell (Mac/Linux) or in the GIT bash or Linux Subsystem (Windows) depening on your platform.

## What it can do

You can program a controller running this software with an array of `colors`. It requires the `count` of LEDs, the `pin` on which the LED strip/matix/single LED is connected to and the color `type` of the strip (A list is after this text).

Optionally you can define with a `delay`, an `offset`, a `step` and the `brightness`.

The data has to be send in the JSON format. If no colors are defined the strip will turn off.

If the strip has more LEDs then colors were specified the colors will be reused from the beginning (Example: [red, green, blue] to 5 LEDs = [red, green, blue, red, green]).

### Available types

| Color mode | Type |
| ---------- | ---- |
| RGB        | 6    |
| RBG        | 9    |
| GRB        | 82   |
| GBR        | 161  |
| BRG        | 88   |
| BGR        | 164  |
| WRGB       | 27   |
| WRBG       | 30   |
| WGRB       | 39   |
| WGBR       | 54   |
| WBRG       | 45   |
| WBGR       | 57   |
| RWGB       | 75   |
| RWBG       | 78   |
| RGWB       | 135  |
| RGBW       | 198  |
| RBWG       | 141  |
| RBGW       | 201  |
| GWRB       | 99   |
| GWBR       | 114  |
| GRWB       | 147  |
| GRBW       | 210  |
| GBWR       | 177  |
| GBRW       | 225  |
| BWRG       | 108  |
| BWGR       | 120  |
| BRWG       | 156  |
| BRGW       | 216  |
| BGWR       | 180  |
| BGRW       | 228  |

### Parameter description

| Name       | Type                                                         | Description                                                  |
| ---------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| pin        | Number (depening on your controller)                         | Defines on which pin your LED strip is connected to. The number here should be the GPIO number. In order to get it you have to take a look into your controllers specification. |
| type       | Number (from table above)                                    | Describes in which format the colors are transmitted.        |
| count      | Number (0 - 1024)                                            | The number of LEDs. NeoPixels can only be chained to 1024 LEDs |
| colors     | Array of numbers (numbers depending on your type, max amount is 2048) | The NeoPixels have a color depth of 8bit and you can transfer up to 4 channels (red, green, blue, white) depending on the selected type. So the max size in the end is a 32bit number.<br />The array itself can contain 2048 which is a storage decision, although you may be able to bump it up even more in code. |
| delay      | Number (time in ms)                                          | Describes with which rate the strip will be updated and in which speed the colors will rotate. |
| step       | Number (0 - Amount of your colors)                           | Describes how many steps colors will rotate after each delay. |
| offset     | Number (0 - Amount of your colors)                           | Describes which offset the colors should start with. This allows you to synchronize multiple controllers if needed. |
| brightness | Number (0 - 255)                                             | Sets the brightness of the strip.                            |

## Controlling it

To control the strip you have to send a JSON object with the above described parameters to the topic `DEVICE_NAME "/colors"` (the `DEVICE_NAME` is the name
for the device, that you have specified during the configure).

## Future plans

- Build a whole UI for controlling (also) this application. 
  - This should then also allow you to synchronize the strips
  - It will be linked here then
- Create an easier installer/flasher for all of my ESP focused sketches
  - This should also make it easiear and more pleasing to configure the sketch
