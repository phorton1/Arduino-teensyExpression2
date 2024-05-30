EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr B 17000 11000
encoding utf-8
Sheet 1 1
Title "Raspberry Pi Zero (W) uHAT Template Board"
Date "2019-02-28"
Rev "1.0"
Comp ""
Comment1 "This Schematic is licensed under MIT Open Source License."
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J1
U 1 1 5C77771F
P 2700 2100
F 0 "J1" H 2750 3217 50  0000 C CNN
F 1 "GPIO_CONNECTOR" H 2750 3126 50  0000 C CNN
F 2 "lib:PinSocket_2x20_P2.54mm_Vertical_Centered_Anchor" H 2700 2100 50  0001 C CNN
F 3 "~" H 2700 2100 50  0001 C CNN
	1    2700 2100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5C777805
P 2300 3250
F 0 "#PWR0101" H 2300 3000 50  0001 C CNN
F 1 "GND" H 2305 3077 50  0001 C CNN
F 2 "" H 2300 3250 50  0001 C CNN
F 3 "" H 2300 3250 50  0001 C CNN
	1    2300 3250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5C777838
P 3200 3250
F 0 "#PWR0102" H 3200 3000 50  0001 C CNN
F 1 "GND" H 3205 3077 50  0001 C CNN
F 2 "" H 3200 3250 50  0001 C CNN
F 3 "" H 3200 3250 50  0001 C CNN
	1    3200 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 1600 2300 1600
Wire Wire Line
	2300 1600 2300 2400
Wire Wire Line
	2500 2400 2300 2400
Connection ~ 2300 2400
Wire Wire Line
	2300 2400 2300 3100
Wire Wire Line
	2500 3100 2300 3100
Connection ~ 2300 3100
Wire Wire Line
	2300 3100 2300 3250
Wire Wire Line
	3000 1400 3200 1400
Wire Wire Line
	3200 1400 3200 1800
Wire Wire Line
	3000 1800 3200 1800
Connection ~ 3200 1800
Wire Wire Line
	3200 1800 3200 2100
Wire Wire Line
	3000 2100 3200 2100
Connection ~ 3200 2100
Wire Wire Line
	3000 2600 3200 2600
Wire Wire Line
	3200 2100 3200 2600
Connection ~ 3200 2600
Wire Wire Line
	3200 2600 3200 2800
Wire Wire Line
	3000 2800 3200 2800
Connection ~ 3200 2800
Wire Wire Line
	3200 2800 3200 3250
$Comp
L power:+3.3V #PWR0103
U 1 1 5C777AB0
P 2250 1100
F 0 "#PWR0103" H 2250 950 50  0001 C CNN
F 1 "+3.3V" H 2265 1273 50  0000 C CNN
F 2 "" H 2250 1100 50  0001 C CNN
F 3 "" H 2250 1100 50  0001 C CNN
	1    2250 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 1200 2250 1100
Wire Wire Line
	2500 2000 2250 2000
Wire Wire Line
	2250 2000 2250 1200
Connection ~ 2250 1200
$Comp
L power:+5V #PWR0104
U 1 1 5C777E01
P 3300 1100
F 0 "#PWR0104" H 3300 950 50  0001 C CNN
F 1 "+5V" H 3315 1273 50  0000 C CNN
F 2 "" H 3300 1100 50  0001 C CNN
F 3 "" H 3300 1100 50  0001 C CNN
	1    3300 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 1200 3300 1200
Wire Wire Line
	3300 1200 3300 1100
Wire Wire Line
	3000 1300 3300 1300
Wire Wire Line
	3300 1300 3300 1200
Connection ~ 3300 1200
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5C77824A
P 1850 1100
F 0 "#FLG0101" H 1850 1175 50  0001 C CNN
F 1 "PWR_FLAG" H 1850 1274 50  0000 C CNN
F 2 "" H 1850 1100 50  0001 C CNN
F 3 "~" H 1850 1100 50  0001 C CNN
	1    1850 1100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5C778504
P 1900 3350
F 0 "#PWR0105" H 1900 3100 50  0001 C CNN
F 1 "GND" H 1905 3177 50  0001 C CNN
F 2 "" H 1900 3350 50  0001 C CNN
F 3 "" H 1900 3350 50  0001 C CNN
	1    1900 3350
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5C778511
P 1900 3300
F 0 "#FLG0102" H 1900 3375 50  0001 C CNN
F 1 "PWR_FLAG" H 1900 3474 50  0000 C CNN
F 2 "" H 1900 3300 50  0001 C CNN
F 3 "~" H 1900 3300 50  0001 C CNN
	1    1900 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 3300 1900 3350
