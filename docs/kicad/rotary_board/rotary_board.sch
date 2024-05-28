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
L Device:Rotary_Encoder_Switch SW?
U 1 1 663E57E0
P 6500 1450
F 0 "SW?" H 6500 1817 50  0001 C CNN
F 1 "ROT1" H 6500 1725 50  0000 C CNN
F 2 "" H 6350 1610 50  0001 C CNN
F 3 "~" H 6500 1710 50  0001 C CNN
	1    6500 1450
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J?
U 1 1 663EE45D
P 2550 2000
F 0 "J?" H 2630 2042 50  0000 L CNN
F 1 "Conn_01x05" H 2630 1951 50  0000 L CNN
F 2 "" H 2550 2000 50  0001 C CNN
F 3 "~" H 2550 2000 50  0001 C CNN
	1    2550 2000
	-1   0    0    -1  
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW?
U 1 1 663F3D60
P 6500 2300
F 0 "SW?" H 6500 2667 50  0001 C CNN
F 1 "ROT2" H 6500 2575 50  0000 C CNN
F 2 "" H 6350 2460 50  0001 C CNN
F 3 "~" H 6500 2560 50  0001 C CNN
	1    6500 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW?
U 1 1 663F4490
P 6500 3150
F 0 "SW?" H 6500 3517 50  0001 C CNN
F 1 "ROT3" H 6500 3425 50  0000 C CNN
F 2 "" H 6350 3310 50  0001 C CNN
F 3 "~" H 6500 3410 50  0001 C CNN
	1    6500 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW?
U 1 1 663F4B46
P 6500 4050
F 0 "SW?" H 6500 4417 50  0001 C CNN
F 1 "ROT4" H 6500 4325 50  0000 C CNN
F 2 "" H 6350 4210 50  0001 C CNN
F 3 "~" H 6500 4310 50  0001 C CNN
	1    6500 4050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 663F4E53
P 2750 1800
F 0 "#PWR?" H 2750 1550 50  0001 C CNN
F 1 "GND" V 2750 1600 50  0000 C CNN
F 2 "" H 2750 1800 50  0001 C CNN
F 3 "" H 2750 1800 50  0001 C CNN
	1    2750 1800
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 663F68D2
P 4750 3100
F 0 "#PWR?" H 4750 2850 50  0001 C CNN
F 1 "GND" V 4750 2900 50  0000 C CNN
F 2 "" H 4750 3100 50  0001 C CNN
F 3 "" H 4750 3100 50  0001 C CNN
	1    4750 3100
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR?
U 1 1 663F6C65
P 2750 2200
F 0 "#PWR?" H 2750 2050 50  0001 C CNN
F 1 "+3.3V" V 2750 2450 50  0000 C CNN
F 2 "" H 2750 2200 50  0001 C CNN
F 3 "" H 2750 2200 50  0001 C CNN
	1    2750 2200
	0    1    1    0   
$EndComp
Text GLabel 6800 1350 2    50   Input ~ 0
SW1
Text GLabel 6800 2200 2    50   Input ~ 0
SW2
Text GLabel 6800 3050 2    50   Input ~ 0
SW3
Text GLabel 6800 3950 2    50   Input ~ 0
SW4
Text GLabel 6200 1350 0    50   Input ~ 0
ROT1A
Text GLabel 6200 1550 0    50   Input ~ 0
ROT1B
Text GLabel 6200 2200 0    50   Input ~ 0
ROT2A
Text GLabel 6200 2400 0    50   Input ~ 0
ROT2B
Text GLabel 6200 3050 0    50   Input ~ 0
ROT3A
Text GLabel 6200 3250 0    50   Input ~ 0
ROT3B
Text GLabel 6200 3950 0    50   Input ~ 0
ROT4A
Text GLabel 6200 4150 0    50   Input ~ 0
ROT4B
$Comp
L power:GND #PWR?
U 1 1 664272DF
P 6800 2400
F 0 "#PWR?" H 6800 2150 50  0001 C CNN
F 1 "GND" V 6800 2200 50  0000 C CNN
F 2 "" H 6800 2400 50  0001 C CNN
F 3 "" H 6800 2400 50  0001 C CNN
	1    6800 2400
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66427662
P 6800 3250
F 0 "#PWR?" H 6800 3000 50  0001 C CNN
F 1 "GND" V 6800 3050 50  0000 C CNN
F 2 "" H 6800 3250 50  0001 C CNN
F 3 "" H 6800 3250 50  0001 C CNN
	1    6800 3250
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66427948
P 6800 4150
F 0 "#PWR?" H 6800 3900 50  0001 C CNN
F 1 "GND" V 6800 3950 50  0000 C CNN
F 2 "" H 6800 4150 50  0001 C CNN
F 3 "" H 6800 4150 50  0001 C CNN
	1    6800 4150
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66428D78
P 6800 1550
F 0 "#PWR?" H 6800 1300 50  0001 C CNN
F 1 "GND" V 6800 1350 50  0000 C CNN
F 2 "" H 6800 1550 50  0001 C CNN
F 3 "" H 6800 1550 50  0001 C CNN
	1    6800 1550
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66429552
P 6200 1450
F 0 "#PWR?" H 6200 1200 50  0001 C CNN
F 1 "GND" V 6200 1250 50  0000 C CNN
F 2 "" H 6200 1450 50  0001 C CNN
F 3 "" H 6200 1450 50  0001 C CNN
	1    6200 1450
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66429883
P 6200 2300
F 0 "#PWR?" H 6200 2050 50  0001 C CNN
F 1 "GND" V 6200 2100 50  0000 C CNN
F 2 "" H 6200 2300 50  0001 C CNN
F 3 "" H 6200 2300 50  0001 C CNN
	1    6200 2300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6642A07A
