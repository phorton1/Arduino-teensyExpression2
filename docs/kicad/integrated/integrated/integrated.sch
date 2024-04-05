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
L 0_my_teensy:myTeensy3.6 U1
U 1 1 64C58045
P 3850 4750
F 0 "U1" H 4400 5300 50  0000 L CNN
F 1 "myTeensy3.6" H 4200 5200 50  0000 L CNN
F 2 "0_my_teensy:myTeensy3.6" H 3850 4750 50  0001 C CNN
F 3 "" H 3850 4750 50  0001 C CNN
	1    3850 4750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J5
U 1 1 64C5C08D
P 2700 6500
F 0 "J5" V 2950 6450 50  0000 L CNN
F 1 "LEDS" V 2850 6400 50  0000 L CNN
F 2 "0_my_footprints:myPinSocket_1x03" H 2700 6500 50  0001 C CNN
F 3 "~" H 2700 6500 50  0001 C CNN
	1    2700 6500
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J6
U 1 1 64C5D02E
P 3400 6500
F 0 "J6" V 3650 6450 50  0000 L CNN
F 1 "ROTARY12" V 3550 6300 50  0000 L CNN
F 2 "0_my_footprints:myPinSocket_1x05" H 3400 6500 50  0001 C CNN
F 3 "~" H 3400 6500 50  0001 C CNN
	1    3400 6500
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J2
U 1 1 64C61392
P 3650 2050
F 0 "J2" V 3900 2050 50  0000 R CNN
F 1 "TFT_CONTROL" V 3800 2250 50  0000 R CNN
F 2 "0_my_footprints:myJSTx08" H 3650 2050 50  0001 C CNN
F 3 "~" H 3650 2050 50  0001 C CNN
	1    3650 2050
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J1
U 1 1 64C62D8F
P 2650 2050
F 0 "J1" V 2900 2050 50  0000 R CNN
F 1 "EXPR" V 2800 2100 50  0000 R CNN
F 2 "0_my_footprints:myPinSocket_1x06" H 2650 2050 50  0001 C CNN
F 3 "~" H 2650 2050 50  0001 C CNN
	1    2650 2050
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J7
U 1 1 64C75495
P 4000 6500
F 0 "J7" V 4250 6450 50  0000 L CNN
F 1 "ROTARY34" V 4150 6300 50  0000 L CNN
F 2 "0_my_footprints:myPinSocket_1x05" H 4000 6500 50  0001 C CNN
F 3 "~" H 4000 6500 50  0001 C CNN
	1    4000 6500
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J8
U 1 1 64C7A741
P 4650 6500
F 0 "J8" V 4900 6450 50  0000 L CNN
F 1 "SW_OUT" V 4800 6400 50  0000 L CNN
F 2 "0_my_footprints:myPinSocket_1x05" H 4650 6500 50  0001 C CNN
F 3 "~" H 4650 6500 50  0001 C CNN
	1    4650 6500
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J9
U 1 1 64C7D5D7
P 5150 6500
F 0 "J9" V 5400 6450 50  0000 L CNN
F 1 "SW_IN" V 5300 6400 50  0000 L CNN
F 2 "0_my_footprints:myPinSocket_1x05" H 5150 6500 50  0001 C CNN
F 3 "~" H 5150 6500 50  0001 C CNN
	1    5150 6500
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J3
U 1 1 64C8119F
P 4600 2050
F 0 "J3" V 4850 2050 50  0000 R CNN
F 1 "TFT_DATA" V 4750 2250 50  0000 R CNN
F 2 "0_my_footprints:myJSTx08" H 4600 2050 50  0001 C CNN
F 3 "~" H 4600 2050 50  0001 C CNN
	1    4600 2050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR013
U 1 1 64C98700
P 2800 6300
F 0 "#PWR013" H 2800 6050 50  0001 C CNN
F 1 "GND" V 2800 6100 50  0000 C CNN
F 2 "" H 2800 6300 50  0001 C CNN
F 3 "" H 2800 6300 50  0001 C CNN
	1    2800 6300
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR014
U 1 1 64C9DA52
P 2600 6300
F 0 "#PWR014" H 2600 6150 50  0001 C CNN
F 1 "+5V" V 2600 6500 50  0000 C CNN
F 2 "" H 2600 6300 50  0001 C CNN
F 3 "" H 2600 6300 50  0001 C CNN
	1    2600 6300
	1    0    0    -1  