Text Notes 3850 1300 0    50   ~ 10
If back powering Pi with 5V \nNOTE that the Raspberry Pi 3B+ and Pi Zero \nand ZeroW do not include an input ZVD.
Wire Notes Line
	3800 1000 3800 1350
Wire Notes Line
	3800 1350 5650 1350
Wire Notes Line
	5800 1200 5800 850 
Wire Notes Line
	5650 1000 3800 1000
Wire Wire Line
	2250 1200 2500 1200
Wire Wire Line
	1850 1200 1850 1100
Wire Wire Line
	1850 1200 2250 1200
$Comp
L power:PWR_FLAG #FLG0103
U 1 1 5C77CEFA
P 3650 1100
F 0 "#FLG0103" H 3650 1175 50  0001 C CNN
F 1 "PWR_FLAG" H 3650 1274 50  0000 C CNN
F 2 "" H 3650 1100 50  0001 C CNN
F 3 "~" H 3650 1100 50  0001 C CNN
	1    3650 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 1200 3650 1200
Wire Wire Line
	3650 1100 3650 1200
Text Label 1550 1300 0    50   ~ 0
GPIO2_SDA1
Text Label 1550 1400 0    50   ~ 0
GPIO3_SCL1
Text Label 1550 1500 0    50   ~ 0
GPIO4_GPIO_GCLK
Text Label 1550 1700 0    50   ~ 0
GPIO17_GEN0
Text Label 1550 1800 0    50   ~ 0
GPIO27_GEN2
Text Label 1550 1900 0    50   ~ 0
GPIO22_GEN3
Text Label 1550 2100 0    50   ~ 0
GPIO10_SPI_MOSI
Wire Wire Line
	1450 2100 2500 2100
Wire Wire Line
	1450 2200 2500 2200
Wire Wire Line
	1450 2500 2500 2500
Wire Wire Line
	1450 2600 2500 2600
Wire Wire Line
	1450 2700 2500 2700
Wire Wire Line
	1450 2800 2500 2800
Wire Wire Line
	1450 2900 2500 2900
Wire Wire Line
	1450 3000 2500 3000
Wire Wire Line
	1450 1900 2500 1900
Wire Wire Line
	1450 1800 2500 1800
Wire Wire Line
	1450 1700 2500 1700
Wire Wire Line
	1450 1500 2500 1500
Wire Wire Line
	1450 1400 2500 1400
Wire Wire Line
	1450 1300 2500 1300
Text Label 1550 2200 0    50   ~ 0
GPIO9_SPI_MISO
Text Label 1550 2500 0    50   ~ 0
ID_SD
Text Label 1550 2600 0    50   ~ 0
GPIO5
Text Label 1550 2700 0    50   ~ 0
GPIO6
Text Label 1550 2800 0    50   ~ 0
GPIO13
Text Label 1550 2900 0    50   ~ 0
GPIO19
Text Label 1550 3000 0    50   ~ 0
GPIO26
NoConn ~ 1450 1800
NoConn ~ 1450 1900
NoConn ~ 1450 2500
NoConn ~ 1450 2600
NoConn ~ 1450 2700
NoConn ~ 1450 2800
NoConn ~ 1450 3000
Text Label 3350 1500 0    50   ~ 0
GPIO14_TXD0
Text Label 3350 1600 0    50   ~ 0
GPIO15_RXD0
Text Label 3350 1700 0    50   ~ 0
GPIO18_GEN1
Text Label 3350 1900 0    50   ~ 0
GPIO23_GEN4
Text Label 3350 2000 0    50   ~ 0
GPIO24_GEN5
Text Label 3350 2200 0    50   ~ 0
GPIO25_GEN6
Text Label 3350 2300 0    50   ~ 0
GPIO8_SPI_CE0_N
Text Label 3350 2400 0    50   ~ 0
GPIO7_SPI_CE1_N
Wire Wire Line
	3000 2300 4050 2300
Wire Wire Line
	3000 2400 4050 2400
Text Label 3350 2500 0    50   ~ 0
ID_SC
Text Label 3350 2700 0    50   ~ 0
GPIO12
Text Label 3350 2900 0    50   ~ 0
GPIO16
Text Label 3350 3000 0    50   ~ 0
GPIO20
Text Label 3350 3100 0    50   ~ 0
GPIO21
Wire Wire Line
	3000 1500 4050 1500
Wire Wire Line
	3000 1600 4050 1600
Wire Wire Line
	3000 1700 4050 1700
Wire Wire Line
	3000 1900 4050 1900
Wire Wire Line
	3000 2000 4050 2000
Wire Wire Line
	3000 2200 4050 2200
Wire Wire Line
	3000 2500 4050 2500
Wire Wire Line
	3000 2700 4050 2700
Wire Wire Line
	3000 2900 4050 2900
Wire Wire Line
	3000 3000 4050 3000
