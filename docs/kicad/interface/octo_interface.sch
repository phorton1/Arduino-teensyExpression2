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
L Connector_Generic:Conn_01x04 J11
U 1 1 66159DC7
P 6350 4150
F 0 "J11" V 5350 4150 50  0000 R CNN
F 1 "PI_0" V 5250 4150 50  0000 R CNN
F 2 "cnc3018-PCB:my4pin" H 6350 4150 50  0001 C CNN
F 3 "~" H 6350 4150 50  0001 C CNN
	1    6350 4150
	0    1    -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J9
U 1 1 6615D6DC
P 4950 4150
F 0 "J9" V 3950 4200 50  0000 R CNN
F 1 "PI_2" V 3850 4200 50  0000 R CNN
F 2 "cnc3018-PCB:my3pin" H 4950 4150 50  0001 C CNN
F 3 "~" H 4950 4150 50  0001 C CNN
	1    4950 4150
	0    1    -1   0   
$EndComp
NoConn ~ 4950 4350
NoConn ~ 5650 4350
$Comp
L Connector_Generic:Conn_01x03 J8
U 1 1 66163A35
P 5550 4050
F 0 "J8" V 5050 4400 50  0000 R CNN
F 1 "PI_3" V 5150 4450 50  0000 R CNN
F 2 "cnc3018-PCB:my3pin" H 5550 4050 50  0001 C CNN
F 3 "~" H 5550 4050 50  0001 C CNN
	1    5550 4050
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR01
U 1 1 66165D1C
P 6350 4350
F 0 "#PWR01" H 6350 4100 50  0001 C CNN
F 1 "GND" V 6350 4150 50  0000 C CNN
F 2 "" H 6350 4350 50  0001 C CNN
F 3 "" H 6350 4350 50  0001 C CNN
	1    6350 4350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR02
U 1 1 66166E40
P 6450 4350
F 0 "#PWR02" H 6450 4200 50  0001 C CNN
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
Text GLabel 5550 4350 3    50   Input ~ 0
RPI_READY
Text GLabel 5450 4350 3    50   Input ~ 0
DISPLAY_CS
$Comp
L Connector_Generic:Conn_01x04 J10
U 1 1 6616B9A9
P 5650 4150
F 0 "J10" V 4650 4150 50  0000 R CNN
F 1 "PI_1" V 4550 4150 50  0000 R CNN
F 2 "cnc3018-PCB:my4pin" H 5650 4150 50  0001 C CNN
F 3 "~" H 5650 4150 50  0001 C CNN
	1    5650 4150
	0    1    -1   0   
$EndComp
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
$Comp
L Connector_Generic:Conn_01x03 J3
U 1 1 661747FA
P 6900 2150
F 0 "J3" H 6800 1800 50  0000 R CNN
F 1 "AOUT1" H 6800 1700 50  0000 R CNN
F 2 "cnc3018-PCB:my3pin" H 6900 2150 50  0001 C CNN
F 3 "~" H 6900 2150 50  0001 C CNN
	1    6900 2150
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J4
U 1 1 66174EFD
P 7000 2150
F 0 "J4" H 6900 1800 50  0000 R CNN
F 1 "AOUT2" H 6900 1700 50  0000 R CNN
F 2 "cnc3018-PCB:my3pin" H 7000 2150 50  0001 C CNN
F 3 "~" H 7000 2150 50  0001 C CNN
	1    7000 2150
	-1   0    0    -1  
