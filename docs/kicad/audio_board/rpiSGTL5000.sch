EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A2 23386 16535
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
L power:GND #PWR013
U 1 1 66610187
P 3200 5200
F 0 "#PWR013" H 3200 4950 50  0001 C CNN
F 1 "GND" V 3200 5000 50  0000 C CNN
F 2 "" H 3200 5200 50  0001 C CNN
F 3 "" H 3200 5200 50  0001 C CNN
	1    3200 5200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 6660F832
P 3600 5200
F 0 "#PWR014" H 3600 4950 50  0001 C CNN
F 1 "GND" V 3600 5000 50  0000 C CNN
F 2 "" H 3600 5200 50  0001 C CNN
F 3 "" H 3600 5200 50  0001 C CNN
	1    3600 5200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 6660E5AB
P 3400 4000
F 0 "#PWR08" H 3400 3750 50  0001 C CNN
F 1 "GND" V 3400 3800 50  0000 C CNN
F 2 "" H 3400 4000 50  0001 C CNN
F 3 "" H 3400 4000 50  0001 C CNN
	1    3400 4000
	-1   0    0    1   
$EndComp
Text GLabel 3300 4000 1    50   Input Italic 0
RPI_CD
Text GLabel 3500 5200 3    50   UnSpc ~ 0
RPI_TX
Text GLabel 3400 5200 3    50   UnSpc ~ 0
RPI_RX
Text GLabel 3300 5200 3    50   Output ~ 10
I2S_BCLK
Text GLabel 2150 6000 0    50   Input ~ 0
RPI_I2S_TX_ACTIVE
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 66503904
P 1550 3100
F 0 "J2" H 1500 3250 50  0001 L CNN
F 1 "RPI_RUN" H 1630 3051 50  0001 L CNN
F 2 "0_my_footprints2:Oval1Pin" H 1550 3100 50  0001 C CNN
F 3 "~" H 1550 3100 50  0001 C CNN
	1    1550 3100
	1    0    0    -1  
$EndComp
Text GLabel 1350 3100 0    50   Input ~ 0
RPI_RUN
Text GLabel 2100 4000 1    50   Output ~ 10
I2S_FCLK
Text GLabel 2000 5200 3    50   Output ~ 10
I2S_RX
Text GLabel 1900 5200 3    50   Output ~ 10
I2S_TX
Text GLabel 2900 4000 1    50   Input ~ 0
RPI_MOSI
Text GLabel 2800 4000 1    50   Input ~ 0
RPI_MISO
Text GLabel 2700 4000 1    50   Input ~ 0
RPI_SCLK
Text GLabel 2700 5200 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 2600 5200 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 2100 5200 3    50   UnSpc ~ 0
RPI_READY
Text GLabel 2900 6000 2    50   Input ~ 0
RPI_I2S_RX_ACTIVE
$Comp
L power:GND #PWR07
U 1 1 6660E9EA
P 2600 4000
F 0 "#PWR07" H 2600 3750 50  0001 C CNN
F 1 "GND" V 2600 3800 50  0000 C CNN
F 2 "" H 2600 4000 50  0001 C CNN
F 3 "" H 2600 4000 50  0001 C CNN
	1    2600 4000
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 6660EE40
P 1900 4000
F 0 "#PWR06" H 1900 3750 50  0001 C CNN
F 1 "GND" V 1900 3800 50  0000 C CNN
F 2 "" H 1900 4000 50  0001 C CNN
F 3 "" H 1900 4000 50  0001 C CNN
	1    1900 4000
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR012
U 1 1 666109AB
P 2900 5200
F 0 "#PWR012" H 2900 4950 50  0001 C CNN
F 1 "GND" V 2900 5000 50  0000 C CNN
F 2 "" H 2900 5200 50  0001 C CNN
F 3 "" H 2900 5200 50  0001 C CNN
	1    2900 5200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 666110E8
P 2400 5200
F 0 "#PWR011" H 2400 4950 50  0001 C CNN
F 1 "GND" V 2400 5000 50  0000 C CNN
F 2 "" H 2400 5200 50  0001 C CNN
F 3 "" H 2400 5200 50  0001 C CNN
	1    2400 5200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 66611616
P 2200 5200
F 0 "#PWR010" H 2200 4950 50  0001 C CNN
F 1 "GND" V 2200 5000 50  0000 C CNN
F 2 "" H 2200 5200 50  0001 C CNN
F 3 "" H 2200 5200 50  0001 C CNN
	1    2200 5200
	1    0    0    -1  
$EndComp
$Comp
L 0_my_symbols:rpiA J4
U 1 1 66521D2F
P 2850 4850
F 0 "J4" H 2900 5000 50  0001 C CNN
F 1 "rpiA" H 2950 5000 50  0000 C CNN
F 2 "0_my_footprints2:pinSocket1x20_bottom" H 2700 4700 50  0001 C CNN
F 3 "" H 2700 4700 50  0001 C CNN
	1    2850 4850
	1    0    0    -1  