NoConn ~ 4050 1900
NoConn ~ 4050 2000
NoConn ~ 4050 2500
Wire Wire Line
	3000 3100 4050 3100
$Comp
L Mechanical:MountingHole H1
U 1 1 5C7C4C81
P 6600 950
F 0 "H1" H 6700 996 50  0000 L CNN
F 1 "MountingHole" H 6700 905 50  0000 L CNN
F 2 "lib:MountingHole_2.7mm_M2.5_uHAT_RPi" H 6600 950 50  0001 C CNN
F 3 "~" H 6600 950 50  0001 C CNN
	1    6600 950 
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5C7C7FBC
P 6600 1150
F 0 "H2" H 6700 1196 50  0000 L CNN
F 1 "MountingHole" H 6700 1105 50  0000 L CNN
F 2 "lib:MountingHole_2.7mm_M2.5_uHAT_RPi" H 6600 1150 50  0001 C CNN
F 3 "~" H 6600 1150 50  0001 C CNN
	1    6600 1150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5C7C8014
P 6600 1350
F 0 "H3" H 6700 1396 50  0000 L CNN
F 1 "MountingHole" H 6700 1305 50  0000 L CNN
F 2 "lib:MountingHole_2.7mm_M2.5_uHAT_RPi" H 6600 1350 50  0001 C CNN
F 3 "~" H 6600 1350 50  0001 C CNN
	1    6600 1350
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5C7C8030
P 6600 1550
F 0 "H4" H 6700 1596 50  0000 L CNN
F 1 "MountingHole" H 6700 1505 50  0000 L CNN
F 2 "lib:MountingHole_2.7mm_M2.5_uHAT_RPi" H 6600 1550 50  0001 C CNN
F 3 "~" H 6600 1550 50  0001 C CNN
	1    6600 1550
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J2
U 1 1 66572B9B
P 2650 6100
F 0 "J2" V 2800 6350 50  0000 R CNN
F 1 "display" V 2800 6200 50  0000 R CNN
F 2 "0_my_footprints:myJSTx08" H 2650 6100 50  0001 C CNN
F 3 "~" H 2650 6100 50  0001 C CNN
	1    2650 6100
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR02
U 1 1 66575AB4
P 3050 5900
F 0 "#PWR02" H 3050 5650 50  0001 C CNN
F 1 "GND" H 3055 5727 50  0001 C CNN
F 2 "" H 3050 5900 50  0001 C CNN
F 3 "" H 3050 5900 50  0001 C CNN
	1    3050 5900
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 66579D48
P 2350 5900
F 0 "#PWR01" H 2350 5750 50  0001 C CNN
F 1 "+5V" V 2350 6100 50  0000 C CNN
F 2 "" H 2350 5900 50  0001 C CNN
F 3 "" H 2350 5900 50  0001 C CNN
	1    2350 5900
	1    0    0    -1  
$EndComp
Text GLabel 2450 5900 1    50   Input ~ 0
LED_CS
Text GLabel 2550 5900 1    50   Input ~ 0
TOUCH_CS
Text GLabel 2650 5900 1    50   Input ~ 0
TFT_CD
Text GLabel 2750 5900 1    50   Input ~ 0
MOSI
Text GLabel 2850 5900 1    50   Input ~ 0
MISO
Text GLabel 2950 5900 1    50   Input ~ 0
SCLK
Text GLabel 4050 2300 2    50   Input ~ 0
LED_CS
Text GLabel 4050 2400 2    50   Input ~ 0
TOUCH_CS
Text GLabel 1450 1700 0    50   Input ~ 0
TFT_CD
Text GLabel 1450 2100 0    50   Input ~ 0
MOSI
Text GLabel 1450 2200 0    50   Input ~ 0
MISO
Text GLabel 1450 2300 0    50   Input ~ 0
SCLK
Text Label 1550 2300 0    50   ~ 0
GPIO11_SPI_SCLK
Wire Wire Line
	1450 2300 2500 2300
$Comp
L Device:CP1 C1
U 1 1 665C2343
P 9350 3250
F 0 "C1" H 9465 3296 50  0000 L CNN
F 1 "2.2uf" H 9465 3205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 9350 3250 50  0001 C CNN
F 3 "~" H 9350 3250 50  0001 C CNN
	1    9350 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C2
U 1 1 665C40E4
P 9800 3250
F 0 "C2" H 9915 3296 50  0000 L CNN
F 1 "0.1uf" H 9915 3205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 9800 3250 50  0001 C CNN
F 3 "~" H 9800 3250 50  0001 C CNN
	1    9800 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C3
U 1 1 665DEBAC
P 10250 3250
F 0 "C3" H 10365 3296 50  0000 L CNN
F 1 "0.1uf" H 10365 3205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 10250 3250 50  0001 C CNN
F 3 "~" H 10250 3250 50  0001 C CNN
	1    10250 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C6
