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
L Connector_Generic:Conn_01x03 J4
U 1 1 6615D6DC
P 4950 4150
F 0 "J4" V 3950 4200 50  0000 R CNN
F 1 "PI_2" V 3850 4200 50  0000 R CNN
F 2 "0_my_footprints:invPinSocket_1x03" H 4950 4150 50  0001 C CNN
F 3 "~" H 4950 4150 50  0001 C CNN
	1    4950 4150
	0    1    -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J3
U 1 1 66163A35
P 5550 4050
F 0 "J3" V 5050 4400 50  0000 R CNN
F 1 "PI_3" V 5150 4450 50  0000 R CNN
F 2 "0_my_footprints:invPinSocket_1x03" H 5550 4050 50  0001 C CNN
F 3 "~" H 5550 4050 50  0001 C CNN
	1    5550 4050
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR07
U 1 1 66165D1C
P 6350 4350
F 0 "#PWR07" H 6350 4100 50  0001 C CNN
F 1 "GND" V 6350 4150 50  0000 C CNN
F 2 "" H 6350 4350 50  0001 C CNN
F 3 "" H 6350 4350 50  0001 C CNN
	1    6350 4350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR08
U 1 1 66166E40
P 6450 4350
F 0 "#PWR08" H 6450 4200 50  0001 C CNN
F 1 "+5V" V 6450 4550 50  0000 C CNN
F 2 "" H 6450 4350 50  0001 C CNN
F 3 "" H 6450 4350 50  0001 C CNN
	1    6450 4350
	-1   0    0    1   
$EndComp
Text GLabel 6250 4350 3    50   Input ~ 0
RPI_TX0
Text GLabel 6150 4350 3    50   Input ~ 0
RPI_RX0
Text GLabel 5750 4350 3    50   Input ~ 0
DISPLAY_CD
Text GLabel 5450 4350 3    50   Input ~ 0
DISPLAY_CS
Text GLabel 5050 4350 3    50   Input ~ 0
AUDIO_RX_ACTIVE
Text GLabel 4850 4350 3    50   Input ~ 0
AUDIO_TX_ACTIVE
Text GLabel 5450 3850 1    50   Input ~ 0
DISPLAY_SCLK
Text GLabel 5550 3850 1    50   Input ~ 0
DISPLAY_MISO
Text GLabel 5650 3850 1    50   Input ~ 0
DISPLAY_MOSI
NoConn ~ 6050 4150
NoConn ~ 5850 4150
NoConn ~ 6550 4050
NoConn ~ 6450 4050
NoConn ~ 6350 4050
NoConn ~ 6250 4050
NoConn ~ 6150 4050
NoConn ~ 6050 4050
NoConn ~ 5850 4050
NoConn ~ 5750 4050
NoConn ~ 5150 4150
NoConn ~ 4750 4150
NoConn ~ 4650 4150
NoConn ~ 5950 4050
NoConn ~ 5950 4150
NoConn ~ 5250 4150
NoConn ~ 5350 4050
NoConn ~ 5250 4050
NoConn ~ 5150 4050
NoConn ~ 5050 4050
NoConn ~ 4950 4050
NoConn ~ 4850 4050
NoConn ~ 4750 4050
NoConn ~ 4650 4050
$Comp
L Connector_Generic:Conn_01x01 J7
U 1 1 662BD669
P 4250 3750
F 0 "J7" H 4330 3792 50  0000 L CNN
F 1 "PI_RUN" H 4330 3701 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4250 3750 50  0001 C CNN
F 3 "~" H 4250 3750 50  0001 C CNN
	1    4250 3750
	1    0    0    -1  
$EndComp
Text GLabel 4050 3750 0    50   Input ~ 0
RPI_RUN
$Comp
L Connector_Generic:Conn_01x08 J2
U 1 1 661AFDC8
P 6100 2050
F 0 "J2" V 6250 1750 50  0000 L CNN
F 1 "Display" V 6250 1950 50  0000 L CNN
F 2 "0_my_footprints:myJSTx08" H 6100 2050 50  0001 C CNN
F 3 "~" H 6100 2050 50  0001 C CNN
	1    6100 2050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 661AFDCE
P 6400 2250
F 0 "#PWR03" H 6400 2000 50  0001 C CNN
F 1 "GND" V 6400 2050 50  0000 C CNN
F 2 "" H 6400 2250 50  0001 C CNN
F 3 "" H 6400 2250 50  0001 C CNN
	1    6400 2250
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR04
U 1 1 661AFDD4
P 6500 2250
F 0 "#PWR04" H 6500 2100 50  0001 C CNN
F 1 "+5V" V 6500 2450 50  0000 C CNN
F 2 "" H 6500 2250 50  0001 C CNN
F 3 "" H 6500 2250 50  0001 C CNN
	1    6500 2250
	-1   0    0    1   
