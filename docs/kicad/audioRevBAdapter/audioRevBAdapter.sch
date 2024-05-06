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
L 0_my_teensy:teensyAudioRevB U2
U 1 1 663C0C59
P 2900 2950
F 0 "U2" H 3778 2978 60  0001 L CNN
F 1 "teensyAudioRevB" H 3350 2900 60  0000 R CNN
F 2 "0_my_teensy:audioShieldRevB" V 3850 2900 60  0001 C CNN
F 3 "" V 3850 2900 60  0000 C CNN
	1    2900 2950
	1    0    0    -1  
$EndComp
Text GLabel 3550 4000 3    50   Input ~ 0
MCLK1
Text GLabel 3350 3700 3    50   Input ~ 0
BCLK1
Text GLabel 2650 2250 1    50   Input ~ 0
LRCLK1
Text GLabel 3050 2250 1    50   Input ~ 0
I2C_SCL
Text GLabel 3150 2250 1    50   Input ~ 0
I2C_SCA
Text GLabel 3550 2250 1    50   Input ~ 0
I2C_SCLK
Text GLabel 3650 2250 1    50   Input ~ 0
I2S_RX1A
Text GLabel 2750 2250 1    50   Input ~ 0
I2S_TX1A
$Comp
L Device:R R1
U 1 1 6652E99E
P 3550 3850
F 0 "R1" H 3500 4500 50  0001 L CNN
F 1 "100" V 3550 3850 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 3480 3850 50  0001 C CNN
F 3 "~" H 3550 3850 50  0001 C CNN
	1    3550 3850
	-1   0    0    1   
$EndComp
Text Notes 2150 900  0    79   ~ 16
teensy Audio Shield Rev B
Text Notes 2000 1150 0    50   ~ 0
This PCB is only needed until I get the RevD audio shields,\nat which time it can be stacked with the 4.1
$Comp
L power:GND #PWR0101
U 1 1 6634081F
P 2350 3700
F 0 "#PWR0101" H 2350 3450 50  0001 C CNN
F 1 "GND" V 2350 3500 50  0000 C CNN
F 2 "" H 2350 3700 50  0001 C CNN
F 3 "" H 2350 3700 50  0001 C CNN
	1    2350 3700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 66341ED9
P 2450 2250
F 0 "#PWR0102" H 2450 2000 50  0001 C CNN
F 1 "GND" V 2450 2050 50  0000 C CNN
F 2 "" H 2450 2250 50  0001 C CNN
F 3 "" H 2450 2250 50  0001 C CNN
	1    2450 2250
	-1   0    0    1   
$EndComp
Text GLabel 2550 2250 1    50   Input ~ 0
T41_3.3V
$Comp
L Connector_Generic:Conn_01x14 J2
U 1 1 6634BEBF
P 5100 3450
F 0 "J2" H 5180 3442 50  0001 L CNN
F 1 "trensy41_0-14" V 5300 3200 50  0000 L CNN
F 2 "0_my_footprints:myPinSocket_1x14" H 5100 3450 50  0001 C CNN
F 3 "~" H 5100 3450 50  0001 C CNN
	1    5100 3450
	0    -1   -1   0   
$EndComp
Text GLabel 5800 3650 3    50   Input ~ 0
T3_MISO
Text GLabel 5700 3650 3    50   Input ~ 0
T3_MOSI
$Comp
L power:GND #PWR02
U 1 1 66358873
P 4500 3650
F 0 "#PWR02" H 4500 3400 50  0001 C CNN
F 1 "GND" V 4500 3450 50  0000 C CNN
F 2 "" H 4500 3650 50  0001 C CNN
F 3 "" H 4500 3650 50  0001 C CNN
	1    4500 3650
	1    0    0    -1  
$EndComp
Text GLabel 5300 3650 3    50   Input ~ 0
I2S_TX1A
Text GLabel 5400 3650 3    50   Input ~ 0
I2S_RX1A
Text GLabel 5200 3650 3    50   Input ~ 0
I2S_RX1B
Text GLabel 4700 3650 3    50   Input ~ 0
T41_TX
Text GLabel 4600 3650 3    50   Input ~ 0
T41_RX
Text GLabel 5600 3650 3    50   Input ~ 0
T3_LED_CS
Text GLabel 5500 3650 3    50   Input ~ 0
T3_CD
Text GLabel 4500 2500 1    50   Input ~ 0
T41_VIN
Text GLabel 5800 2500 1    50   Input ~ 0
T3_SCLK
Text GLabel 4700 2500 1    50   Input ~ 0
T41_3.3V
Text GLabel 5600 2500 1    50   Input ~ 0
I2C_SCLK
Text GLabel 5300 2500 1    50   Input ~ 0
I2C_SCA
Text GLabel 5200 2500 1    50   Input ~ 0
I2C_SCL
Text GLabel 5100 2500 1    50   Input ~ 0
LRCLK1
Text GLabel 5000 2500 1    50   Input ~ 0
BCLK1
Text GLabel 4800 2500 1    50   Input ~ 0
MCLK1
$Comp
L power:GND #PWR01
U 1 1 6635CCA7
P 4600 2500
F 0 "#PWR01" H 4600 2250 50  0001 C CNN
F 1 "GND" V 4600 2300 50  0000 C CNN
F 2 "" H 4600 2500 50  0001 C CNN
F 3 "" H 4600 2500 50  0001 C CNN
	1    4600 2500
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x14 J1
U 1 1 66354B03
P 5200 2700
F 0 "J1" H 5280 2692 50  0001 L CNN
F 1 "trensy41_15-40" V 5400 2450 50  0000 L CNN
F 2 "0_my_footprints:myPinSocket_1x14" H 5200 2700 50  0001 C CNN
F 3 "~" H 5200 2700 50  0001 C CNN
	1    5200 2700
	0    1    1    0   
$EndComp
$EndSCHEMATC