U 1 1 665E4B1B
P 11550 3400
F 0 "C6" H 11665 3446 50  0000 L CNN
F 1 "2.2uf" H 11665 3355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 11550 3400 50  0001 C CNN
F 3 "~" H 11550 3400 50  0001 C CNN
	1    11550 3400
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C4
U 1 1 665E655D
P 12700 3300
F 0 "C4" H 12815 3346 50  0000 L CNN
F 1 "2.2uf" H 12815 3255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 12700 3300 50  0001 C CNN
F 3 "~" H 12700 3300 50  0001 C CNN
	1    12700 3300
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C5
U 1 1 665E793F
P 13200 3300
F 0 "C5" H 13315 3346 50  0000 L CNN
F 1 "0.1uf" H 13315 3255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 13200 3300 50  0001 C CNN
F 3 "~" H 13200 3300 50  0001 C CNN
	1    13200 3300
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C7
U 1 1 665EA775
P 13400 4050
F 0 "C7" V 13550 4100 50  0000 L CNN
F 1 "2.2uf" V 13550 3900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 13400 4050 50  0001 C CNN
F 3 "~" H 13400 4050 50  0001 C CNN
	1    13400 4050
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP1 C12
U 1 1 665F76E4
P 11050 6600
F 0 "C12" H 11165 6646 50  0000 L CNN
F 1 "0.15uf" H 11165 6555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 11050 6600 50  0001 C CNN
F 3 "~" H 11050 6600 50  0001 C CNN
	1    11050 6600
	1    0    0    -1  
$EndComp
$Comp
L 0_my_teensy:pauls_sgtl_5000_20 U2
U 1 1 665FF80D
P 12050 5450
F 0 "U2" H 12050 4950 50  0000 C CNN
F 1 "pauls_sgtl_5000_20" H 12050 5050 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-20-1EP_3x3mm_P0.4mm_EP1.65x1.65mm" H 12050 5700 50  0001 C CNN
F 3 "https://www.nxp.com/docs/en/data-sheet/SGTL5000.pdf" H 12050 5450 50  0001 C CNN
	1    12050 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C13
U 1 1 66622F61
P 12900 6700
F 0 "C13" H 13015 6746 50  0000 L CNN
F 1 "2.2uf" H 13015 6655 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 12900 6700 50  0001 C CNN
F 3 "~" H 12900 6700 50  0001 C CNN
	1    12900 6700
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C11
U 1 1 666245B4
P 13300 6000
F 0 "C11" V 13450 6150 50  0000 C CNN
F 1 "0.1uf" V 13450 5950 50  0000 C CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 13300 6000 50  0001 C CNN
F 3 "~" H 13300 6000 50  0001 C CNN
	1    13300 6000
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 6662B8CB
P 10800 4550
F 0 "R1" H 10870 4596 50  0000 L CNN
F 1 "2.2k" V 10800 4550 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 10730 4550 50  0001 C CNN
F 3 "~" H 10800 4550 50  0001 C CNN
	1    10800 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 66638229
P 10850 5200
F 0 "R3" V 10950 5150 50  0000 L CNN
F 1 "100" V 10850 5200 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 10780 5200 50  0001 C CNN
F 3 "~" H 10850 5200 50  0001 C CNN
	1    10850 5200
	0    -1   -1   0   
$EndComp
$Comp
L Regulator_Linear:AP7361C-18E U1
U 1 1 66694AD5
P 11050 3250
F 0 "U1" H 11050 3492 50  0000 C CNN
F 1 "AP7313-18" H 11050 3401 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 11050 3475 50  0001 C CIN
F 3 "https://www.diodes.com/assets/Datasheets/AP7361C.pdf" H 11050 3200 50  0001 C CNN
	1    11050 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C8
U 1 1 6671088E
P 13400 4400
F 0 "C8" V 13550 4450 50  0000 L CNN
F 1 "2.2uf" V 13550 4250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 13400 4400 50  0001 C CNN
F 3 "~" H 13400 4400 50  0001 C CNN
	1    13400 4400
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP1 C9
U 1 1 66711C8F
P 13400 4750
F 0 "C9" V 13550 4800 50  0000 L CNN
F 1 "2.2uf" V 13550 4600 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 13400 4750 50  0001 C CNN
F 3 "~" H 13400 4750 50  0001 C CNN
	1    13400 4750
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP1 C10
U 1 1 66712EE0
P 13400 5100
F 0 "C10" V 13550 5150 50  0000 L CNN
F 1 "2.2uf" V 13550 4950 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 13400 5100 50  0001 C CNN
F 3 "~" H 13400 5100 50  0001 C CNN
	1    13400 5100
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 6671720D
P 11050 4550
F 0 "R2" H 11120 4596 50  0000 L CNN
F 1 "2.2k" V 11050 4550 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 10980 4550 50  0001 C CNN
F 3 "~" H 11050 4550 50  0001 C CNN
	1    11050 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 6671E4B6
