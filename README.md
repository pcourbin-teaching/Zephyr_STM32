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
     - 1 task allowing lighting the 2 LEDs when the button has been pressed (using interruption).
  - [Un overlay](NoGUI/zephyr/boards/stm32f429i_disc1.overlay) is defined to show how to add the declaration of a new external LED, not used in the proposed code.
- [WithGUI](WithGUI/) - Using the board screen, an image and an external sensor [BME680](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme680/) connected in I2C.
Temperature and humidity values are added to a chart and regularly printed on the screen using labels.
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

| Tool | Version |
|---|---|
| [PlatformIO (PIO)](https://platformio.org/) | Core 6.1.5 - Home 3.4.3 |
| [Zephyr RTOS](https://docs.platformio.org/en/stable/frameworks/zephyr.html) | 2.7.1 (linked with PIO 6.1.5) |
| [LVGL](https://docs.lvgl.io/7.11/) | 7.6.1 (linked with [Zephyr 2.7.1](https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/lib/gui/lvgl)) |

Some useful links about the board:

| Link | Details |
|---|---|
| [ST STM32F429I Discovery](https://docs.zephyrproject.org/2.7.0/boards/arm/stm32f429i_disc1/doc/index.html) | Details of the board on Zephyr doc |
| [ST STM32F429I Discovery Pin Mapping](https://os.mbed.com/platforms/ST-Discovery-F429ZI/) | Pin mapping of the board, from MBED doc |
| [ST STM32F429I Discovery Zephyr conf](https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/boards/arm/stm32f429i_disc1) | Folder in Zephyr defining the board details |
| [ST STM32F429I Discovery Zephyr DTS](https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v2.7.1/boards/arm/stm32f429i_disc1/stm32f429i_disc1.dts) | DTS file in Zephyr defining the board details |

Some useful links about the Zephyr:
| Link | Details |
|---|---|
| Zephyr Devicetree (DTS) and Overlay principles [1](https://docs.zephyrproject.org/2.7.0/guides/dts/howtos.html) [2](https://www.jaredwolff.com/optimize-zephyr-config-and-overlays/) | Details on how to use DTS and overlays |
| [Basic Display](https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/samples/drivers/display) | Example using basic Display code on Zephyr |
| [LVGL]](https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/samples/subsys/display/lvgl) | Example using LVGL and Display code on Zephyr |
| [Zephyr Threads](https://github.com/zephyrproject-rtos/zephyr/tree/zephyr-v2.7.1/tests/kernel/threads) | Doc on Thread in Zephyr |
| [Sleep and periodic tasks](https://lists.zephyrproject.org/g/users/topic/drift_through_k_sleep_or/80362499) | Discussion on how to create periodic tasks on Zephyr |

TODO, integrate touch screen on Zephyr, some useful links :
- Code using LVGL and the touch screen using HAL: https://github.com/lvgl/lv_port_stm32f429_disco/blob/master/hal_stm_lvgl/stm32f429i_discovery.h
- Code of DTS for the board on last version of Zephyr https://github.com/zephyrproject-rtos/zephyr/blob/main/boards/arm/stm32f429i_disc1/stm32f429i_disc1.dts
- Example of PlatformIO project using LVGL and BSP (BSP-stmpe811) to integrate the touch screen: https://github.com/lvgl/lv_platformio/blob/master/platformio.ini

TODO, see examples of teaching resources using Zephyr:
- https://academy.nordicsemi.com/topic/exercise-3-7/


[license-shield]: https://img.shields.io/github/license/pcourbin-teaching/Zephyr_STM32.svg
[maintenance-shield]: https://img.shields.io/badge/maintainer-%40pcourbin-blue.svg
[buymecoffee]: https://www.buymeacoffee.com/pcourbin
[buymecoffeebadge]: https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg
[user_profile]: https://github.com/pcourbin