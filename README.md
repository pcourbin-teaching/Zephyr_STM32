..  Copyright 2019-present PlatformIO <contact@platformio.org>
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

How to build PlatformIO based project
=====================================

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-ststm32/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Change directory to example
$ cd platform-ststm32/examples/zephyr-blink

# Build project
$ pio run

# Upload firmware
$ pio run --target upload

# Build specific environment
$ pio run -e blackpill_f103c8

# Upload firmware for the specific environment
$ pio run -e blackpill_f103c8 --target upload

# Clean build files
$ pio run --target clean
```


Refs:
- Board on Zephyr : https://docs.zephyrproject.org/1.14.0/boards/arm/stm32f429i_disc1/doc/index.html
- Version used : 
  - PIO : Core 6.1.5 - Home 3.4.3
  - Zephyr from PIO : 2.7.1 - https://docs.platformio.org/en/stable/frameworks/zephyr.html
  - LVGL from Zephyr : 7.6.1 - https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/lib/gui/lvgl - https://docs.lvgl.io/7.11/
- Blinky Sample : https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/samples/basic/blinky
- stm32f429i_disc1 : https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/boards/arm/stm32f429i_disc1
    - DTS config : https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v2.7.1/boards/arm/stm32f429i_disc1/stm32f429i_disc1.dts
- Zephyr Devicetree (DTS) and Overlay principles :
  - https://docs.zephyrproject.org/2.7.0/guides/dts/howtos.html?highlight=overlay
  - https://www.jaredwolff.com/optimize-zephyr-config-and-overlays/
- Display
  - Display Sample : https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/samples/drivers/display
  - LVGL Sample : https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/samples/subsys/display/lvgl


Touch screen, see https://github.com/lvgl/lv_port_stm32f429_disco/blob/master/hal_stm_lvgl/stm32f429i_discovery.h
// https://github.com/CliffsDover/zephyr_lvgl_nrf52_pca10040_ili9341/blob/master/nrf52_pca10040.overlay
// https://github.com/zephyrproject-rtos/zephyr/blob/v2.7-branch/boards/arm/stm32f429i_disc1/stm32f429i_disc1.dts
// https://github.com/zephyrproject-rtos/zephyr/blob/main/boards/arm/stm32f429i_disc1/stm32f429i_disc1.dts
// https://github.com/lvgl/lv_platformio/blob/master/platformio.ini