P 13300 6250
F 0 "R4" V 13200 6200 50  0000 L CNN
F 1 "2.2k" V 13300 6250 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 13230 6250 50  0001 C CNN
F 3 "~" H 13300 6250 50  0001 C CNN
	1    13300 6250
	0    -1   -1   0   
$EndComp
$Comp
L pspice:INDUCTOR L1
U 1 1 66732386
P 12150 2850
F 0 "L1" H 12150 3065 50  0000 C CNN
F 1 "Ferrite 600" H 12150 2974 50  0000 C CNN
F 2 "Inductor_SMD:L_1206_3216Metric" H 12150 2850 50  0001 C CNN
F 3 "~" H 12150 2850 50  0001 C CNN
	1    12150 2850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 66736651
P 9350 3400
F 0 "#PWR04" H 9350 3150 50  0001 C CNN
F 1 "GND" H 9355 3227 50  0001 C CNN
F 2 "" H 9350 3400 50  0001 C CNN
F 3 "" H 9350 3400 50  0001 C CNN
	1    9350 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 66737978
P 9800 3400
F 0 "#PWR05" H 9800 3150 50  0001 C CNN
F 1 "GND" H 9805 3227 50  0001 C CNN
F 2 "" H 9800 3400 50  0001 C CNN
F 3 "" H 9800 3400 50  0001 C CNN
	1    9800 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 66738CBD
P 10250 3400
F 0 "#PWR06" H 10250 3150 50  0001 C CNN
F 1 "GND" H 10255 3227 50  0001 C CNN
F 2 "" H 10250 3400 50  0001 C CNN
F 3 "" H 10250 3400 50  0001 C CNN
	1    10250 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 6673A0E5
P 11050 3550
F 0 "#PWR09" H 11050 3300 50  0001 C CNN
F 1 "GND" H 11055 3377 50  0001 C CNN
F 2 "" H 11050 3550 50  0001 C CNN
F 3 "" H 11050 3550 50  0001 C CNN
	1    11050 3550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 6673B339
P 11550 3550
F 0 "#PWR010" H 11550 3300 50  0001 C CNN
F 1 "GND" H 11555 3377 50  0001 C CNN
F 2 "" H 11550 3550 50  0001 C CNN
F 3 "" H 11550 3550 50  0001 C CNN
	1    11550 3550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR017
U 1 1 6673D272
P 11050 6750
F 0 "#PWR017" H 11050 6500 50  0001 C CNN
F 1 "GND" H 11055 6577 50  0001 C CNN
F 2 "" H 11050 6750 50  0001 C CNN
F 3 "" H 11050 6750 50  0001 C CNN
	1    11050 6750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 6673E682
P 12900 6850
F 0 "#PWR018" H 12900 6600 50  0001 C CNN
F 1 "GND" H 12905 6677 50  0001 C CNN
F 2 "" H 12900 6850 50  0001 C CNN
F 3 "" H 12900 6850 50  0001 C CNN
	1    12900 6850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 667408A5
P 12700 3450
F 0 "#PWR07" H 12700 3200 50  0001 C CNN
F 1 "GND" H 12705 3277 50  0001 C CNN
F 2 "" H 12700 3450 50  0001 C CNN
F 3 "" H 12700 3450 50  0001 C CNN
	1    12700 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 66741BB5
P 13200 3450
F 0 "#PWR08" H 13200 3200 50  0001 C CNN
F 1 "GND" H 13205 3277 50  0001 C CNN
F 2 "" H 13200 3450 50  0001 C CNN
F 3 "" H 13200 3450 50  0001 C CNN
	1    13200 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 66743AF8
P 12050 6550
F 0 "#PWR016" H 12050 6300 50  0001 C CNN
F 1 "GND" H 12055 6377 50  0001 C CNN
F 2 "" H 12050 6550 50  0001 C CNN
F 3 "" H 12050 6550 50  0001 C CNN
	1    12050 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	11900 2850 11750 2850
Wire Wire Line
	12400 2850 12500 2850
Wire Wire Line
	13200 2850 13200 3150
Wire Wire Line
	12700 3150 12700 2850
Connection ~ 12700 2850
Wire Wire Line
	12700 2850 13200 2850
Wire Wire Line
	12500 2850 12500 3850
Wire Wire Line
	12500 3850 12250 3850
Wire Wire Line
	12250 3850 12250 4350