$EndComp
Text GLabel 2700 6300 1    50   Input ~ 0
LEDS
$Comp
L power:+3V3 #PWR016
U 1 1 64C9FDB5
P 3800 6300
F 0 "#PWR016" H 3800 6150 50  0001 C CNN
F 1 "+3V3" V 3800 6500 50  0000 C CNN
F 2 "" H 3800 6300 50  0001 C CNN
F 3 "" H 3800 6300 50  0001 C CNN
	1    3800 6300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 64CA12B5
P 3200 6300
F 0 "#PWR015" H 3200 6050 50  0001 C CNN
F 1 "GND" V 3200 6100 50  0000 C CNN
F 2 "" H 3200 6300 50  0001 C CNN
F 3 "" H 3200 6300 50  0001 C CNN
	1    3200 6300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR011
U 1 1 64CA2492
P 3050 5000
F 0 "#PWR011" H 3050 4750 50  0001 C CNN
F 1 "GND" V 3050 4800 50  0000 C CNN
F 2 "" H 3050 5000 50  0001 C CNN
F 3 "" H 3050 5000 50  0001 C CNN
	1    3050 5000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 64CA2F90
P 3150 3450
F 0 "#PWR08" H 3150 3200 50  0001 C CNN
F 1 "GND" V 3150 3250 50  0000 C CNN
F 2 "" H 3150 3450 50  0001 C CNN
F 3 "" H 3150 3450 50  0001 C CNN
	1    3150 3450
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR07
U 1 1 64CA4C9F
P 3050 3450
F 0 "#PWR07" H 3050 3300 50  0001 C CNN
F 1 "+5V" V 3050 3650 50  0000 C CNN
F 2 "" H 3050 3450 50  0001 C CNN
F 3 "" H 3050 3450 50  0001 C CNN
	1    3050 3450
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR012
U 1 1 64CA8793
P 4450 5000
F 0 "#PWR012" H 4450 4850 50  0001 C CNN
F 1 "+3V3" V 4450 5200 50  0000 C CNN
F 2 "" H 4450 5000 50  0001 C CNN
F 3 "" H 4450 5000 50  0001 C CNN
	1    4450 5000
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR010
U 1 1 64CADA6D
P 4450 3450
F 0 "#PWR010" H 4450 3200 50  0001 C CNN
F 1 "GND" V 4450 3250 50  0000 C CNN
F 2 "" H 4450 3450 50  0001 C CNN
F 3 "" H 4450 3450 50  0001 C CNN
	1    4450 3450
	-1   0    0    1   