$EndComp
NoConn ~ 6550 4150
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
NoConn ~ 5350 4150
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
Text GLabel 7200 2250 2    50   Input ~ 0
AGND
Text GLabel 6700 2050 0    50   Input ~ 0
AOUT1+
Text GLabel 7200 2150 2    50   Input ~ 0
AOUT2+
NoConn ~ 7000 2350
NoConn ~ 6900 2350
NoConn ~ 7200 2050
NoConn ~ 6700 2150
NoConn ~ 7000 1950
NoConn ~ 7000 1850
NoConn ~ 7000 1750
NoConn ~ 7000 1650
NoConn ~ 7000 1550
NoConn ~ 7000 1450
NoConn ~ 7000 1350
NoConn ~ 7000 1250
NoConn ~ 6900 1950
NoConn ~ 6900 1850
NoConn ~ 6900 1750
NoConn ~ 6900 1650
NoConn ~ 6900 1550
NoConn ~ 6900 1450
NoConn ~ 6900 1350
NoConn ~ 6900 1250
Wire Notes Line
	6850 1200 6850 1300
Wire Notes Line
	6850 1300 6950 1300
Wire Notes Line
	6950 1300 6950 1200
Wire Notes Line
	6950 1200 6850 1200
Wire Notes Line
	4400 1150 4400 1250
Wire Notes Line
	4400 1250 4500 1250
Wire Notes Line
	4500 1250 4500 1150
Wire Notes Line
	4500 1150 4400 1150
$Comp
L Connector_Generic:Conn_01x04 J1
U 1 1 66196645
P 4450 1300
F 0 "J1" H 4300 100 50  0000 R CNN
F 1 "AIN1" H 4350 0   50  0000 R CNN
F 2 "cnc3018-PCB:my4pin" H 4450 1300 50  0001 C CNN
F 3 "~" H 4450 1300 50  0001 C CNN
	1    4450 1300
	1    0    0    -1  
$EndComp
NoConn ~ 4450 1600
NoConn ~ 4450 1700
NoConn ~ 4450 1800
NoConn ~ 4450 1900
NoConn ~ 4450 2000
NoConn ~ 4450 2100
NoConn ~ 4450 2200
NoConn ~ 4450 2300
NoConn ~ 4250 1200
NoConn ~ 4250 1300
Text GLabel 4750 1200 2    50   Input ~ 0
ADC_FB1-
Text GLabel 4750 1300 2    50   Input ~ 0
ADC_FB1+
Text GLabel 4750 1400 2    50   Input ~ 0
ADC_FB2-
Text GLabel 4750 1500 2    50   Input ~ 0
ADC_FB2+
Text GLabel 6700 2250 0    50   Input ~ 0
AGND
Text GLabel 4250 1400 0    50   Input ~ 0
AGND
Text GLabel 4250 1500 0    50   Input ~ 0
AGND
$Comp
L Connector_Generic:Conn_01x12 J2
U 1 1 661A2A9C
P 4550 1700
F 0 "J2" H 4250 900 50  0000 L CNN
F 1 "AIN2" H 4250 800 50  0000 L CNN
F 2 "cnc3018-PCB:my14Pin" H 4550 1700 50  0001 C CNN
F 3 "~" H 4550 1700 50  0001 C CNN
	1    4550 1700
	-1   0    0    -1  
$EndComp
Text GLabel 4750 1600 2    50   Input ~ 0
ADC_FB3-
Text GLabel 4750 1700 2    50   Input ~ 0
ADC_FB3+
Text GLabel 4750 1800 2    50   Input ~ 0
ADC_FB4-
Text GLabel 4750 1900 2    50   Input ~ 0
ADC_FB4+
Text GLabel 4750 2000 2    50   Input ~ 0
ADC_FB5-
Text GLabel 4750 2100 2    50   Input ~ 0
ADC_FB5+
Text GLabel 4750 2200 2    50   Input ~ 0
ADC_FB6-
Text GLabel 4750 2300 2    50   Input ~ 0
ADC_FB6+
Text GLabel 8450 1200 0    50   Input ~ 0
AOUT1+
Text GLabel 8450 1750 0    50   Input ~ 0
AGND
$Comp
L Device:CP1 C1
U 1 1 661B546B
P 8700 1200
F 0 "C1" V 8952 1200 50  0000 C CNN
F 1 "10u" V 8861 1200 50  0000 C CNN
F 2 "0_my_footprints:CP_my10uf" H 8700 1200 50  0001 C CNN
F 3 "~" H 8700 1200 50  0001 C CNN
	1    8700 1200
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R4
U 1 1 661B5F9A
P 8900 1500
F 0 "R4" H 8970 1546 50  0000 L CNN
F 1 "10K" V 8900 1500 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 8830 1500 50  0001 C CNN
F 3 "~" H 8900 1500 50  0001 C CNN
	1    8900 1500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 661B69AD