Connection ~ 12500 2850
Wire Wire Line
	12500 2850 12700 2850
Connection ~ 11750 2850
Wire Wire Line
	11750 2850 10650 2850
Wire Wire Line
	11750 2850 11750 3000
Wire Wire Line
	12050 3000 11750 3000
Wire Wire Line
	12050 3000 12050 4350
Wire Wire Line
	11850 4350 11850 3250
Wire Wire Line
	11850 3250 11550 3250
Connection ~ 11550 3250
Wire Wire Line
	11550 3250 11350 3250
Wire Wire Line
	10750 3250 10650 3250
Wire Wire Line
	10650 3250 10650 2850
Connection ~ 10650 2850
Wire Wire Line
	10650 2850 10250 2850
Wire Wire Line
	10250 3100 10250 2850
Connection ~ 10250 2850
Wire Wire Line
	10250 2850 9800 2850
Wire Wire Line
	9800 3100 9800 2850
Connection ~ 9800 2850
Wire Wire Line
	9800 2850 9350 2850
Wire Wire Line
	9350 3100 9350 2850
Wire Wire Line
	12650 5100 13250 5100
Wire Wire Line
	12650 5000 13050 5000
Wire Wire Line
	13050 5000 13050 4750
Wire Wire Line
	13050 4750 13250 4750
Wire Wire Line
	12650 4900 12950 4900
Wire Wire Line
	12950 4900 12950 4400
Wire Wire Line
	12950 4400 13250 4400
Wire Wire Line
	12650 4800 12850 4800
Wire Wire Line
	12850 4800 12850 4050
Wire Wire Line
	12850 4050 13250 4050
Wire Wire Line
	12650 5450 13800 5450
Wire Wire Line
	12650 5550 13800 5550
Wire Wire Line
	12650 5650 13800 5650
Wire Wire Line
	12650 6000 13150 6000
Wire Wire Line
	12650 6250 12900 6250
Wire Wire Line
	12900 6550 12900 6250
Connection ~ 12900 6250
Wire Wire Line
	12900 6250 13150 6250
Wire Wire Line
	13450 6000 13600 6000
Wire Wire Line
	13450 6250 13600 6250
Wire Wire Line
	13600 6250 13600 6000
Connection ~ 13600 6000
Wire Wire Line
	13600 6000 13800 6000
Wire Wire Line
	11450 6250 11050 6250
Wire Wire Line
	11050 6250 11050 6450
Wire Wire Line
	11450 4800 11050 4800
Wire Wire Line
	11450 4900 10800 4900
Wire Wire Line
	10800 4700 10800 4900
Connection ~ 10800 4900
Wire Wire Line
	10800 4900 10400 4900
Wire Wire Line
	11050 4700 11050 4800
Connection ~ 11050 4800
Wire Wire Line
	11050 4800 10400 4800
Wire Wire Line
	11000 5200 11450 5200
Wire Wire Line
	10700 5200 10400 5200
Wire Wire Line
	11450 5300 10400 5300
Wire Wire Line
	11450 5400 10400 5400
Wire Wire Line
	11450 5500 10400 5500
Wire Wire Line
	11450 5600 10400 5600
$Comp
L power:+3.3V #PWR03
U 1 1 667E8F34
P 9350 2650
F 0 "#PWR03" H 9350 2500 50  0001 C CNN
F 1 "+3.3V" H 9365 2823 50  0000 C CNN
F 2 "" H 9350 2650 50  0001 C CNN
F 3 "" H 9350 2650 50  0001 C CNN
	1    9350 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9350 2650 9350 2850
Connection ~ 9350 2850
$Comp
L power:+3.3V #PWR011
U 1 1 667F1BE4
P 10800 4400
F 0 "#PWR011" H 10800 4250 50  0001 C CNN
F 1 "+3.3V" H 10815 4573 50  0000 C CNN
F 2 "" H 10800 4400 50  0001 C CNN
F 3 "" H 10800 4400 50  0001 C CNN
	1    10800 4400
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR012
U 1 1 667F5890
P 11050 4400
F 0 "#PWR012" H 11050 4250 50  0001 C CNN
F 1 "+3.3V" H 11065 4573 50  0000 C CNN
F 2 "" H 11050 4400 50  0001 C CNN
F 3 "" H 11050 4400 50  0001 C CNN
	1    11050 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	13550 5100 13800 5100
Wire Wire Line
	13550 4750 13800 4750
Wire Wire Line
	13550 4400 13800 4400
Wire Wire Line
	13550 4050 13800 4050