$EndComp
Text GLabel 3350 5000 3    50   Input ~ 0
ROT1A
Text GLabel 3450 5000 3    50   Input ~ 0
ROT1B
Text GLabel 3550 5000 3    50   Input ~ 0
ROT2A
Text GLabel 3850 5000 3    50   Input ~ 0
T36-RX3
Text GLabel 3950 5000 3    50   Input ~ 0
T36-TX3
Text GLabel 4050 5000 3    50   Input ~ 0
ROT3A
Text GLabel 4150 5000 3    50   Input ~ 0
ROT3B
Text GLabel 4350 5000 3    50   Input ~ 0
ROT4B
Text GLabel 4250 5000 3    50   Input ~ 0
ROT4A
Text GLabel 3600 6300 1    50   Input ~ 0
ROT1A
Text GLabel 3500 6300 1    50   Input ~ 0
ROT1B
Text GLabel 3300 6300 1    50   Input ~ 0
ROT2B
Text GLabel 3400 6300 1    50   Input ~ 0
ROT2A
Text GLabel 4200 6300 1    50   Input ~ 0
ROT3A
Text GLabel 4100 6300 1    50   Input ~ 0
ROT3B
Text GLabel 3900 6300 1    50   Input ~ 0
ROT4B
Text GLabel 4000 6300 1    50   Input ~ 0
ROT4A
Text GLabel 4550 5000 3    50   Input ~ 0
SW_OUT0
Text GLabel 4650 5000 3    50   Input ~ 0
SW_OUT1
Text GLabel 4750 5000 3    50   Input ~ 0
SW_OUT2
Text GLabel 4850 5000 3    50   Input ~ 0
SW_OUT3
Text GLabel 4950 5000 3    50   Input ~ 0
SW_OUT4
Text GLabel 5050 5000 3    50   Input ~ 0
SW_IN0
Text GLabel 5150 5000 3    50   Input ~ 0
SW_IN1
Text GLabel 5250 5000 3    50   Input ~ 0
SW_IN2
Text GLabel 5350 5000 3    50   Input ~ 0
SW_IN3
Text GLabel 5350 3450 1    50   Input ~ 0
SW_IN4
Text GLabel 4750 6300 1    50   Input ~ 0
SW_OUT1
Text GLabel 4650 6300 1    50   Input ~ 0
SW_OUT2
Text GLabel 4550 6300 1    50   Input ~ 0
SW_OUT3
Text GLabel 4450 6300 1    50   Input ~ 0
SW_OUT4
Text GLabel 4850 6300 1    50   Input ~ 0
SW_OUT0
Text GLabel 5250 6300 1    50   Input ~ 0
SW_IN1
Text GLabel 5150 6300 1    50   Input ~ 0
SW_IN2
Text GLabel 5050 6300 1    50   Input ~ 0
SW_IN3
Text GLabel 4950 6300 1    50   Input ~ 0
SW_IN4
Text GLabel 5350 6300 1    50   Input ~ 0
SW_IN0
$Comp
L power:GND #PWR01
U 1 1 64CFCC09
P 2950 2250
F 0 "#PWR01" H 2950 2000 50  0001 C CNN
F 1 "GND" V 2950 2050 50  0000 C CNN
F 2 "" H 2950 2250 50  0001 C CNN
F 3 "" H 2950 2250 50  0001 C CNN
	1    2950 2250
	-1   0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR09
U 1 1 64D070B3
P 3250 3450
F 0 "#PWR09" H 3250 3300 50  0001 C CNN
F 1 "+3V3" V 3250 3650 50  0000 C CNN
F 2 "" H 3250 3450 50  0001 C CNN
F 3 "" H 3250 3450 50  0001 C CNN
	1    3250 3450
	1    0    0    -1  
$EndComp
Text GLabel 2750 2250 3    50   Input ~ 0
EXPR1
Text GLabel 2650 2250 3    50   Input ~ 0
EXPR2
Text GLabel 2550 2250 3    50   Input ~ 0
EXPR3
Text GLabel 2450 2250 3    50   Input ~ 0
EXPR4
Text GLabel 3350 3450 1    50   Input ~ 0
EXPR1
Text GLabel 3450 3450 1    50   Input ~ 0
EXPR2
Text GLabel 3550 3450 1    50   Input ~ 0
EXPR3
Text GLabel 3650 3450 1    50   Input ~ 0
EXPR4
$Comp
L power:+5V #PWR02
U 1 1 64D14F5D
P 4050 2250
F 0 "#PWR02" H 4050 2100 50  0001 C CNN
F 1 "+5V" V 4050 2450 50  0000 C CNN
F 2 "" H 4050 2250 50  0001 C CNN
F 3 "" H 4050 2250 50  0001 C CNN
	1    4050 2250
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 64D19F15
P 3350 2250
F 0 "#PWR04" H 3350 2000 50  0001 C CNN
F 1 "GND" V 3350 2050 50  0000 C CNN
F 2 "" H 3350 2250 50  0001 C CNN
F 3 "" H 3350 2250 50  0001 C CNN
	1    3350 2250
	-1   0    0    -1  