P 6200 3150
F 0 "#PWR?" H 6200 2900 50  0001 C CNN
F 1 "GND" V 6200 2950 50  0000 C CNN
F 2 "" H 6200 3150 50  0001 C CNN
F 3 "" H 6200 3150 50  0001 C CNN
	1    6200 3150
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6642A3A3
P 6200 4050
F 0 "#PWR?" H 6200 3800 50  0001 C CNN
F 1 "GND" V 6200 3850 50  0000 C CNN
F 2 "" H 6200 4050 50  0001 C CNN
F 3 "" H 6200 4050 50  0001 C CNN
	1    6200 4050
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR?
U 1 1 6643C635
P 4650 3100
F 0 "#PWR?" H 4650 2950 50  0001 C CNN
F 1 "+3.3V" V 4650 3350 50  0000 C CNN
F 2 "" H 4650 3100 50  0001 C CNN
F 3 "" H 4650 3100 50  0001 C CNN
	1    4650 3100
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 6643CB12
P 4850 2050
F 0 "R?" H 4920 2096 50  0001 L CNN
F 1 "10K" V 4850 2050 50  0000 C CNN
F 2 "" V 4780 2050 50  0001 C CNN
F 3 "~" H 4850 2050 50  0001 C CNN
	1    4850 2050
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 6643CD42
P 2800 3600
F 0 "C?" H 2915 3646 50  0001 L CNN
F 1 "100nf" H 2915 3600 50  0000 L CNN
F 2 "" H 2838 3450 50  0001 C CNN
F 3 "~" H 2800 3600 50  0001 C CNN
	1    2800 3600
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR?
U 1 1 66443A19
P 2800 3450
F 0 "#PWR?" H 2800 3300 50  0001 C CNN
F 1 "+3.3V" V 2800 3700 50  0000 C CNN
F 2 "" H 2800 3450 50  0001 C CNN
F 3 "" H 2800 3450 50  0001 C CNN
	1    2800 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66444811
P 2800 3750
F 0 "#PWR?" H 2800 3500 50  0001 C CNN
F 1 "GND" V 2800 3550 50  0000 C CNN
F 2 "" H 2800 3750 50  0001 C CNN
F 3 "" H 2800 3750 50  0001 C CNN
	1    2800 3750
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR?
U 1 1 664504D1
P 4850 1800
F 0 "#PWR?" H 4850 1650 50  0001 C CNN
F 1 "+3.3V" V 4850 2050 50  0000 C CNN
F 2 "" H 4850 1800 50  0001 C CNN
F 3 "" H 4850 1800 50  0001 C CNN
	1    4850 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 2200 4850 2300
Wire Wire Line
	4850 1800 4850 1900
Text GLabel 4550 2300 1    50   Input ~ 0
ROT1A
Text GLabel 4450 2300 1    50   Input ~ 0
ROT1B
Text GLabel 4150 2300 1    50   Input ~ 0
ROT2A
Text GLabel 4050 2300 1    50   Input ~ 0
ROT2B
Text GLabel 3850 3100 3    50   Input ~ 0
ROT3A
Text GLabel 3950 3100 3    50   Input ~ 0
ROT3B
Text GLabel 4250 3100 3    50   Input ~ 0
ROT4A
Text GLabel 4350 3100 3    50   Input ~ 0
ROT4B
Text GLabel 4350 2300 1    50   Input ~ 0
SW1
Text GLabel 3950 2300 1    50   Input ~ 0
SW2
Text GLabel 4050 3100 3    50   Input ~ 0
SW3
Text GLabel 4450 3100 3    50   Input ~ 0
SW4
$Comp
L power:GND #PWR?
U 1 1 66491CF1
P 4600 3700
F 0 "#PWR?" H 4600 3450 50  0001 C CNN
F 1 "GND" V 4600 3500 50  0000 C CNN
F 2 "" H 4600 3700 50  0001 C CNN
F 3 "" H 4600 3700 50  0001 C CNN
	1    4600 3700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66491F00