Text GLabel 10400 5200 0    50   Input ~ 0
SGTL_MCLK
Text GLabel 10400 5300 0    50   Input ~ 0
SGTL_BLCK
Text GLabel 10400 5400 0    50   Input ~ 0
SGTL_FCLK
Text GLabel 10400 5500 0    50   Input ~ 0
SGTL_I2S_IN
Text GLabel 10400 5600 0    50   Input ~ 0
SGTL_I2S_OUT
Text GLabel 10400 4800 0    50   Input ~ 0
SDA
Text GLabel 10400 4900 0    50   Input ~ 0
SCL
Text GLabel 1450 1300 0    50   Input ~ 0
SDA
Text GLabel 1450 1400 0    50   Input ~ 0
SCL
Text GLabel 1450 1500 0    50   Input ~ 0
SGTL_MCLK
Text GLabel 4050 1700 2    50   Input ~ 0
SGTL_BLCK
Text GLabel 1450 2900 0    50   Input ~ 0
SGTL_FCLK
Text GLabel 4050 3100 2    50   Input ~ 0
SGTL_I2S_IN
Text GLabel 4050 3000 2    50   Input ~ 0
SGTL_I2S_OUT
Text GLabel 13800 4050 2    50   Input ~ 0
LINEIN_L
Text GLabel 13800 4400 2    50   Input ~ 0
LINEIN_R
Text GLabel 13800 5100 2    50   Input ~ 0
LINEOUT_R
Text GLabel 13800 4750 2    50   Input ~ 0
LINEOUT_L
Text GLabel 13800 6000 2    50   Input ~ 0
MIC_IN
Text GLabel 13800 5450 2    50   Input ~ 0
HP_L
Text GLabel 13800 5550 2    50   Input ~ 0
HP_R
Text GLabel 13800 5650 2    50   Input ~ 0
HP_GND
Text GLabel 4050 2900 2    50   Input ~ 0
I2S_TX_ACTIVE
Text GLabel 4050 2700 2    50   Input ~ 0
I2S_RX_ACTIVE
Text GLabel 4050 2200 2    50   Input ~ 0
RPI_READY
Text GLabel 4050 1500 2    50   Input ~ 0
RPI_TX
Text GLabel 4050 1600 2    50   Input ~ 0
RPI_RX
$Comp
L Connector_Generic:Conn_01x05 J3
U 1 1 66906118
P 4300 6100
F 0 "J3" V 4450 5800 50  0000 L CNN
F 1 "esp32" V 4450 6000 50  0000 L CNN
F 2 "0_my_footprints:myJSTx05" H 4300 6100 50  0001 C CNN
F 3 "~" H 4300 6100 50  0001 C CNN
	1    4300 6100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR013
U 1 1 66909EDC
P 4500 5900
F 0 "#PWR013" H 4500 5650 50  0001 C CNN
F 1 "GND" H 4505 5727 50  0001 C CNN
F 2 "" H 4500 5900 50  0001 C CNN
F 3 "" H 4500 5900 50  0001 C CNN
	1    4500 5900
	-1   0    0    1   
$EndComp
Text GLabel 4300 5900 1    50   Input ~ 0
RPI_TX
Text GLabel 4400 5900 1    50   Input ~ 0
RPI_RX
Text GLabel 4200 5900 1    50   Input ~ 0
RPI_READY
Text GLabel 4100 5900 1    50   Input ~ 0
RPI_RUN
$Comp
L Connector_Generic:Conn_01x05 J4
U 1 1 669204D4
P 5450 6100
F 0 "J4" V 5600 5800 50  0000 L CNN
F 1 "audio" V 5600 6000 50  0000 L CNN
F 2 "0_my_footprints:myJSTx05" H 5450 6100 50  0001 C CNN
F 3 "~" H 5450 6100 50  0001 C CNN
	1    5450 6100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR014
U 1 1 66925DC4
P 5450 5900
F 0 "#PWR014" H 5450 5650 50  0001 C CNN
F 1 "GND" H 5455 5727 50  0001 C CNN
F 2 "" H 5450 5900 50  0001 C CNN
F 3 "" H 5450 5900 50  0001 C CNN
	1    5450 5900
	-1   0    0    1   
$EndComp
Text GLabel 5250 5900 1    50   Input ~ 0
LINEIN_L
Text GLabel 5350 5900 1    50   Input ~ 0
LINEIN_R
Text GLabel 5650 5900 1    50   Input ~ 0
LINEOUT_R
Text GLabel 5550 5900 1    50   Input ~ 0
LINEOUT_L
$Comp
L Connector_Generic:Conn_01x03 J5
U 1 1 6694CA8A
P 6300 6100
F 0 "J5" V 6450 5950 50  0000 L CNN
F 1 "HP" V 6450 6050 50  0000 L CNN
F 2 "0_my_footprints:myJSTx03" H 6300 6100 50  0001 C CNN
F 3 "~" H 6300 6100 50  0001 C CNN
	1    6300 6100
	0    1    1    0   