$EndComp
Text GLabel 3850 2250 3    50   Input ~ 0
TFT_RES
Text GLabel 3750 2250 3    50   Input ~ 0
TFT_CS
Text GLabel 3650 2250 3    50   Input ~ 0
TFT_CD_RS
Text GLabel 3550 2250 3    50   Input ~ 0
TFT_WR
Text GLabel 3450 2250 3    50   Input ~ 0
TFT_RD
Text GLabel 4400 2250 3    50   Input ~ 0
TFT_DAT1
Text GLabel 4300 2250 3    50   Input ~ 0
TFT_DAT0
Text GLabel 5000 2250 3    50   Input ~ 0
TFT_DAT7
Text GLabel 4900 2250 3    50   Input ~ 0
TFT_DAT6
Text GLabel 4800 2250 3    50   Input ~ 0
TFT_DAT5
Text GLabel 4700 2250 3    50   Input ~ 0
TFT_DAT4
Text GLabel 4600 2250 3    50   Input ~ 0
TFT_DAT3
Text GLabel 4500 2250 3    50   Input ~ 0
TFT_DAT2
Text GLabel 3850 3450 1    50   Input ~ 0
TFT_RES
Text GLabel 3950 3450 1    50   Input ~ 0
TFT_CS
Text GLabel 4050 3450 1    50   Input ~ 0
TFT_CD_RS
Text GLabel 4150 3450 1    50   Input ~ 0
TFT_WR
Text GLabel 4250 3450 1    50   Input ~ 0
TFT_RD
Text GLabel 5150 3450 1    50   Input ~ 0
TFT_DAT1
Text GLabel 5250 3450 1    50   Input ~ 0
TFT_DAT0
Text GLabel 3750 3450 1    50   Input ~ 0
TFT_DAT7
Text GLabel 4350 3450 1    50   Input ~ 0
TFT_DAT6
Text GLabel 4750 3450 1    50   Input ~ 0
TFT_DAT5
Text GLabel 4850 3450 1    50   Input ~ 0
TFT_DAT4
Text GLabel 4950 3450 1    50   Input ~ 0
TFT_DAT3
Text GLabel 5050 3450 1    50   Input ~ 0
TFT_DAT2
NoConn ~ 4550 3450
NoConn ~ 4650 3450
$Comp
L power:+3V3 #PWR03
U 1 1 64D171C2
P 3950 2250
F 0 "#PWR03" H 3950 2100 50  0001 C CNN
F 1 "+3V3" V 3950 2450 50  0000 C CNN
F 2 "" H 3950 2250 50  0001 C CNN
F 3 "" H 3950 2250 50  0001 C CNN
	1    3950 2250
	1    0    0    1   
$EndComp
$Comp
L power:+3V3 #PWR06
U 1 1 64D07CE1
P 2850 2550
F 0 "#PWR06" H 2850 2400 50  0001 C CNN
F 1 "+3V3" V 2850 2750 50  0000 C CNN
F 2 "" H 2850 2550 50  0001 C CNN
F 3 "" H 2850 2550 50  0001 C CNN
	1    2850 2550
	1    0    0    1   
$EndComp
Text GLabel 1550 3950 1    50   Input ~ 0
EXPR1
Text GLabel 1650 3950 1    50   Input ~ 0
EXPR2
Text GLabel 1750 3950 1    50   Input ~ 0
EXPR3
Text GLabel 1850 3950 1    50   Input ~ 0
EXPR4
$Comp
L Device:R R2
U 1 1 64E4ADF9
P 1650 4100
F 0 "R2" V 1650 3550 50  0000 L CNN
F 1 "10K" V 1650 4100 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 1580 4100 50  0001 C CNN
F 3 "~" H 1650 4100 50  0001 C CNN
	1    1650 4100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 64E4B2A6
P 1750 4100
F 0 "R3" V 1750 3550 50  0000 L CNN
F 1 "10K" V 1750 4100 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 1680 4100 50  0001 C CNN
F 3 "~" H 1750 4100 50  0001 C CNN
	1    1750 4100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 64E4B7AB
P 1850 4100
F 0 "R4" V 1850 3550 50  0000 L CNN
F 1 "10K" V 1850 4100 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 1780 4100 50  0001 C CNN
F 3 "~" H 1850 4100 50  0001 C CNN
	1    1850 4100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 64CFCDE5
P 1550 4100
F 0 "R1" V 1550 3550 50  0000 L CNN
F 1 "10K" V 1550 4100 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 1480 4100 50  0001 C CNN
F 3 "~" H 1550 4100 50  0001 C CNN
	1    1550 4100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 64E50A7C
P 1550 4250
F 0 "#PWR0101" H 1550 4000 50  0001 C CNN
F 1 "GND" V 1550 4050 50  0000 C CNN
F 2 "" H 1550 4250 50  0001 C CNN
F 3 "" H 1550 4250 50  0001 C CNN
	1    1550 4250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 64E50B86
