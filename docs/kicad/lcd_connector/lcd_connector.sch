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
L Device:R R2
U 1 1 6149A29E
P 4300 4150
F 0 "R2" V 4200 4150 50  0000 C CNN
F 1 "100" V 4300 4150 50  0000 C CNN
F 2 "cnc3018-PCB:myResistor" V 4230 4150 50  0001 C CNN
F 3 "~" H 4300 4150 50  0001 C CNN
	1    4300 4150
	0    1    1    0   
$EndComp
NoConn ~ 3000 4150
$Comp
L cnc3018_Library:TFT_ILI9341 M1
U 1 1 6147A123
P 1950 3600
F 0 "M1" H 1417 2635 50  0000 C CNN
F 1 "TFT_ILI9341" H 1417 2726 50  0000 C CNN
F 2 "cnc3018-PCB:my14Pin" H 1450 3350 50  0001 C CNN
F 3 "" H 1450 3350 50  0001 C CNN
	1    1950 3600
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J1
U 1 1 6147C8F0
P 4350 2850
F 0 "J1" H 4550 2850 50  0000 L CNN
F 1 "5V" H 4700 2850 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4350 2850 50  0001 C CNN
F 3 "~" H 4350 2850 50  0001 C CNN
	1    4350 2850
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 6147DA28
P 4350 2950
F 0 "J2" H 4550 2950 50  0000 L CNN
F 1 "GND" H 4700 2950 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4350 2950 50  0001 C CNN
F 3 "~" H 4350 2950 50  0001 C CNN
	1    4350 2950
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J3
U 1 1 6147E475
P 4350 3050
F 0 "J3" H 4550 3050 50  0000 L CNN
F 1 "LCD_CS" H 4700 3050 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4350 3050 50  0001 C CNN
F 3 "~" H 4350 3050 50  0001 C CNN
	1    4350 3050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J4
U 1 1 6147F1E3
P 4350 3150
F 0 "J4" H 4550 3150 50  0000 L CNN
F 1 "DC" H 4700 3150 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4350 3150 50  0001 C CNN
F 3 "~" H 4350 3150 50  0001 C CNN
	1    4350 3150
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J5
U 1 1 6147FDC6
P 4350 3250
F 0 "J5" H 4550 3250 50  0000 L CNN
F 1 "MOSI" H 4700 3250 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4350 3250 50  0001 C CNN
F 3 "~" H 4350 3250 50  0001 C CNN
	1    4350 3250
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J6
U 1 1 61480747
P 4350 3350
F 0 "J6" H 4550 3350 50  0000 L CNN
F 1 "SCK" H 4700 3350 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4350 3350 50  0001 C CNN
F 3 "~" H 4350 3350 50  0001 C CNN
	1    4350 3350
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J7
U 1 1 6148260B
P 4350 3450
F 0 "J7" H 4550 3450 50  0000 L CNN
F 1 "MOSI" H 4700 3450 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4350 3450 50  0001 C CNN
F 3 "~" H 4350 3450 50  0001 C CNN
	1    4350 3450
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J8
U 1 1 6148308A
P 4350 3550
F 0 "J8" H 4550 3550 50  0000 L CNN
F 1 "MISO" H 4700 3550 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4350 3550 50  0001 C CNN
F 3 "~" H 4350 3550 50  0001 C CNN
	1    4350 3550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 66264EAA
P 3000 2850
F 0 "#PWR01" H 3000 2700 50  0001 C CNN
F 1 "+5V" V 3000 3000 50  0000 L CNN
F 2 "" H 3000 2850 50  0001 C CNN
F 3 "" H 3000 2850 50  0001 C CNN
	1    3000 2850
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 66265B7F
P 3000 2950
F 0 "#PWR03" H 3000 2700 50  0001 C CNN
F 1 "GND" V 3000 2800 50  0000 R CNN
F 2 "" H 3000 2950 50  0001 C CNN
F 3 "" H 3000 2950 50  0001 C CNN
	1    3000 2950
	0    -1   -1   0   
$EndComp
Text GLabel 3000 3050 2    50   Input ~ 0
LCD_CS
Text GLabel 3000 3150 2    50   Input ~ 0
RESET
Text GLabel 3000 3250 2    50   Input ~ 0
DC
Text GLabel 3000 3350 2    50   Input ~ 0
MOSI
Text GLabel 3000 3450 2    50   Input ~ 0
SCK
Text GLabel 3000 3550 2    50   Input ~ 0
LED
Text GLabel 3000 3650 2    50   Input ~ 0
MISO
Text GLabel 3000 3750 2    50   Input ~ 0
SCK
Text GLabel 3000 3850 2    50   Input ~ 0
TOUCH_CS
Text GLabel 3000 3950 2    50   Input ~ 0
MOSI
Text GLabel 3000 4050 2    50   Input ~ 0
MISO
$Comp
L power:+5V #PWR02
U 1 1 66276BB4
P 4150 2850
F 0 "#PWR02" H 4150 2700 50  0001 C CNN
F 1 "+5V" V 4150 3000 50  0000 L CNN
F 2 "" H 4150 2850 50  0001 C CNN
F 3 "" H 4150 2850 50  0001 C CNN
	1    4150 2850
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 66276BBA
P 4150 2950
F 0 "#PWR04" H 4150 2700 50  0001 C CNN
F 1 "GND" V 4150 2800 50  0000 R CNN
F 2 "" H 4150 2950 50  0001 C CNN
F 3 "" H 4150 2950 50  0001 C CNN
	1    4150 2950
	0    1    -1   0   
$EndComp
Text GLabel 4150 3050 0    50   Input ~ 0
LCD_CS
Text GLabel 4150 3150 0    50   Input ~ 0
DC
Text GLabel 4150 3250 0    50   Input ~ 0
MOSI
Text GLabel 4150 3350 0    50   Input ~ 0
SCK
Text GLabel 4150 3550 0    50   Input ~ 0
MISO
Text GLabel 4150 3450 0    50   Input ~ 0
TOUCH_CS
Text GLabel 4150 4150 0    50   Input ~ 0
LED
$Comp
L power:+5V #PWR06
U 1 1 66290720
P 4450 4150
F 0 "#PWR06" H 4450 4000 50  0001 C CNN
F 1 "+5V" V 4450 4300 50  0000 L CNN
F 2 "" H 4450 4150 50  0001 C CNN
F 3 "" H 4450 4150 50  0001 C CNN
	1    4450 4150
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 6148DFD0
P 4300 3900
F 0 "R1" V 4200 3900 50  0000 C CNN
F 1 "10K" V 4300 3900 50  0000 C CNN
F 2 "cnc3018-PCB:myResistor" V 4230 3900 50  0001 C CNN
F 3 "~" H 4300 3900 50  0001 C CNN
	1    4300 3900
	0    1    1    0   
$EndComp
Text GLabel 4150 3900 0    50   Input ~ 0
RESET
$Comp
L power:+5V #PWR05
U 1 1 662981CB
P 4450 3900
F 0 "#PWR05" H 4450 3750 50  0001 C CNN
F 1 "+5V" V 4450 4050 50  0000 L CNN
F 2 "" H 4450 3900 50  0001 C CNN
F 3 "" H 4450 3900 50  0001 C CNN
	1    4450 3900
	0    1    1    0   
$EndComp
$EndSCHEMATC
