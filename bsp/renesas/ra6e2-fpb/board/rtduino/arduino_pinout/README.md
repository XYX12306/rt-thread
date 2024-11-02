# ra6e2-fpb Arduino Ecosystem Compatibility Guide

**English** | **[中文](README_zh.md)**

## 1 RTduino - Arduino Ecosystem Compatibility Layer for RT-Thread

The ra6e2-fpb development board has been fully adapted for the [RTduino software package](https://github.com/RTduino/RTduino), the Arduino Ecosystem compatibility layer for RT-Thread. Users can operate this BSP following Arduino programming conventions and leverage a vast array of libraries from the Arduino community, providing significant enhancement to the RT-Thread ecosystem. For more information, refer to the [RTduino Software Package Documentation](https://github.com/RTduino/RTduino).

### 1.1 Activating Arduino Ecosystem Compatibility Layer for this BSP

Enter the `menuconfig` command under the Env tool, or select RT-Thread Settings in the RT-Thread Studio IDE:

```Kconfig
Hardware Drivers Config --->
    Onboard Peripheral Drivers --->
        [*] Compatible with Arduino Ecosystem (RTduino)
```

## 2 Arduino Pin Configuration

For more information about pin layout, refer to [pins_arduino.c](pins_arduino.c) and [pins_arduino.h](pins_arduino.h).

![ra6e2-fpb-pinout-figure](ra6e2-fpb-pinout-figure.jpg)

| Arduino Pin Number | ra6e2 Pin Number | 5V Tolerance | Remarks |
| ------------------- | ----------------- | ------------ | ------------------------------------------- |
| 0 (D0) | P410 | Yes | Serial0-RX, default handled by RT-Thread's UART device framework uart0 |
| 1 (D1) | P411 | Yes | Serial0-TX, default handled by RT-Thread's UART device framework uart0 |
| 2 (D2) | P105 | Yes |  |
| 3 (D3) | P408 | Yes |  |
| 4 (D4) | P500 | Yes |  |
| 5 (D5) | P409 | Yes |  |
| 6 (D6) | P113 | Yes |  |
| 7 (D7) | P008 | Yes |  |
| 8 (D8) | P006 | Yes |  |
| 9 (D9) | P403 | Yes |  |
| 10 (D10) | P301 | Yes |  |
| 11 (D11) | P109 | Yes |  |
| 12 (D12) | P110 | Yes |  |
| 13 (D13) | P111 | Yes |  |
| 14 (D14) | P101 | Yes |  |
| 15 (D15) | P100 | Yes |  |
| 16 (A0) | P000 | Yes |  |
| 17 (A1) | P001 | Yes |  |
| 18 (A2) | P002 | Yes |  |
| 19 (A3) | P004 | Yes |  |
| 20 (A4) | P003 | Yes |  |
| 21 (A5) | P013 | Yes |  |

> Note:
> 1. RTduino currently does not support MDK; it is recommended to use the GNU GCC toolchain for compilation.
> 2. The default PWM channel for Renesas is channel 0; for detailed driver information, refer to the `bsp\renesas\libraries\HAL_Drivers\drv_pwm.c` file.