$EndComp
$Comp
L 0_my_symbols:rpiB J3
U 1 1 66525118
P 2850 4350
F 0 "J3" H 3050 4300 50  0001 C CNN
F 1 "rpiB" H 3200 4300 50  0000 C CNN
F 2 "0_my_footprints2:pinSocket1x20_bottom" H 2750 3950 50  0001 C CNN
F 3 "" H 2750 3950 50  0001 C CNN
	1    2850 4350
	1    0    0    -1  
$EndComp
Text GLabel 1950 2350 3    50   Input ~ 0
RPI_RUN
Text GLabel 1850 2350 3    50   Input ~ 0
RPI_TX
Text GLabel 1750 2350 3    50   Input ~ 0
RPI_RX
Text GLabel 2050 2350 3    50   Input ~ 0
RPI_READY
$Comp
L power:GND #PWR01
U 1 1 6644CAB9
P 1650 2350
F 0 "#PWR01" H 1650 2100 50  0001 C CNN
F 1 "GND" V 1650 2150 50  0000 C CNN
F 2 "" H 1650 2350 50  0001 C CNN
F 3 "" H 1650 2350 50  0001 C CNN
	1    1650 2350
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 6645117F
P 4500 2600
F 0 "#PWR03" H 4500 2350 50  0001 C CNN
F 1 "GND" V 4500 2400 50  0000 C CNN
F 2 "" H 4500 2600 50  0001 C CNN
F 3 "" H 4500 2600 50  0001 C CNN
	1    4500 2600
	-1   0    0    -1  
$EndComp
Text GLabel 4200 2600 3    50   Input ~ 0
RPI_MOSI
Text GLabel 4300 2600 3    50   Input ~ 0
RPI_MISO
Text GLabel 4400 2600 3    50   Input ~ 0
RPI_SCLK
Text GLabel 3900 2600 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 4000 2600 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 4100 2600 3    50   Input ~ 0
RPI_CD
$Comp
L Device:LED D1
U 1 1 6647603F
P 2300 6500
F 0 "D1" H 2293 6245 50  0001 C CNN
F 1 "yellow" H 2293 6337 50  0000 C CNN
F 2 "0_my_footprints2:led_yellow" H 2300 6500 50  0001 C CNN
F 3 "~" H 2300 6500 50  0001 C CNN
	1    2300 6500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 66478725
P 2300 6200
F 0 "R1" V 2093 6200 50  0001 C CNN
F 1 "220" V 2300 6200 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 2230 6200 50  0001 C CNN
F 3 "~" H 2300 6200 50  0001 C CNN
	1    2300 6200
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D2
U 1 1 66485268
P 2800 6500
F 0 "D2" H 2793 6245 50  0001 C CNN
F 1 "yellow" H 2793 6337 50  0000 C CNN
F 2 "0_my_footprints2:led_yellow" H 2800 6500 50  0001 C CNN
F 3 "~" H 2800 6500 50  0001 C CNN
	1    2800 6500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 6648526E
P 2800 6200
F 0 "R2" V 2593 6200 50  0001 C CNN
F 1 "220" V 2800 6200 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 2730 6200 50  0001 C CNN
F 3 "~" H 2800 6200 50  0001 C CNN
	1    2800 6200
	-1   0    0    1   
$EndComp
Wire Wire Line
	2800 6650 2800 6750
Wire Wire Line
	2300 6650 2300 6750
Text Notes 950  900  0    100  ~ 20
rpi SGTL5000 interface board
Text Notes 1050 2100 0    50   ~ 0
The wire connector for the rPi control does not\nneed to preset itself as 5 pins in a row.  The pins\nmay be somewhat arbitrarily positioned on the\nPCB as a cable will be soldered into the pin holes.
Text Notes 3250 1650 0    50   ~ 10
has semi-standardized new display connector
Text Notes 3450 1750 0    50   ~ 0
developed during _prototype0 design phases
Text Notes 3450 1850 0    50   ~ 0
is known to likely map well to rPi pins
Text Notes 1800 7600 0    50   ~ 10
has onboard I2S activity LEDS
Text Notes 1950 7800 0    50   ~ 0
in the prototype stage, these only need to be\nvisible, and do not need to be off-board
Text Notes 950  1350 0    50   ~ 0
This PCB is intended to to be a shield on top of an rPi Zero 2 W \nthat has a power connector, accepts a SGTL5000 (teensy audio shield), \nand has a wired connector for rPi control, and a JST8 socket for a \nILI9341 or similar display device.\n
Text Notes 3250 2100 0    50   ~ 0
This will be a bare pin header soldered from the top, \nand epoxied on the bottom, without any JST socket
Text GLabel 3700 4000 1    50   Output ~ 10
SDA
Text GLabel 3600 4000 1    50   Output ~ 10
SCL
Text GLabel 3500 4000 1    50   Output ~ 10
I2S_MCLK
$Comp
L power:+5V #PWR015
U 1 1 6651B6F3
P 3700 5200
F 0 "#PWR015" H 3700 5050 50  0001 C CNN
F 1 "+5V" V 3700 5400 50  0000 C CNN
F 2 "" H 3700 5200 50  0001 C CNN
F 3 "" H 3700 5200 50  0001 C CNN
	1    3700 5200
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR016
U 1 1 665212AA
P 3800 5200
F 0 "#PWR016" H 3800 5050 50  0001 C CNN
F 1 "+5V" V 3800 5400 50  0000 C CNN
F 2 "" H 3800 5200 50  0001 C CNN
F 3 "" H 3800 5200 50  0001 C CNN
	1    3800 5200
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR09
U 1 1 6652166E
P 3800 4000
F 0 "#PWR09" H 3800 3850 50  0001 C CNN
F 1 "+3.3V" V 3800 4200 50  0000 C CNN
F 2 "" H 3800 4000 50  0001 C CNN
F 3 "" H 3800 4000 50  0001 C CNN
	1    3800 4000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR02
