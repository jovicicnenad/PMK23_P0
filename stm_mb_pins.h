/*
 * This file contains all the pin names neccessary
 * for easy interaction with the EDS-MB for NUCLEOL476RG.
 *
 * University of Belgrade - School of Electrical Engineering
 * Department of Electronics
 * Bulevar Kralja Aleksandra 73, 11120 Belgrade, Serbia
 *
 * March 2023.
 *
 */

// LEDs, 560R in series, cathode to GND; left - red LED1;  right - blue LED2:
#define MB_LED1                                                            PA_11
#define MB_LED2                                                            PB_15
// Switches, DPST, pulled up with 10k resistor; left SW1, right SW2:
// SW_EXT connector, from left to right:

#define MB_SW1                                                             PC_9
#define MB_SW2                                                             PC_8
// Potentiometers; left POT1, right POT2:
#define MB_POT1                                                            PA_0
#define MB_POT2                                                            PA_1
// Common anode, two digit seven-segment display. All signals active at '0':
#define MB_SEL1                                                            PB_6
#define MB_SEL2                                                            PC_7
#define MB_A                                                               PA_10
#define MB_B                                                               PA_9
#define MB_C                                                               PA_8
#define MB_D                                                               PB_10
#define MB_E                                                               PB_5
#define MB_F                                                               PB_4
#define MB_G                                                               PB_3
// Piezoelectric buzzer, 10k in parallel; Active at '0':
#define MB_BUZZ                                                            PA_11
// SDD1306-based OLED display; Uses I2C_2 to communicate with NUCLEO:
#define MB_OLED_SDA                                                        PB_14
#define MB_OLED_SCL                                                        PB_13
// ESP8266 WiFi microcontroller; Uses UART_3 to communicate with NUCLEO:
#define MB_ESP_RST                                                         PA_12
#define MB_ESP_TX                                                          PC_4
#define MB_ESP_RX                                                          PC_5
#define MB_ESP_EN                                                          RESET
// mikroBUS pinout, upper leftmost pin is no. 1:
#define MB_MIKRO_AN                                                        PB_1
#define MB_MIKRO_RST                                                       PB_12
#define MB_MIKRO_CS                                                        PD_2
#define MB_MIKRO_SCK                                                       PC_10
#define MB_MIKRO_MISO                                                      PC_11
#define MB_MIKRO_MOSI                                                      PC_12
#define MB_MIKRO_SDA                                                       PB_14
#define MB_MIKRO_SCL                                                       PB_13
#define MB_MIKRO_TX                                                        PB_11
#define MB_MIKRO_RX                                                        PB_7
#define MB_MIKRO_INT                                                       PB_2
#define MB_MIKRO_PWM                                                       PC_6
// ANALOG_EXT connector, from left to right:
#define MB_AI1                                                             PC_3
#define MB_AI2                                                             PC_2
#define MB_A0                                                              PA_4