P 4700 3700
F 0 "#PWR?" H 4700 3450 50  0001 C CNN
F 1 "GND" V 4700 3500 50  0000 C CNN
F 2 "" H 4700 3700 50  0001 C CNN
F 3 "" H 4700 3700 50  0001 C CNN
	1    4700 3700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66495A69
P 4450 1400
F 0 "#PWR?" H 4450 1150 50  0001 C CNN
F 1 "GND" V 4450 1200 50  0000 C CNN
F 2 "" H 4450 1400 50  0001 C CNN
F 3 "" H 4450 1400 50  0001 C CNN
	1    4450 1400
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66495DD6
P 4550 1400
F 0 "#PWR?" H 4550 1150 50  0001 C CNN
F 1 "GND" V 4550 1200 50  0000 C CNN
F 2 "" H 4550 1400 50  0001 C CNN
F 3 "" H 4550 1400 50  0001 C CNN
	1    4550 1400
	-1   0    0    1   
$EndComp
$Comp
L 0_my_symbols:mcp23017 U?
U 1 1 663ECAFA
P 4500 2700
F 0 "U?" H 5228 2746 50  0001 L CNN
F 1 "mcp23017" H 4700 2700 50  0000 L CNN
F 2 "" H 4750 2450 50  0001 C CNN
F 3 "" H 4750 2450 50  0001 C CNN
	1    4500 2700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66499F8B
P 5150 2300
F 0 "#PWR?" H 5150 2050 50  0001 C CNN
F 1 "GND" V 5150 2100 50  0000 C CNN
F 2 "" H 5150 2300 50  0001 C CNN
F 3 "" H 5150 2300 50  0001 C CNN
	1    5150 2300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6649A25F
P 5050 2300
F 0 "#PWR?" H 5050 2050 50  0001 C CNN
F 1 "GND" V 5050 2100 50  0000 C CNN
F 2 "" H 5050 2300 50  0001 C CNN
F 3 "" H 5050 2300 50  0001 C CNN
	1    5050 2300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6649A57A
P 4950 2300
F 0 "#PWR?" H 4950 2050 50  0001 C CNN
F 1 "GND" V 4950 2100 50  0000 C CNN
F 2 "" H 4950 2300 50  0001 C CNN
F 3 "" H 4950 2300 50  0001 C CNN
	1    4950 2300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6649A95A
P 4850 3100
F 0 "#PWR?" H 4850 2850 50  0001 C CNN
F 1 "GND" V 4850 2900 50  0000 C CNN
F 2 "" H 4850 3100 50  0001 C CNN
F 3 "" H 4850 3100 50  0001 C CNN
	1    4850 3100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6649AD0A
P 5150 3100
F 0 "#PWR?" H 5150 2850 50  0001 C CNN
F 1 "GND" V 5150 2900 50  0000 C CNN
F 2 "" H 5150 3100 50  0001 C CNN
F 3 "" H 5150 3100 50  0001 C CNN
	1    5150 3100
	1    0    0    -1  
$EndComp
Text GLabel 4950 3100 3    50   Input ~ 0
SCL
Text GLabel 5050 3100 3    50   Input ~ 0
SDA
Text GLabel 2750 1900 2    50   Input ~ 0
SCL
Text GLabel 2750 2000 2    50   Input ~ 0
SDA
Text GLabel 4650 2300 1    50   Input ~ 0
ROT_INT
Text GLabel 2750 2100 2    50   Input ~ 0
ROT_INT
NoConn ~ 4750 2300
Text Notes 1950 1300 0    50   ~ 0
Note:  there are (1K) pullup resistors on\nSDA and SCL on the client board
Text GLabel 4750 2300 1    50   Input ~ 0
ROT_INT
Text Notes 2000 2900 0    50   ~ 0
Not resolved if there needs to be a\ndecoupling cap on the mpc23017
Text Notes 5450 950  0    50   ~ 0
may want 0.1uf ceramic caps tween pins and ground\nfor hardware debouncing
$EndSCHEMATC