$EndComp
Text GLabel 6200 5900 1    50   Input ~ 0
HP_L
Text GLabel 6400 5900 1    50   Input ~ 0
HP_R
Text GLabel 6300 5900 1    50   Input ~ 0
HP_GND
$Comp
L Connector_Generic:Conn_01x02 J6
U 1 1 6695AB42
P 6950 6100
F 0 "J6" V 6822 6180 50  0000 L CNN
F 1 "mic" V 7100 5950 50  0000 L CNN
F 2 "0_my_footprints:myJSTx02" H 6950 6100 50  0001 C CNN
F 3 "~" H 6950 6100 50  0001 C CNN
	1    6950 6100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR015
U 1 1 6695D46A
P 6850 5900
F 0 "#PWR015" H 6850 5650 50  0001 C CNN
F 1 "GND" H 6855 5727 50  0001 C CNN
F 2 "" H 6850 5900 50  0001 C CNN
F 3 "" H 6850 5900 50  0001 C CNN
	1    6850 5900
	-1   0    0    1   
$EndComp
Text GLabel 6950 5900 1    50   Input ~ 0
MIC_IN
$Comp
L Connector_Generic:Conn_01x01 J7
U 1 1 6696873E
P 7500 6100
F 0 "J7" V 7600 5850 50  0000 L CNN
F 1 "RPI_RUN" V 7600 6000 50  0000 L CNN
F 2 "0_my_footprints:myPinHeader_1x01" H 7500 6100 50  0001 C CNN
F 3 "~" H 7500 6100 50  0001 C CNN
	1    7500 6100
	0    1    1    0   
$EndComp
Text GLabel 7500 5900 1    50   Input ~ 0
RPI_RUN
Text Notes 10400 1900 0    50   ~ 0
0.1uf = 100nf = 104
Text Notes 10400 2050 0    50   ~ 0
2.2uf =2200nf = 225
Text Notes 10050 1700 0    50   ~ 0
all smts caps are 0603 package size except where noted
Text Notes 13850 6300 0    50   ~ 0
0402 package size
Text Notes 11800 2500 0    50   ~ 0
1206 package size
Text Notes 9800 4450 0    50   ~ 0
0403 package size
Text Notes 10050 1600 0    50   ~ 0
all smt resistors are 0402 package size
$Comp
L Device:LED D1
U 1 1 669C8EA0
P 5900 3500
F 0 "D1" H 5893 3245 50  0000 C CNN
F 1 "LED" H 5893 3336 50  0000 C CNN
F 2 "LED_SMD:LED_1206_3216Metric" H 5900 3500 50  0001 C CNN
F 3 "~" H 5900 3500 50  0001 C CNN
	1    5900 3500
	-1   0    0    1   
$EndComp
$Comp
L Device:R R5
U 1 1 669CD380
P 5600 3500
F 0 "R5" V 5500 3450 50  0000 L CNN
F 1 "2.2k" V 5600 3500 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5530 3500 50  0001 C CNN
F 3 "~" H 5600 3500 50  0001 C CNN
	1    5600 3500
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 669D3326
P 6050 3500
F 0 "#PWR0106" H 6050 3250 50  0001 C CNN
F 1 "GND" H 6055 3327 50  0001 C CNN
F 2 "" H 6050 3500 50  0001 C CNN
F 3 "" H 6050 3500 50  0001 C CNN
	1    6050 3500
	0    -1   -1   0   
$EndComp
Text GLabel 5450 3500 0    50   Input ~ 0
I2S_RX_ACTIVE
Text GLabel 5450 3900 0    50   Input ~ 0
I2S_TX_ACTIVE
$Comp
L Device:LED D2
U 1 1 669EC89D
P 5900 3900
F 0 "D2" H 5893 3645 50  0000 C CNN
F 1 "LED" H 5893 3736 50  0000 C CNN
F 2 "LED_SMD:LED_1206_3216Metric" H 5900 3900 50  0001 C CNN
F 3 "~" H 5900 3900 50  0001 C CNN
	1    5900 3900
	-1   0    0    1   
$EndComp
$Comp
L Device:R R6
U 1 1 669EC8A3
P 5600 3900
F 0 "R6" V 5500 3850 50  0000 L CNN
F 1 "2.2k" V 5600 3900 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5530 3900 50  0001 C CNN
F 3 "~" H 5600 3900 50  0001 C CNN
	1    5600 3900
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 669EC8A9
P 6050 3900
F 0 "#PWR0107" H 6050 3650 50  0001 C CNN
F 1 "GND" H 6055 3727 50  0001 C CNN
F 2 "" H 6050 3900 50  0001 C CNN
F 3 "" H 6050 3900 50  0001 C CNN
	1    6050 3900
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
