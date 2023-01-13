[![License][license-shield]](LICENSE)
[![Project Maintenance][maintenance-shield]][user_profile]
[![BuyMeCoffee][buymecoffeebadge]][buymecoffee]

Examples using [Zephyr RTOS](https://www.zephyrproject.org/) on [ST STM32F429I Discovery](https://www.st.com/en/evaluation-tools/32f429idiscovery.html) board using [PlatformIO](https://platformio.org/) IDE
=====================================

This repository contains examples of codes used in teaching, particularly at [ESIEA](https://www.esiea.fr/).

3 examples are proposed:
- [NoGUI](NoGUI/) - Using the 2 LEDs on the board and the button.
   - 3 tasks are created:
     - 1 task per LED with sporadic lighting (using k_msleep)
     - 1 task allowing to light the 2 LEDs when the button has been pressed (using interruption).
  - [Un overlay](NoGUI/zephyr/boards/stm32f429i_disc1.overlay) is defined to show how to add the declaration of a new external LED, not used in the proposed code.
- [WithGUI](WithGUI/) - Using the board screen and an external sensor [BME680](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme680/) connected in I2C.
  - Screen:
     - The [LVGL](https://lvgl.io/) library is used, from the Zephyr framework.
     - The appropriate configuration is made in the [.conf](WithGUI/zephyr/prj.conf) file of the project, in particular to activate the management of:
       - DISPLAY
       - LVGL and its components (buttons, charts, labels, etc.)
       - SENSORS and in particular the BME680 driver
     - The appropriate configuration is made in the [.conf](WithGUI/zephyr/boards/stm32f429i_disc1.conf) file of the board, in particular to activate the management of:
       - SPI and the ILI9340 driver used by the screen,
       - specific characteristics of the screen for LVGL (size, color)
       - I2C used by the BME680.
     - [An overlay](WithGUI/zephyr/boards/stm32f429i_disc1.overlay) is defined to specify that the BME680 is connected to **I2C 3** of the board, with address **0x77**.
- [PeriodicTasks](PeriodicTasks/) - Creation of 2 periodic tasks (using timers) with display on the LEDs integrated into the board and with a configurable duration (use of 'nop' to simulate the use of the processor).

Some details on versions used:

| Tool                                                                        | Version                                                                                                          |
|-----------------------------------------------------------------------------:|-------------------------------------------------------------------------------------------------------------------|
| [PlatformIO (PIO)](https://platformio.org/)                                 | Core 6.1.5 - Home 3.4.3                                                                                          |
| [Zephyr RTOS](https://docs.platformio.org/en/stable/frameworks/zephyr.html) | 2.7.1 (linked with PIO 6.1.5)                                                                                    |
| [LVGL](https://docs.lvgl.io/7.11/)                                          | 7.6.1 (linked with [Zephyr 2.7.1](https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/lib/gui/lvgl)) |

Some useful links:

| Link | Details |
|-----:|--------|
| [ST STM32F429I Discovery](https://docs.zephyrproject.org/2.7.0/boards/arm/stm32f429i_disc1/doc/index.html) | Details of the board on Zephyr doc |
| [ST STM32F429I Discovery Pin Mapping](https://os.mbed.com/platforms/ST-Discovery-F429ZI/) | Pin mapping of the board, from MBED doc |
| [ST STM32F429I Discovery Zephyr conf](https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/boards/arm/stm32f429i_disc1) | Folder in Zephyr defining the board details |
| [ST STM32F429I Discovery Zephyr DTS](https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v2.7.1/boards/arm/stm32f429i_disc1/stm32f429i_disc1.dts) | DTS file in Zephyr defining the board details |


Refs:
- Zephyr Devicetree (DTS) and Overlay principles :
  - https://docs.zephyrproject.org/2.7.0/guides/dts/howtos.html?highlight=overlay
  - https://www.jaredwolff.com/optimize-zephyr-config-and-overlays/
- Display
  - Display Sample : https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/samples/drivers/display
  - LVGL Sample : https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/samples/subsys/display/lvgl

- Zephyr
  - Create Thread : https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v2.7.1/tests/kernel/threads/dynamic_thread/src/main.c
  - https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/tests/kernel/threads
  - Sleep and periodic tasks with timers or work queue : https://lists.zephyrproject.org/g/users/topic/drift_through_k_sleep_or/80362499?p=
Touch screen, see https://github.com/lvgl/lv_port_stm32f429_disco/blob/master/hal_stm_lvgl/stm32f429i_discovery.h
// https://github.com/CliffsDover/zephyr_lvgl_nrf52_pca10040_ili9341/blob/master/nrf52_pca10040.overlay
// https://github.com/zephyrproject-rtos/zephyr/blob/v2.7-branch/boards/arm/stm32f429i_disc1/stm32f429i_disc1.dts
// https://github.com/zephyrproject-rtos/zephyr/blob/main/boards/arm/stm32f429i_disc1/stm32f429i_disc1.dts
// https://github.com/lvgl/lv_platformio/blob/master/platformio.ini


Scheduling ? https://academy.nordicsemi.com/topic/exercise-3-7/


[license-shield]: https://img.shields.io/github/license/pcourbin-teaching/Zephyr_STM32.svg
[maintenance-shield]: https://img.shields.io/badge/maintainer-%40pcourbin-blue.svg
[buymecoffee]: https://www.buymeacoffee.com/pcourbin
[buymecoffeebadge]: https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg
[user_profile]: https://github.com/pcourbin