U 1 1 6653177E
P 3800 2600
F 0 "#PWR02" H 3800 2450 50  0001 C CNN
F 1 "+5V" V 3800 2800 50  0000 C CNN
F 2 "" H 3800 2600 50  0001 C CNN
F 3 "" H 3800 2600 50  0001 C CNN
	1    3800 2600
	-1   0    0    1   
$EndComp
Wire Wire Line
	2300 5200 2300 6000
Wire Wire Line
	2800 5200 2800 6000
Wire Wire Line
	2900 6000 2800 6000
Connection ~ 2800 6000
Wire Wire Line
	2800 6000 2800 6050
Wire Wire Line
	2150 6000 2300 6000
Connection ~ 2300 6000
Wire Wire Line
	2300 6000 2300 6050
Wire Wire Line
	2300 6750 2550 6750
$Comp
L power:GND #PWR018
U 1 1 66562CCD
P 2550 6850
F 0 "#PWR018" H 2550 6600 50  0001 C CNN
F 1 "GND" V 2550 6650 50  0000 C CNN
F 2 "" H 2550 6850 50  0001 C CNN
F 3 "" H 2550 6850 50  0001 C CNN
	1    2550 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 6850 2550 6750
Connection ~ 2550 6750
Wire Wire Line
	2550 6750 2800 6750
Text Notes 950  1650 0    50   ~ 10
arbirarily positioned rPi control wired connector
$Comp
L Connector_Generic:Conn_01x08 J1
U 1 1 665749E8
P 4200 2400
F 0 "J1" V 4164 2780 50  0001 L CNN
F 1 "display" V 4350 2250 50  0000 L CNN
F 2 "0_my_footprints:myJSTx08" H 4200 2400 50  0001 C CNN
F 3 "~" H 4200 2400 50  0001 C CNN
	1    4200 2400
	0    1    -1   0   
$EndComp
$Comp
L 0_my_teensy:teensyAudioRevD U1
U 1 1 664E10BD
P 5900 4600
F 0 "U1" H 6778 4628 60  0000 L CNN
F 1 "teensyAudioRevD" H 6778 4522 60  0000 L CNN
F 2 "0_my_teensy:audioShieldRevD" V 6850 4550 60  0001 C CNN
F 3 "" V 6850 4550 60  0000 C CNN
	1    5900 4600
	1    0    0    -1  
$EndComp
Text GLabel 5950 3900 1    50   Input ~ 0
I2S_FCLK
Text GLabel 6150 3900 1    50   Input ~ 0
SDA
Text GLabel 6050 3900 1    50   Input ~ 0
SCL
Text GLabel 5650 3900 1    50   Input ~ 0
I2S_MCLK
Text GLabel 6150 5350 3    50   Input ~ 0
I2S_RX
Text GLabel 6250 5350 3    50   Input ~ 0
I2S_TX
Text GLabel 5850 3900 1    50   Input ~ 0
I2S_BCLK
$Comp
L power:GND #PWR04
U 1 1 66504730
P 5450 3900
F 0 "#PWR04" H 5450 3650 50  0001 C CNN
F 1 "GND" V 5450 3700 50  0000 C CNN
F 2 "" H 5450 3900 50  0001 C CNN
F 3 "" H 5450 3900 50  0001 C CNN
	1    5450 3900
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR017
U 1 1 66504B0C
P 5350 5400
F 0 "#PWR017" H 5350 5150 50  0001 C CNN
F 1 "GND" V 5350 5200 50  0000 C CNN
F 2 "" H 5350 5400 50  0001 C CNN
F 3 "" H 5350 5400 50  0001 C CNN
	1    5350 5400
	-1   0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR05
U 1 1 66506389
P 5550 3900
F 0 "#PWR05" H 5550 3750 50  0001 C CNN
F 1 "+3.3V" V 5550 4100 50  0000 C CNN
F 2 "" H 5550 3900 50  0001 C CNN
F 3 "" H 5550 3900 50  0001 C CNN
	1    5550 3900
	1    0    0    -1  
$EndComp
$EndSCHEMATC