$EndComp
Text GLabel 6300 2250 3    50   Input ~ 0
DISPLAY_CS
Text GLabel 6200 2250 3    50   Input ~ 0
DISPLAY_CD
Text GLabel 5900 2250 3    50   Input ~ 0
DISPLAY_MOSI
Text GLabel 6100 2250 3    50   Input ~ 0
DISPLAY_SCLK
Text GLabel 6000 2250 3    50   Input ~ 0
DISPLAY_MISO
Text GLabel 5800 2250 3    50   Input ~ 0
TOUCH_CS
$Comp
L power:GND #PWR05
U 1 1 661AF57F
P 4950 4350
F 0 "#PWR05" H 4950 4100 50  0001 C CNN
F 1 "GND" V 4950 4150 50  0000 C CNN
F 2 "" H 4950 4350 50  0001 C CNN
F 3 "" H 4950 4350 50  0001 C CNN
	1    4950 4350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 661B2BC3
P 5650 4350
F 0 "#PWR06" H 5650 4100 50  0001 C CNN
F 1 "GND" V 5650 4150 50  0000 C CNN
F 2 "" H 5650 4350 50  0001 C CNN
F 3 "" H 5650 4350 50  0001 C CNN
	1    5650 4350
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J5
U 1 1 661BB80B
P 5550 4150
F 0 "J5" V 4550 4200 50  0000 L CNN
F 1 "PI_1" V 4450 4150 50  0000 L CNN
F 2 "0_my_footprints:invPinSocket_1x5" H 5550 4150 50  0001 C CNN
F 3 "~" H 5550 4150 50  0001 C CNN
	1    5550 4150
	0    1    -1   0   
$EndComp
Text GLabel 5350 2250 3    50   Input ~ 0
RPI_RUN
$Comp
L power:+5V #PWR01
U 1 1 662CCE56
P 4650 2250
F 0 "#PWR01" H 4650 2100 50  0001 C CNN
F 1 "+5V" V 4650 2450 50  0000 C CNN
F 2 "" H 4650 2250 50  0001 C CNN
F 3 "" H 4650 2250 50  0001 C CNN
	1    4650 2250
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR02
U 1 1 662CCE50
P 4750 2250
F 0 "#PWR02" H 4750 2000 50  0001 C CNN
F 1 "GND" V 4750 2050 50  0000 C CNN
F 2 "" H 4750 2250 50  0001 C CNN
F 3 "" H 4750 2250 50  0001 C CNN
	1    4750 2250
	-1   0    0    -1  
$EndComp
Text GLabel 5250 2250 3    50   Input ~ 0
AUDIO_TX_ACTIVE
Text GLabel 5150 2250 3    50   Input ~ 0
AUDIO_RX_ACTIVE
Text GLabel 5050 2250 3    50   Input ~ 0
RPI_READY
Text GLabel 4950 2250 3    50   Input ~ 0
RPI_RX0
Text GLabel 4850 2250 3    50   Input ~ 0
RPI_TX0
$Comp
L Connector_Generic:Conn_01x08 J1
U 1 1 662C3EE5
P 4950 2050
F 0 "J1" V 5100 1750 50  0000 L CNN
F 1 "Teensy3.2" V 5100 1950 50  0000 L CNN
F 2 "0_my_footprints:myJSTx08" H 4950 2050 50  0001 C CNN
F 3 "~" H 4950 2050 50  0001 C CNN
	1    4950 2050
	0    -1   -1   0   
$EndComp
Text Notes 5600 1750 0    50   ~ 0
modified moved CS, CD, and MOSI
Text GLabel 5550 4350 3    50   Input ~ 0
RPI_READY
Text GLabel 5350 4350 3    50   Input ~ 0
TOUCH_CS
$Comp
L Connector_Generic:Conn_01x05 J?
U 1 1 66297EC0
P 6350 4150
F 0 "J?" V 5350 4200 50  0000 L CNN
F 1 "PI_0" V 5250 4150 50  0000 L CNN
F 2 "0_my_footprints:invPinSocket_1x5" H 6350 4150 50  0001 C CNN
F 3 "~" H 6350 4150 50  0001 C CNN
	1    6350 4150
	0    1    -1   0   
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 6629B0FE
P 6550 4350
F 0 "#PWR?" H 6550 4200 50  0001 C CNN
F 1 "+5V" V 6550 4550 50  0000 C CNN
F 2 "" H 6550 4350 50  0001 C CNN
F 3 "" H 6550 4350 50  0001 C CNN
	1    6550 4350
	-1   0    0    1   
$EndComp
$EndSCHEMATC