P 9100 1200
F 0 "R1" V 8893 1200 50  0000 C CNN
F 1 "562" V 9100 1200 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 9030 1200 50  0001 C CNN
F 3 "~" H 9100 1200 50  0001 C CNN
	1    9100 1200
	0    1    1    0   
$EndComp
$Comp
L Device:C C4
U 1 1 661B714F
P 9300 1500
F 0 "C4" H 9415 1546 50  0000 L CNN
F 1 "1.5n" H 9415 1455 50  0000 L CNN
F 2 "cnc3018-PCB:my2pin" H 9338 1350 50  0001 C CNN
F 3 "~" H 9300 1500 50  0001 C CNN
	1    9300 1500
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J6
U 1 1 661B9D6F
P 9050 3550
F 0 "J6" H 8950 3200 50  0000 R CNN
F 1 "OUT" H 8950 3100 50  0000 R CNN
F 2 "cnc3018-PCB:my3pin" H 9050 3550 50  0001 C CNN
F 3 "~" H 9050 3550 50  0001 C CNN
	1    9050 3550
	1    0    0    -1  
$EndComp
Text GLabel 8850 3550 0    50   Input ~ 0
AGND
Text GLabel 9450 1200 2    50   Input ~ 0
OUT1
Text GLabel 8850 3450 0    50   Input ~ 0
OUT1
Wire Wire Line
	8450 1200 8550 1200
Wire Wire Line
	8850 1200 8900 1200
Wire Wire Line
	8900 1350 8900 1200
Connection ~ 8900 1200
Wire Wire Line
	8900 1200 8950 1200
Wire Wire Line
	9250 1200 9300 1200
Wire Wire Line
	9300 1350 9300 1200
Connection ~ 9300 1200
Wire Wire Line
	9300 1200 9450 1200
Wire Wire Line
	9300 1650 9300 1750
Wire Wire Line
	9300 1750 8900 1750
Wire Wire Line
	8900 1650 8900 1750
Connection ~ 8900 1750
Wire Wire Line
	8900 1750 8450 1750
Text GLabel 8450 2200 0    50   Input ~ 0
AOUT2+
Text GLabel 8450 2750 0    50   Input ~ 0
AGND
$Comp
L Device:CP1 C5
U 1 1 661C2552
P 8700 2200
F 0 "C5" V 8952 2200 50  0000 C CNN
F 1 "10u" V 8861 2200 50  0000 C CNN
F 2 "0_my_footprints:CP_my10uf" H 8700 2200 50  0001 C CNN
F 3 "~" H 8700 2200 50  0001 C CNN
	1    8700 2200
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R8
U 1 1 661C2558
P 8900 2500
F 0 "R8" H 8970 2546 50  0000 L CNN
F 1 "10K" V 8900 2500 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 8830 2500 50  0001 C CNN
F 3 "~" H 8900 2500 50  0001 C CNN
	1    8900 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 661C255E
P 9100 2200
F 0 "R5" V 8893 2200 50  0000 C CNN
F 1 "562" V 9100 2200 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 9030 2200 50  0001 C CNN
F 3 "~" H 9100 2200 50  0001 C CNN
	1    9100 2200
	0    1    1    0   
