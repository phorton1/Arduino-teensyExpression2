EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L 0_my_symbols:rpi_connector J?
U 1 1 66123A43
P 4500 3650
F 0 "J?" H 5528 3685 50  0000 L CNN
F 1 "rpi_connector" H 5528 3594 50  0000 L CNN
F 2 "" H 4400 3600 50  0001 C CNN
F 3 "" H 4400 3600 50  0001 C CNN
	1    4500 3650
	1    0    0    -1  
$EndComp
Text GLabel 4450 3000 1    50   Input Italic 0
DISPLAY_MISO
Text GLabel 4550 3000 1    50   Input Italic 0
DISPLAY_MOSI
Text GLabel 4350 3000 1    50   Input Italic 0
DISPLAY_SCLK
Text GLabel 4650 5550 3    50   Input ~ 0
RPI_BLUE_9486_HARDWIRED_CD
Text GLabel 4450 5400 3    50   Input ~ 0
RPI_BLUE_9486_HARDIWIRED_RESET
Text GLabel 4350 4350 3    50   Input Italic 0
DISPLAY_LCD_CS
Text GLabel 4250 4350 3    50   Input Italic 0
DISPLAY_TOUCH_CS
Text GLabel 4950 4350 3    50   Input ~ 0
LOOPER_I2S_BCLK
Text GLabel 3750 3000 1    50   Input ~ 10
AUDIO_I2S_FCLK
Text GLabel 3650 4350 3    50   Input ~ 10
AUDIO_I2S_RXD
Text GLabel 3550 4350 3    50   Input ~ 10
AUDIO_I2S_TXD
Text GLabel 3950 4350 3    50   Input ~ 0
PRH_AUDIO_RX_ACTIVE
Text GLabel 3750 4350 3    50   Input ~ 0
PRH_AUDIO_TX_ACTIVE
Text GLabel 4450 4350 3    50   Input ~ 0
PRH_SYSTEM_RPI_READY
Text GLabel 5050 4350 3    50   Input Italic 0
RPI_RX0
Text GLabel 5150 4350 3    50   Input Italic 0
RPI_TX0
Text GLabel 5250 4350 3    50   Input ~ 0
RPI_GND
Text GLabel 5350 4350 3    50   Input ~ 0
RPI_5V
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 661586DE
P 7100 2050
F 0 "J?" V 7064 1562 50  0000 R CNN
F 1 "Conn_01x08" V 6973 1562 50  0000 R CNN
F 2 "" H 7100 2050 50  0001 C CNN
F 3 "~" H 7100 2050 50  0001 C CNN
	1    7100 2050
	0    1    -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 66159D8D
P 7150 5100
F 0 "J?" V 7022 5480 50  0000 L CNN
F 1 "Conn_01x08" V 7113 5480 50  0000 L CNN
F 2 "" H 7150 5100 50  0001 C CNN
F 3 "~" H 7150 5100 50  0001 C CNN
	1    7150 5100
	0    1    1    0   
$EndComp
Text GLabel 7150 4900 1    50   Input ~ 0
RPI_RX0
Text GLabel 7250 4900 1    50   Input ~ 0
RPI_TX0
Text GLabel 7350 4900 1    50   Input ~ 0
RPI_GND
Text GLabel 7450 4900 1    50   Input ~ 0
RPI_5V
Text GLabel 7050 4900 1    50   Input ~ 0
PRH_SYSTEM_RPI_READY
Text GLabel 6950 4900 1    50   Input ~ 0
PRH_AUDIO_RX_ACTIVE
Text GLabel 6850 4900 1    50   Input ~ 0
PRH_AUDIO_TX_ACTIVE
Text GLabel 6750 4900 1    50   Input ~ 0
RPI_RUN
Text GLabel 2800 3750 0    50   Input ~ 0
RPI_RUN
Text GLabel 7300 2250 3    50   Input ~ 0
RPI_GND
Text GLabel 7400 2250 3    50   Input ~ 0
RPI_5V
Text GLabel 7200 2250 3    50   Input ~ 0
DISPLAY_LCD_CS
Text GLabel 6800 2250 3    50   Input ~ 0
DISPLAY_MISO
Text GLabel 7000 2250 3    50   Input ~ 0
DISPLAY_MOSI
Text GLabel 6900 2250 3    50   Input ~ 0
DISPLAY_SCLK
Text GLabel 7100 2250 3    50   Input ~ 0
DISPLAY_CD
Text GLabel 6700 2250 3    50   Input ~ 0
DISPLAY_TOUCH_CS
Wire Notes Line
	4300 3100 4300 3450
Wire Notes Line
	4300 3450 4600 3450
Wire Notes Line
	4600 3450 4600 3100
Wire Notes Line
	5000 4250 5000 3900
Wire Notes Line
	5000 3900 5500 3900
Text GLabel 5450 4350 3    50   Input ~ 0
DISPLAY_5V
Text GLabel 4950 3000 1    50   Input ~ 0
UNUSED_DISPLAY_TOUCH_IRQ
Text GLabel 4550 4350 3    50   Input ~ 0
DISPLAY_GND
Wire Notes Line
	4200 4250 4200 3900
Wire Notes Line
	4200 3900 4700 3900
Wire Notes Line
	4700 3900 4700 4250
Wire Notes Line
	5400 3900 5400 4250
Wire Notes Line
	3700 4250 3700 3900
Wire Notes Line
	3700 3900 4000 3900
Wire Notes Line
	4000 3900 4000 4250
Text GLabel 4050 3000 1    50   Input ~ 10
OCTO_RESET
Text GLabel 4850 3000 1    50   Input ~ 10
OCTO_MULT0
Text GLabel 4750 3000 1    50   Input ~ 10
OCTO_MULT1
Text GLabel 4750 4350 3    50   Input ~ 10
OCTO_MULT2
Text GLabel 4650 4350 3    50   Input ~ 10
OCTO_MULT3
Text Notes 850  7100 0    50   ~ 0
SPI1 uses the following GPIO pins:\nMOSI: GPIO 20 (BCM 19)\nMISO: GPIO 19 (BCM 10)\nSCLK: GPIO 21 (BCM 9)\nCE2: GPIO 18 (BCM 24)\nSPI2 (AUX_SPI1):\nSPI2 uses the following GPIO pins:\nMOSI: GPIO 27 (BCM 16)\nMISO: GPIO 26 (BCM 12)\nSCLK: GPIO 28 (BCM 20)\nCE2: GPIO 29 (BCM 5)
Text GLabel 5350 3000 1    50   Input ~ 10
OCTO_SDA
Text GLabel 5250 3000 1    50   Input ~ 10
OCTO_SCL
Text GLabel 5150 3000 1    50   Input ~ 0
DISPLAY_CD
$EndSCHEMATC