P 1650 4250
F 0 "#PWR0102" H 1650 4000 50  0001 C CNN
F 1 "GND" V 1650 4050 50  0000 C CNN
F 2 "" H 1650 4250 50  0001 C CNN
F 3 "" H 1650 4250 50  0001 C CNN
	1    1650 4250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 64E50E20
P 1750 4250
F 0 "#PWR0103" H 1750 4000 50  0001 C CNN
F 1 "GND" V 1750 4050 50  0000 C CNN
F 2 "" H 1750 4250 50  0001 C CNN
F 3 "" H 1750 4250 50  0001 C CNN
	1    1750 4250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 64E50F3A
P 1850 4250
F 0 "#PWR0104" H 1850 4000 50  0001 C CNN
F 1 "GND" V 1850 4050 50  0000 C CNN
F 2 "" H 1850 4250 50  0001 C CNN
F 3 "" H 1850 4250 50  0001 C CNN
	1    1850 4250
	1    0    0    -1  
$EndComp
Text GLabel 3750 5000 3    50   Input ~ 0
ROT2B
Text GLabel 3650 5000 3    50   Input ~ 0
LEDS
Text Notes 2600 3050 2    50   ~ 0
This 1K resistor is\nimportant so that\nthe 3V power is not \nshorted to ground when \ninserting pedal plugs!\n
$Comp
L Device:R R5
U 1 1 64E70350
P 2850 2400
F 0 "R5" V 2850 1800 50  0000 L CNN
F 1 "1K" V 2850 2400 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2780 2400 50  0001 C CNN
F 3 "~" H 2850 2400 50  0001 C CNN
	1    2850 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 64CCBE5B
P 9200 2850
F 0 "R?" H 9270 2896 50  0000 L CNN
F 1 "220" V 9200 2850 50  0000 C CNN
F 2 "" V 9130 2850 50  0001 C CNN
F 3 "~" H 9200 2850 50  0001 C CNN
	1    9200 2850
	0    1    1    0   
$EndComp
Text GLabel 6550 4300 0    50   Input ~ 0
TEENSY_PROG
Text GLabel 8800 2850 0    50   Input ~ 0
PI_RUN
Text GLabel 9350 2850 2    50   Input ~ 0
SENS_PI_RUN
$Comp
L Transistor_BJT:BC547 Q?
U 1 1 64CF1712
P 9000 2550
F 0 "Q?" H 9191 2596 50  0000 L CNN
F 1 "BC547" H 9191 2505 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 9200 2475 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC547.pdf" H 9000 2550 50  0001 L CNN
	1    9000 2550
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 64CF464E
P 8900 2250
F 0 "#PWR?" H 8900 2000 50  0001 C CNN
F 1 "GND" H 8905 2077 50  0000 C CNN
F 2 "" H 8900 2250 50  0001 C CNN
F 3 "" H 8900 2250 50  0001 C CNN
	1    8900 2250
	-1   0    0    1   
$EndComp
Text GLabel 9200 2550 2    50   Input ~ 0
PI_REBOOT
Wire Wire Line
	9050 2850 8900 2850
Wire Wire Line
	8900 2750 8900 2850
Connection ~ 8900 2850
Wire Wire Line
	8900 2850 8800 2850
Wire Wire Line
	8900 2350 8900 2250
Text GLabel 7050 3550 1    50   Input ~ 0
T36_RX3
Text GLabel 7150 3550 1    50   Input ~ 0
T36_TX3
Text GLabel 6850 5000 3    50   Input ~ 0
ONBOARD_LED
Text GLabel 7050 5000 3    50   Input ~ 0
LED_PI_RUN
Text GLabel 6950 5000 3    50   Input ~ 0
LED_PI_READY
Text GLabel 8150 5000 3    50   Input ~ 0
VUSB
Text GLabel 7500 1950 3    50   Input ~ 0
VUSB
Text GLabel 7700 1950 3    50   Input ~ 0
RX1
Text GLabel 7600 1950 3    50   Input ~ 0
TX1
Text GLabel 8050 3550 1    50   Input ~ 0
RX1
Text GLabel 7950 3550 1    50   Input ~ 0
TX1
$Comp
L power:GND #PWR?
U 1 1 64D08031
P 7800 1950
F 0 "#PWR?" H 7800 1700 50  0001 C CNN
F 1 "GND" V 7800 1750 50  0000 C CNN
F 2 "" H 7800 1950 50  0001 C CNN
F 3 "" H 7800 1950 50  0001 C CNN
	1    7800 1950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 64D0323E