$EndComp
$Comp
L Device:C C8
U 1 1 661C2564
P 9300 2500
F 0 "C8" H 9415 2546 50  0000 L CNN
F 1 "1.5n" H 9415 2455 50  0000 L CNN
F 2 "cnc3018-PCB:my2pin" H 9338 2350 50  0001 C CNN
F 3 "~" H 9300 2500 50  0001 C CNN
	1    9300 2500
	1    0    0    -1  
$EndComp
Text GLabel 9450 2200 2    50   Input ~ 0
OUT2
Wire Wire Line
	8450 2200 8550 2200
Wire Wire Line
	8850 2200 8900 2200
Wire Wire Line
	8900 2350 8900 2200
Connection ~ 8900 2200
Wire Wire Line
	8900 2200 8950 2200
Wire Wire Line
	9250 2200 9300 2200
Wire Wire Line
	9300 2350 9300 2200
Connection ~ 9300 2200
Wire Wire Line
	9300 2200 9450 2200
Wire Wire Line
	9300 2650 9300 2750
Wire Wire Line
	9300 2750 8900 2750
Wire Wire Line
	8900 2650 8900 2750
Connection ~ 8900 2750
Wire Wire Line
	8900 2750 8450 2750
Text GLabel 8850 3650 0    50   Input ~ 0
OUT2
Text GLabel 2950 1250 2    50   Input ~ 0
ADC_FB1+
Text GLabel 3050 1800 2    50   Input ~ 0
AGND
$Comp
L Device:CP1 C2
U 1 1 661F294C
P 2500 1250
F 0 "C2" V 2752 1250 50  0000 C CNN
F 1 "10u" V 2661 1250 50  0000 C CNN
F 2 "0_my_footprints:CP_my10uf" H 2500 1250 50  0001 C CNN
F 3 "~" H 2500 1250 50  0001 C CNN
	1    2500 1250
	0    1    -1   0   
$EndComp
$Comp
L Device:R R3
U 1 1 661F2952
P 1900 1500
F 0 "R3" H 1970 1546 50  0000 L CNN
F 1 "100K" V 1900 1500 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 1830 1500 50  0001 C CNN
F 3 "~" H 1900 1500 50  0001 C CNN
	1    1900 1500
	-1   0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 661F2958
P 2100 1250
F 0 "R2" V 1893 1250 50  0000 C CNN
F 1 "150" V 2100 1250 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2030 1250 50  0001 C CNN
F 3 "~" H 2100 1250 50  0001 C CNN
	1    2100 1250
	0    -1   1    0   
$EndComp
$Comp
L Device:C C3
U 1 1 661F295E
P 2750 1500
F 0 "C3" H 2635 1546 50  0000 R CNN
F 1 "2.7n" H 2635 1455 50  0000 R CNN
F 2 "cnc3018-PCB:my2pin" H 2788 1350 50  0001 C CNN
F 3 "~" H 2750 1500 50  0001 C CNN
	1    2750 1500
	-1   0    0    -1  
$EndComp
Text GLabel 1700 1250 0    50   Input ~ 0
IN1
Wire Wire Line
	1700 1250 1900 1250
Wire Wire Line
	1900 1350 1900 1250
Connection ~ 1900 1250
Wire Wire Line
	1900 1250 1950 1250
Wire Wire Line
	2250 1250 2350 1250
Wire Wire Line
	2650 1250 2750 1250
Wire Wire Line
	2750 1350 2750 1250
Connection ~ 2750 1250
Wire Wire Line
	2750 1250 2950 1250
Wire Wire Line
	3050 1800 2750 1800
Wire Wire Line
	1900 1800 1900 1650
Wire Wire Line
	2750 1650 2750 1800
Connection ~ 2750 1800
Wire Wire Line
	2750 1800 1900 1800
Text GLabel 2950 2250 2    50   Input ~ 0
ADC_FB2+
Text GLabel 3050 2800 2    50   Input ~ 0
AGND
$Comp
L Device:CP1 C6
U 1 1 66238F31
P 2500 2250
F 0 "C6" V 2752 2250 50  0000 C CNN
F 1 "10u" V 2661 2250 50  0000 C CNN
F 2 "0_my_footprints:CP_my10uf" H 2500 2250 50  0001 C CNN
F 3 "~" H 2500 2250 50  0001 C CNN
	1    2500 2250
	0    1    -1   0   
$EndComp
$Comp
L Device:R R7
U 1 1 66238F37
P 1900 2500
F 0 "R7" H 1970 2546 50  0000 L CNN
F 1 "100K" V 1900 2500 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 1830 2500 50  0001 C CNN
F 3 "~" H 1900 2500 50  0001 C CNN
	1    1900 2500
	-1   0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 66238F3D
P 2100 2250
F 0 "R6" V 1893 2250 50  0000 C CNN
F 1 "150" V 2100 2250 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2030 2250 50  0001 C CNN
F 3 "~" H 2100 2250 50  0001 C CNN
	1    2100 2250
	0    -1   1    0   
$EndComp
$Comp
L Device:C C7
U 1 1 66238F43
P 2750 2500
F 0 "C7" H 2635 2546 50  0000 R CNN
F 1 "2.7n" H 2635 2455 50  0000 R CNN
F 2 "cnc3018-PCB:my2pin" H 2788 2350 50  0001 C CNN
F 3 "~" H 2750 2500 50  0001 C CNN
	1    2750 2500
	-1   0    0    -1  
$EndComp
Text GLabel 1700 2250 0    50   Input ~ 0
IN2
Wire Wire Line
	1700 2250 1900 2250
Wire Wire Line
	1900 2350 1900 2250
Connection ~ 1900 2250
Wire Wire Line
	1900 2250 1950 2250
Wire Wire Line
	2250 2250 2350 2250
Wire Wire Line
	2650 2250 2750 2250
Wire Wire Line
	2750 2350 2750 2250
Connection ~ 2750 2250
Wire Wire Line
	2750 2250 2950 2250
Wire Wire Line
	3050 2800 2750 2800
Wire Wire Line
	1900 2800 1900 2650
Wire Wire Line
	2750 2650 2750 2800
Connection ~ 2750 2800
Wire Wire Line
	2750 2800 1900 2800
$Comp
L Connector_Generic:Conn_01x03 J5
U 1 1 6624215C
P 1250 3400
F 0 "J5" H 1150 3050 50  0000 R CNN
F 1 "IN" H 1150 2950 50  0000 R CNN
F 2 "cnc3018-PCB:my3pin" H 1250 3400 50  0001 C CNN
F 3 "~" H 1250 3400 50  0001 C CNN
	1    1250 3400
	-1   0    0    -1  
$EndComp
Text GLabel 1450 3400 2    50   Input ~ 0
AGND
Text GLabel 1450 3300 2    50   Input ~ 0
IN1
Text GLabel 1450 3500 2    50   Input ~ 0
IN2
$Comp
L Device:C C9
U 1 1 66251FA8
P 2700 3700
F 0 "C9" V 2550 3900 50  0000 R CNN
F 1 "10u" V 2550 3600 50  0000 R CNN
F 2 "0_my_footprints:CP_my10uf" H 2738 3550 50  0001 C CNN
F 3 "~" H 2700 3700 50  0001 C CNN
	1    2700 3700
	0    1    -1   0   
$EndComp
Text GLabel 2550 3700 0    50   Input ~ 0
AGND
Text GLabel 2850 3700 2    50   Input ~ 0
ADC_FB1-
$Comp
L Device:C C10
U 1 1 66256CCA
P 2700 4050
F 0 "C10" V 2550 4250 50  0000 R CNN
F 1 "10u" V 2550 3950 50  0000 R CNN
F 2 "0_my_footprints:CP_my10uf" H 2738 3900 50  0001 C CNN
F 3 "~" H 2700 4050 50  0001 C CNN
	1    2700 4050
	0    1    -1   0   