P 8150 3550
F 0 "#PWR?" H 8150 3300 50  0001 C CNN
F 1 "GND" V 8150 3350 50  0000 C CNN
F 2 "" H 8150 3550 50  0001 C CNN
F 3 "" H 8150 3550 50  0001 C CNN
	1    8150 3550
	-1   0    0    1   
$EndComp
Text GLabel 6850 3550 1    50   Input ~ 0
PI_REBOOT
Text GLabel 6950 3550 1    50   Input ~ 0
SENS_PI_RUN
Text GLabel 7400 1950 3    50   Input ~ 0
PI_RUN
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 64CE18FB
P 7500 1750
F 0 "J?" V 7500 2300 50  0000 R CNN
F 1 "RPI" V 7650 1750 50  0000 R CNN
F 2 "" H 7500 1750 50  0001 C CNN
F 3 "~" H 7500 1750 50  0001 C CNN
	1    7500 1750
	0    1    -1   0   
$EndComp
$Comp
L 0_my_teensy:myTeensy3.2 U?
U 1 1 64CC40AE
P 7600 4300
F 0 "U?" H 7750 4300 60  0000 L CNN
F 1 "myTeensy3.2" H 7650 4450 60  0000 L CNN
F 2 "" H 7600 3550 60  0000 C CNN
F 3 "" H 7600 3550 60  0000 C CNN
	1    7600 4300
	-1   0    0    1   
$EndComp
Text GLabel 7900 6350 1    50   Input ~ 0
ONBOARD_LED
Text GLabel 7700 6350 1    50   Input ~ 0
LED_PI_RUN
Text GLabel 7600 6350 1    50   Input ~ 0
LED_PI_READY
Text GLabel 7500 6350 1    50   Input ~ 0
I2S_RX_LED
$Comp
L power:GND #PWR?
U 1 1 64D14989
P 7800 6350
F 0 "#PWR?" H 7800 6100 50  0001 C CNN
F 1 "GND" V 7800 6150 50  0000 C CNN
F 2 "" H 7800 6350 50  0001 C CNN
F 3 "" H 7800 6350 50  0001 C CNN
	1    7800 6350
	-1   0    0    1   
$EndComp
Text GLabel 7300 6350 1    50   Input ~ 0
TEENSY_PROG
Text GLabel 7200 6350 1    50   Input ~ 0
PI_RUN
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 64CCC22C
P 7500 6550
F 0 "J?" V 7500 5950 50  0000 R CNN
F 1 "LED_BOARD" V 7650 6650 50  0000 R CNN
F 2 "" H 7500 6550 50  0001 C CNN
F 3 "~" H 7500 6550 50  0001 C CNN
	1    7500 6550
	0    -1   1    0   
$EndComp
Text Notes 7700 1450 1    50   ~ 0
RPI TX PIN  8
Text Notes 7800 1450 1    50   ~ 0
RPI GND PIN  6
Text Notes 7600 1450 1    50   ~ 0
RPI RX PIN 10
Text Notes 7500 1450 1    50   ~ 0
RPI 5V PIN  2
Text Notes 7400 1450 1    50   ~ 0
RPI RUN PIN
Text Notes 7300 1450 1    50   ~ 0
RPI GPIO25 PIN 22
Text Notes 7200 1450 1    50   ~ 0
RPI GPIO12 PIN 32
Text Notes 7100 1450 1    50   ~ 0
RPI GPIO16 PIN 36
Text GLabel 7400 6350 1    50   Input ~ 0
I2S_TX_LED
Text GLabel 7100 1950 3    50   Input ~ 0
I2S_TX_LED
Text GLabel 7650 3550 1    50   Input ~ 0
SENS_PI_READY
Text GLabel 7300 1950 3    50   Input ~ 0
SENS_PI_READY
Text GLabel 7200 1950 3    50   Input ~ 0
I2S_RX_LED
$EndSCHEMATC