$EndComp
Text GLabel 2550 4050 0    50   Input ~ 0
AGND
Text GLabel 2850 4050 2    50   Input ~ 0
ADC_FB2-
Text GLabel 2550 4500 0    50   Input ~ 0
AGND
Text GLabel 2850 4500 2    50   Input ~ 0
ADC_FB3-
Text GLabel 2550 4850 0    50   Input ~ 0
AGND
Text GLabel 2850 4850 2    50   Input ~ 0
ADC_FB3+
Text GLabel 2550 5250 0    50   Input ~ 0
AGND
Text GLabel 2850 5250 2    50   Input ~ 0
ADC_FB4-
Text GLabel 2550 5600 0    50   Input ~ 0
AGND
Text GLabel 2850 5600 2    50   Input ~ 0
ADC_FB4+
Text GLabel 2550 6000 0    50   Input ~ 0
AGND
Text GLabel 2850 6000 2    50   Input ~ 0
ADC_FB5-
Text GLabel 2550 6350 0    50   Input ~ 0
AGND
Text GLabel 2850 6350 2    50   Input ~ 0
ADC_FB5+
Text GLabel 2550 6800 0    50   Input ~ 0
AGND
Text GLabel 2850 6800 2    50   Input ~ 0
ADC_FB6-
Text GLabel 2550 7150 0    50   Input ~ 0
AGND
Text GLabel 2850 7150 2    50   Input ~ 0
ADC_FB6+
$Comp
L Device:R R9
U 1 1 662B4FF7
P 2700 4500
F 0 "R9" V 2800 4450 50  0000 L CNN
F 1 "10K" V 2700 4500 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2630 4500 50  0001 C CNN
F 3 "~" H 2700 4500 50  0001 C CNN
	1    2700 4500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R10
U 1 1 662B61EB
P 2700 4850
F 0 "R10" V 2800 4800 50  0000 L CNN
F 1 "10K" V 2700 4850 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2630 4850 50  0001 C CNN
F 3 "~" H 2700 4850 50  0001 C CNN
	1    2700 4850
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R11
U 1 1 662B6EC2
P 2700 5250
F 0 "R11" V 2800 5200 50  0000 L CNN
F 1 "10K" V 2700 5250 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2630 5250 50  0001 C CNN
F 3 "~" H 2700 5250 50  0001 C CNN
	1    2700 5250
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R12
U 1 1 662B7E0D
P 2700 5600
F 0 "R12" V 2800 5550 50  0000 L CNN
F 1 "10K" V 2700 5600 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2630 5600 50  0001 C CNN
F 3 "~" H 2700 5600 50  0001 C CNN
	1    2700 5600
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R13
U 1 1 662B8B3F
P 2700 6000
F 0 "R13" V 2800 5950 50  0000 L CNN
F 1 "10K" V 2700 6000 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2630 6000 50  0001 C CNN
F 3 "~" H 2700 6000 50  0001 C CNN
	1    2700 6000
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R14
U 1 1 662B97D1
P 2700 6350
F 0 "R14" V 2800 6300 50  0000 L CNN
F 1 "10K" V 2700 6350 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2630 6350 50  0001 C CNN
F 3 "~" H 2700 6350 50  0001 C CNN
	1    2700 6350
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J7
U 1 1 662BD669
P 4550 3600
F 0 "J7" H 4630 3642 50  0000 L CNN
F 1 "PI_RUN" H 4630 3551 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 4550 3600 50  0001 C CNN
F 3 "~" H 4550 3600 50  0001 C CNN
	1    4550 3600
	1    0    0    -1  
$EndComp
Text GLabel 4350 3600 0    50   Input ~ 0
RPI_RUN
$Comp
L Connector_Generic:Conn_01x08 J13
U 1 1 662C3EE5
P 8100 5550
F 0 "J13" V 8250 5250 50  0000 L CNN
F 1 "Teensy3.2" V 8250 5450 50  0000 L CNN
F 2 "cnc3018-PCB:my8Pin" H 8100 5550 50  0001 C CNN
F 3 "~" H 8100 5550 50  0001 C CNN
	1    8100 5550
	0    1    1    0   
$EndComp
Text GLabel 8200 5350 1    50   Input ~ 0
RPI_TX0
Text GLabel 8100 5350 1    50   Input ~ 0
RPI_RX0
Text GLabel 8000 5350 1    50   Input ~ 0
RPI_READY
Text GLabel 7900 5350 1    50   Input ~ 0
AUDIO_RX_ACTIVE
Text GLabel 7800 5350 1    50   Input ~ 0
AUDIO_TX_ACTIVE
$Comp
L power:GND #PWR05
U 1 1 662CCE50
P 8300 5350
F 0 "#PWR05" H 8300 5100 50  0001 C CNN
F 1 "GND" V 8300 5150 50  0000 C CNN
F 2 "" H 8300 5350 50  0001 C CNN
F 3 "" H 8300 5350 50  0001 C CNN
	1    8300 5350
	1    0    0    1   
$EndComp
$Comp
L power:+5V #PWR06
U 1 1 662CCE56
P 8400 5350
F 0 "#PWR06" H 8400 5200 50  0001 C CNN
F 1 "+5V" V 8400 5550 50  0000 C CNN
F 2 "" H 8400 5350 50  0001 C CNN
F 3 "" H 8400 5350 50  0001 C CNN
	1    8400 5350
	-1   0    0    -1  
$EndComp
Text GLabel 7700 5350 1    50   Input ~ 0
RPI_RUN
$Comp
L Connector_Generic:Conn_01x08 J12
U 1 1 662EE4BA
P 9650 4600
F 0 "J12" V 9800 4300 50  0000 L CNN
F 1 "DISPLAY" V 9800 4500 50  0000 L CNN
F 2 "cnc3018-PCB:my8Pin" H 9650 4600 50  0001 C CNN
F 3 "~" H 9650 4600 50  0001 C CNN
	1    9650 4600
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 662F46E9
P 9850 4800
F 0 "#PWR03" H 9850 4550 50  0001 C CNN
F 1 "GND" V 9850 4600 50  0000 C CNN
F 2 "" H 9850 4800 50  0001 C CNN
F 3 "" H 9850 4800 50  0001 C CNN
	1    9850 4800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR04
U 1 1 662F46EF
P 9950 4800
F 0 "#PWR04" H 9950 4650 50  0001 C CNN
F 1 "+5V" V 9950 5000 50  0000 C CNN
F 2 "" H 9950 4800 50  0001 C CNN
F 3 "" H 9950 4800 50  0001 C CNN
	1    9950 4800
	-1   0    0    1   
$EndComp
Text GLabel 9750 4800 3    50   Input ~ 0
DISPLAY_CS
Text GLabel 9650 4800 3    50   Input ~ 0
DISPLAY_CD
Text GLabel 9550 4800 3    50   Input ~ 0
DISPLAY_MOSI
Text GLabel 9450 4800 3    50   Input ~ 0
DISPLAY_SCLK
Text GLabel 9350 4800 3    50   Input ~ 0
DISPLAY_MISO
NoConn ~ 9250 4800
$Comp
L Device:R R15
U 1 1 663486E2
P 2700 6800
F 0 "R15" V 2800 6750 50  0000 L CNN
F 1 "10K" V 2700 6800 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2630 6800 50  0001 C CNN
F 3 "~" H 2700 6800 50  0001 C CNN
	1    2700 6800
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R16
U 1 1 66349449
P 2700 7150
F 0 "R16" V 2800 7100 50  0000 L CNN
F 1 "10K" V 2700 7150 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 2630 7150 50  0001 C CNN
F 3 "~" H 2700 7150 50  0001 C CNN
	1    2700 7150
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
