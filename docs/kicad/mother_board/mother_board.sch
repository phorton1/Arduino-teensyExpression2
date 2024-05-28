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
Text GLabel 10850 3350 3    50   Input ~ 0
T4_5V
$Comp
L power:GND #PWR01
U 1 1 6663CA4B
P 11550 3350
F 0 "#PWR01" H 11550 3100 50  0001 C CNN
F 1 "GND" V 11550 3150 50  0000 C CNN
F 2 "" H 11550 3350 50  0001 C CNN
F 3 "" H 11550 3350 50  0001 C CNN
	1    11550 3350
	1    0    0    -1  
$EndComp
Text GLabel 11350 3350 3    50   Input ~ 0
T4_MOSI
Text GLabel 11250 3350 3    50   Input ~ 0
T4_MISO
Text GLabel 11450 3350 3    50   Input ~ 0
T4_SCLK
Text GLabel 11150 3350 3    50   Input ~ 0
T4_CD
Text GLabel 10950 3350 3    50   Input ~ 0
T4_LED_CS
Text GLabel 11050 3350 3    50   Input ~ 0
T4_TOUCH_CS
Text GLabel 10800 4900 1    50   Output ~ 0
T4_SCLK
Text GLabel 9700 4900 1    50   Input ~ 0
T4_3V
Text GLabel 9500 4900 1    50   Input ~ 0
T4_5V
Text GLabel 10500 7000 3    50   Output ~ 0
T4_CD
Text GLabel 10600 7000 3    50   Output ~ 0
T4_LED_CS
$Comp
L power:GND #PWR09
U 1 1 66399473
P 9600 4900
F 0 "#PWR09" H 9600 4650 50  0001 C CNN
F 1 "GND" V 9600 4700 50  0000 C CNN
F 2 "" H 9600 4900 50  0001 C CNN
F 3 "" H 9600 4900 50  0001 C CNN
	1    9600 4900
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR010
U 1 1 66399467
P 12050 13500
F 0 "#PWR010" H 12050 13250 50  0001 C CNN
F 1 "GND" V 12050 13300 50  0000 C CNN
F 2 "" H 12050 13500 50  0001 C CNN
F 3 "" H 12050 13500 50  0001 C CNN
	1    12050 13500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR021
U 1 1 6637C7F0
P 9500 7000
F 0 "#PWR021" H 9500 6750 50  0001 C CNN
F 1 "GND" V 9500 6800 50  0000 C CNN
F 2 "" H 9500 7000 50  0001 C CNN
F 3 "" H 9500 7000 50  0001 C CNN
	1    9500 7000
	1    0    0    -1  
$EndComp
Text GLabel 10700 7000 3    50   Output ~ 0
T4_MOSI
Text GLabel 10800 7000 3    50   Output ~ 0
T4_MISO
$Comp
L Device:R R5
U 1 1 64CCBE5B
P 6600 7000
F 0 "R5" H 6670 7046 50  0000 L CNN
F 1 "220" V 6600 7000 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 6530 7000 50  0001 C CNN
F 3 "~" H 6600 7000 50  0001 C CNN
	1    6600 7000
	0    -1   1    0   
$EndComp
Text GLabel 7000 7000 2    50   Input ~ 10
RPI_RUN
Text GLabel 6300 7000 0    50   Input ~ 0
SENS_PI_RUN
$Comp
L Transistor_BJT:BC547 Q1
U 1 1 64CF1712
P 6800 6700
F 0 "Q1" H 6991 6746 50  0000 L CNN
F 1 "BC547" H 6991 6655 50  0000 L CNN
F 2 "0_my_footprints2:transistor" H 7000 6625 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC547.pdf" H 6800 6700 50  0001 L CNN
	1    6800 6700
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR015
U 1 1 64CF464E
P 6900 6400
F 0 "#PWR015" H 6900 6150 50  0001 C CNN
F 1 "GND" H 6905 6227 50  0000 C CNN
F 2 "" H 6900 6400 50  0001 C CNN
F 3 "" H 6900 6400 50  0001 C CNN
	1    6900 6400
	1    0    0    1   
$EndComp
Text GLabel 6300 6700 0    50   Input ~ 0
PI_REBOOT
Wire Wire Line
	6750 7000 6900 7000
Wire Wire Line
	6900 6900 6900 7000
Connection ~ 6900 7000
Wire Wire Line
	6900 7000 7000 7000
Wire Wire Line
	6900 6500 6900 6400
$Comp
L Device:LED D2
U 1 1 662B77AA
P 6500 5400
F 0 "D2" H 6493 5145 50  0000 C CNN
F 1 "BLUE" H 6493 5236 50  0000 C CNN
F 2 "0_my_footprints2:led_blue" H 6500 5400 50  0001 C CNN
F 3 "~" H 6500 5400 50  0001 C CNN
	1    6500 5400
	-1   0    0    1   
$EndComp
$Comp
L Device:R R2
U 1 1 662B77B0
P 6800 5400
F 0 "R2" V 6950 5350 50  0000 L CNN
F 1 "560" V 6800 5400 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 6730 5400 50  0001 C CNN
F 3 "~" H 6800 5400 50  0001 C CNN
	1    6800 5400
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR05
U 1 1 662B77B6
P 6950 5400
F 0 "#PWR05" H 6950 5150 50  0001 C CNN
F 1 "GND" V 6950 5200 50  0000 C CNN
F 2 "" H 6950 5400 50  0001 C CNN
F 3 "" H 6950 5400 50  0001 C CNN
	1    6950 5400
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D1
U 1 1 662BFCA9
P 6500 4900
F 0 "D1" H 6493 4645 50  0000 C CNN
F 1 "GREEN" H 6493 4736 50  0000 C CNN
F 2 "0_my_footprints2:led_green" H 6500 4900 50  0001 C CNN
F 3 "~" H 6500 4900 50  0001 C CNN
	1    6500 4900
	-1   0    0    1   
$EndComp
$Comp
L Device:R R1
U 1 1 662BFCAF
P 6800 4900
F 0 "R1" V 6950 4850 50  0000 L CNN
F 1 "220" V 6800 4900 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 6730 4900 50  0001 C CNN
F 3 "~" H 6800 4900 50  0001 C CNN
	1    6800 4900
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR02
U 1 1 662BFCB5
P 6950 4900
F 0 "#PWR02" H 6950 4650 50  0001 C CNN
F 1 "GND" V 6950 4700 50  0000 C CNN
F 2 "" H 6950 4900 50  0001 C CNN
F 3 "" H 6950 4900 50  0001 C CNN
	1    6950 4900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6300 6700 6600 6700
Wire Wire Line
	6300 7000 6450 7000
Text GLabel 6350 5400 0    50   Input ~ 0
LED_PI_READY
Text GLabel 6350 4900 0    50   Input ~ 0
LED_PI_RUN
Text GLabel 3400 5650 1    50   Input ~ 0
RPI_READY
Text GLabel 9600 7000 3    50   Input ~ 0
T4_RX1
Text GLabel 9700 7000 3    50   Input ~ 0
T4_TX1
Text GLabel 10200 7000 3    50   Input ~ 0
BTN_CLK
Text GLabel 10300 7000 3    50   Input ~ 0
BTN_DOUT
Text GLabel 10100 7000 3    50   Input ~ 0
BTN_SENSE
Text GLabel 10300 4900 1    50   Input ~ 0
T4_SDA
Text GLabel 10200 4900 1    50   Input ~ 0
T4_SCL
$Comp
L 0_my_teensy:myTeensy4.0_wProgPin U?
U 1 1 665516E9
P 10050 5900
F 0 "U?" H 9373 5903 60  0001 R CNN
F 1 "myTeensy4.0_wProgPin" H 10550 5600 60  0000 R CNN
F 2 "" V 10600 5900 60  0000 C CNN
F 3 "" V 10600 5900 60  0000 C CNN
	1    10050 5900
	1    0    0    -1  
$EndComp
Text GLabel 9800 4900 1    50   UnSpc ~ 0
EXPR1
Text GLabel 9900 4900 1    50   UnSpc ~ 0
EXPR2
Text GLabel 10000 4900 1    50   UnSpc ~ 0
EXPR3
Text GLabel 10100 4900 1    50   UnSpc ~ 0
EXPR4
Text GLabel 10700 4900 1    50   Output ~ 0
T4_TOUCH_CS
$Comp
L 0_my_symbols:ESP32_DEV U?
U 1 1 66564A96
P 3100 6200
F 0 "U?" H 4228 6296 50  0000 L CNN
F 1 "ESP32_DEV" H 4228 6205 50  0000 L CNN
F 2 "" H 2750 6300 50  0001 C CNN
F 3 "" H 2750 6300 50  0001 C CNN
	1    3100 6200
	1    0    0    -1  
$EndComp
Text GLabel 3100 5650 1    50   Input ~ 0
T4_RX1
Text GLabel 3200 5650 1    50   Input ~ 0
T4_TX1
Text GLabel 2700 6650 3    50   Input ~ 0
LED_ALIVE
Text GLabel 3900 6650 3    50   Input ~ 0
LED_PI_READY
Text GLabel 3600 6650 3    50   Input ~ 0
LED_PI_RUN
Text GLabel 3000 6650 3    50   Input ~ 0
RPI_TX
Text GLabel 3100 6650 3    50   Input ~ 0
RPI_RX
Text GLabel 3600 5650 1    50   Input ~ 0
SENS_PI_RUN
Text GLabel 3500 5650 1    50   Input ~ 0
PI_REBOOT
$Comp
L Connector:AudioJack3_Switch J?
U 1 1 665B19A3
P 9150 13000
F 0 "J?" H 8870 12908 50  0001 R CNN
F 1 "EXPR1" H 8870 12954 50  0000 R CNN
F 2 "" H 9150 13000 50  0001 C CNN
F 3 "~" H 9150 13000 50  0001 C CNN
	1    9150 13000
	0    1    1    0   
$EndComp
$Comp
L Connector:AudioJack3_Switch J?
U 1 1 665B7A81
P 9950 13000
F 0 "J?" H 9670 12908 50  0001 R CNN
F 1 "EXPR1" H 9670 12954 50  0000 R CNN
F 2 "" H 9950 13000 50  0001 C CNN
F 3 "~" H 9950 13000 50  0001 C CNN
	1    9950 13000
	0    1    1    0   
$EndComp
$Comp
L Connector:AudioJack3_Switch J?
U 1 1 665B8EFA
P 10750 13000
F 0 "J?" H 10470 12908 50  0001 R CNN
F 1 "EXPR1" H 10470 12954 50  0000 R CNN
F 2 "" H 10750 13000 50  0001 C CNN
F 3 "~" H 10750 13000 50  0001 C CNN
	1    10750 13000
	0    1    1    0   
$EndComp
$Comp
L Connector:AudioJack3_Switch J?
U 1 1 665B945E
P 11550 13000
F 0 "J?" H 11270 12908 50  0001 R CNN
F 1 "EXPR1" H 11270 12954 50  0000 R CNN
F 2 "" H 11550 13000 50  0001 C CNN
F 3 "~" H 11550 13000 50  0001 C CNN
	1    11550 13000
	0    1    1    0   
$EndComp
Text GLabel 8500 12950 1    50   Input ~ 0
T4_3V
$Comp
L Device:R R?
U 1 1 665E659E
P 8500 13200
F 0 "R?" V 8650 13150 50  0001 L CNN
F 1 "1K" V 8500 13200 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 8430 13200 50  0001 C CNN
F 3 "~" H 8500 13200 50  0001 C CNN
	1    8500 13200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 13200 8850 13350
Wire Wire Line
	8850 13350 9350 13350
Wire Wire Line
	9350 13200 9350 13350
Connection ~ 9350 13350
Wire Wire Line
	9350 13350 9650 13350
Wire Wire Line
	9650 13200 9650 13350
Connection ~ 9650 13350
Wire Wire Line
	9650 13350 10150 13350
Wire Wire Line
	10150 13200 10150 13350
Connection ~ 10150 13350
Wire Wire Line
	10150 13350 10450 13350
Wire Wire Line
	10450 13200 10450 13350
Connection ~ 10450 13350
Wire Wire Line
	10450 13350 10950 13350
Wire Wire Line
	10950 13200 10950 13350
Connection ~ 10950 13350
Wire Wire Line
	10950 13350 11250 13350
Wire Wire Line
	11250 13200 11250 13350
Connection ~ 11250 13350
Wire Wire Line
	11250 13350 11750 13350
Wire Wire Line
	11750 13200 11750 13350
Connection ~ 11750 13350
Wire Wire Line
	11750 13350 12050 13350
Wire Wire Line
	9150 13200 9150 13500
Wire Wire Line
	9150 13500 9950 13500
Wire Wire Line
	11550 13500 11550 13200
Wire Wire Line
	9950 13200 9950 13500
Connection ~ 9950 13500
Wire Wire Line
	9950 13500 10750 13500
Wire Wire Line
	10750 13200 10750 13500
Connection ~ 10750 13500
Wire Wire Line
	10750 13500 11550 13500
Connection ~ 9150 13500
Wire Wire Line
	8500 12950 8500 13050
Wire Wire Line
	8950 13200 8950 13850
Wire Wire Line
	9750 13200 9750 13850
Wire Wire Line
	10550 13200 10550 13850
Wire Wire Line
	11350 13200 11350 13850
Text GLabel 8950 13850 3    50   Input ~ 0
EXPR1
Text GLabel 9750 13850 3    50   Input ~ 0
EXPR2
Text GLabel 10550 13850 3    50   Input ~ 0
EXPR3
Text GLabel 11350 13850 3    50   Input ~ 0
EXPR4
Text GLabel 10400 4900 1    50   Input ~ 0
ROT_INT
$Comp
L Connector_Generic:Conn_01x08 J1
U 1 1 6663CA44
P 11150 3150
F 0 "J1" V 11150 3700 50  0001 R CNN
F 1 "T4_ILI9488" V 11300 3300 50  0000 R CNN
F 2 "0_my_footprints2:JST8" H 11150 3150 50  0001 C CNN
F 3 "~" H 11150 3150 50  0001 C CNN
	1    11150 3150
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J?
U 1 1 6663FACB
P 10200 8950
F 0 "J?" V 10072 9230 50  0001 L CNN
F 1 "BTN_BOARDS" V 10350 8700 50  0000 L CNN
F 2 "" H 10200 8950 50  0001 C CNN
F 3 "~" H 10200 8950 50  0001 C CNN
	1    10200 8950
	0    1    1    0   
$EndComp
Text GLabel 10400 7000 3    50   Input ~ 0
LED_DOUT
Text GLabel 10100 8750 1    50   Input ~ 0
BTN_CLK
Text GLabel 10200 8750 1    50   Input ~ 0
BTN_DOUT
Text GLabel 10000 8750 1    50   Input ~ 0
BTN_SENSE
Text GLabel 10300 8750 1    50   Input ~ 0
LED_DOUT
Text GLabel 10400 8750 1    50   Input ~ 0
T4_5V
$Comp
L power:GND #PWR?
U 1 1 6665BA7E
P 9900 8750
F 0 "#PWR?" H 9900 8500 50  0001 C CNN
F 1 "GND" V 9900 8550 50  0000 C CNN
F 2 "" H 9900 8750 50  0001 C CNN
F 3 "" H 9900 8750 50  0001 C CNN
	1    9900 8750
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J?
U 1 1 66669906
P 9950 3150
F 0 "J?" V 9914 2862 50  0001 R CNN
F 1 "ROTARY" V 10100 3300 50  0000 R CNN
F 2 "" H 9950 3150 50  0001 C CNN
F 3 "~" H 9950 3150 50  0001 C CNN
	1    9950 3150
	0    -1   -1   0   
$EndComp
Text GLabel 10150 3350 3    50   Input ~ 0
T4_3V
Text GLabel 10050 3350 3    50   Input ~ 0
ROT_INT
Text GLabel 9950 3350 3    50   Input ~ 0
T4_SDA
Text GLabel 9850 3350 3    50   Input ~ 0
T4_SCL
$Comp
L power:GND #PWR?
U 1 1 66692FA2
P 9750 3350
F 0 "#PWR?" H 9750 3100 50  0001 C CNN
F 1 "GND" V 9750 3150 50  0000 C CNN
F 2 "" H 9750 3350 50  0001 C CNN
F 3 "" H 9750 3350 50  0001 C CNN
	1    9750 3350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 66610187
P 15300 6400
F 0 "#PWR013" H 15300 6150 50  0001 C CNN
F 1 "GND" V 15300 6200 50  0000 C CNN
F 2 "" H 15300 6400 50  0001 C CNN
F 3 "" H 15300 6400 50  0001 C CNN
	1    15300 6400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 6660F832
P 15700 6400
F 0 "#PWR014" H 15700 6150 50  0001 C CNN
F 1 "GND" V 15700 6200 50  0000 C CNN
F 2 "" H 15700 6400 50  0001 C CNN
F 3 "" H 15700 6400 50  0001 C CNN
	1    15700 6400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 6660E5AB
P 15500 5200
F 0 "#PWR08" H 15500 4950 50  0001 C CNN
F 1 "GND" V 15500 5000 50  0000 C CNN
F 2 "" H 15500 5200 50  0001 C CNN
F 3 "" H 15500 5200 50  0001 C CNN
	1    15500 5200
	-1   0    0    1   
$EndComp
Text GLabel 15400 5200 1    50   Input Italic 0
RPI_CD
Text GLabel 15600 6400 3    50   UnSpc ~ 0
RPI_TX
Text GLabel 15500 6400 3    50   UnSpc ~ 0
RPI_RX
Text GLabel 15400 6400 3    50   Output ~ 10
I2S_BCLK
Text GLabel 14250 7200 0    50   Input ~ 0
RPI_I2S_TX_ACTIVE
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 66503904
P 13050 4700
F 0 "J2" H 13000 4850 50  0001 L CNN
F 1 "RPI_RUN" H 13130 4651 50  0001 L CNN
F 2 "0_my_footprints2:Oval1Pin" H 13050 4700 50  0001 C CNN
F 3 "~" H 13050 4700 50  0001 C CNN
	1    13050 4700
	1    0    0    -1  
$EndComp
Text GLabel 12850 4700 0    50   UnSpc ~ 0
RPI_RUN
Text GLabel 14200 5200 1    50   Output ~ 10
I2S_FCLK
Text GLabel 14100 6400 3    50   Output ~ 10
I2S_RX
Text GLabel 14000 6400 3    50   Output ~ 10
I2S_TX
Text GLabel 15000 5200 1    50   Input ~ 0
RPI_MOSI
Text GLabel 14900 5200 1    50   Input ~ 0
RPI_MISO
Text GLabel 14800 5200 1    50   Input ~ 0
RPI_SCLK
Text GLabel 14800 6400 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 14700 6400 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 14900 6400 3    50   UnSpc ~ 0
RPI_READY
Text GLabel 15000 7200 2    50   Input ~ 0
RPI_I2S_RX_ACTIVE
$Comp
L power:GND #PWR07
U 1 1 6660E9EA
P 14700 5200
F 0 "#PWR07" H 14700 4950 50  0001 C CNN
F 1 "GND" V 14700 5000 50  0000 C CNN
F 2 "" H 14700 5200 50  0001 C CNN
F 3 "" H 14700 5200 50  0001 C CNN
	1    14700 5200
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 6660EE40
P 14000 5200
F 0 "#PWR06" H 14000 4950 50  0001 C CNN
F 1 "GND" V 14000 5000 50  0000 C CNN
F 2 "" H 14000 5200 50  0001 C CNN
F 3 "" H 14000 5200 50  0001 C CNN
	1    14000 5200
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR012
U 1 1 666109AB
P 15000 6400
F 0 "#PWR012" H 15000 6150 50  0001 C CNN
F 1 "GND" V 15000 6200 50  0000 C CNN
F 2 "" H 15000 6400 50  0001 C CNN
F 3 "" H 15000 6400 50  0001 C CNN
	1    15000 6400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 666110E8
P 14500 6400
F 0 "#PWR011" H 14500 6150 50  0001 C CNN
F 1 "GND" V 14500 6200 50  0000 C CNN
F 2 "" H 14500 6400 50  0001 C CNN
F 3 "" H 14500 6400 50  0001 C CNN
	1    14500 6400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 66611616
P 14300 6400
F 0 "#PWR010" H 14300 6150 50  0001 C CNN
F 1 "GND" V 14300 6200 50  0000 C CNN
F 2 "" H 14300 6400 50  0001 C CNN
F 3 "" H 14300 6400 50  0001 C CNN
	1    14300 6400
	1    0    0    -1  
$EndComp
$Comp
L 0_my_symbols:rpiA J4
U 1 1 66521D2F
P 14950 6050
F 0 "J4" H 15000 6200 50  0001 C CNN
F 1 "rpiA" H 15050 6200 50  0000 C CNN
F 2 "0_my_footprints2:pinSocket1x20_bottom" H 14800 5900 50  0001 C CNN
F 3 "" H 14800 5900 50  0001 C CNN
	1    14950 6050
	1    0    0    -1  
$EndComp
$Comp
L 0_my_symbols:rpiB J3
U 1 1 66525118
P 14950 5550
F 0 "J3" H 15150 5500 50  0001 C CNN
F 1 "rpiB" H 15300 5500 50  0000 C CNN
F 2 "0_my_footprints2:pinSocket1x20_bottom" H 14850 5150 50  0001 C CNN
F 3 "" H 14850 5150 50  0001 C CNN
	1    14950 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 6645117F
P 15200 3350
F 0 "#PWR03" H 15200 3100 50  0001 C CNN
F 1 "GND" V 15200 3150 50  0000 C CNN
F 2 "" H 15200 3350 50  0001 C CNN
F 3 "" H 15200 3350 50  0001 C CNN
	1    15200 3350
	-1   0    0    -1  
$EndComp
Text GLabel 14900 3350 3    50   Input ~ 0
RPI_MOSI
Text GLabel 15000 3350 3    50   Input ~ 0
RPI_MISO
Text GLabel 15100 3350 3    50   Input ~ 0
RPI_SCLK
Text GLabel 14600 3350 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 14700 3350 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 14800 3350 3    50   Input ~ 0
RPI_CD
$Comp
L Device:LED D1
U 1 1 6647603F
P 14400 7700
F 0 "D1" H 14393 7445 50  0001 C CNN
F 1 "yellow" H 14393 7537 50  0000 C CNN
F 2 "0_my_footprints2:led_yellow" H 14400 7700 50  0001 C CNN
F 3 "~" H 14400 7700 50  0001 C CNN
	1    14400 7700
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 66478725
P 14400 7400
F 0 "R1" V 14193 7400 50  0001 C CNN
F 1 "220" V 14400 7400 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 14330 7400 50  0001 C CNN
F 3 "~" H 14400 7400 50  0001 C CNN
	1    14400 7400
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D2
U 1 1 66485268
P 14900 7700
F 0 "D2" H 14893 7445 50  0001 C CNN
F 1 "yellow" H 14893 7537 50  0000 C CNN
F 2 "0_my_footprints2:led_yellow" H 14900 7700 50  0001 C CNN
F 3 "~" H 14900 7700 50  0001 C CNN
	1    14900 7700
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 6648526E
P 14900 7400
F 0 "R2" V 14693 7400 50  0001 C CNN
F 1 "220" V 14900 7400 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 14830 7400 50  0001 C CNN
F 3 "~" H 14900 7400 50  0001 C CNN
	1    14900 7400
	-1   0    0    1   
$EndComp
Wire Wire Line
	14900 7850 14900 7950
Wire Wire Line
	14400 7850 14400 7950
Text GLabel 15800 5200 1    50   Output ~ 10
RPI_SDA
Text GLabel 15700 5200 1    50   Output ~ 10
RPI_SCL
Text GLabel 15600 5200 1    50   Output ~ 10
I2S_MCLK
Wire Wire Line
	15000 7200 14900 7200
Wire Wire Line
	14900 7200 14900 7250
Wire Wire Line
	14250 7200 14400 7200
Wire Wire Line
	14400 7200 14400 7250
Wire Wire Line
	14400 7950 14650 7950
$Comp
L power:GND #PWR018
U 1 1 66562CCD
P 14650 8050
F 0 "#PWR018" H 14650 7800 50  0001 C CNN
F 1 "GND" V 14650 7850 50  0000 C CNN
F 2 "" H 14650 8050 50  0001 C CNN
F 3 "" H 14650 8050 50  0001 C CNN
	1    14650 8050
	1    0    0    -1  
$EndComp
Wire Wire Line
	14650 8050 14650 7950
Connection ~ 14650 7950
Wire Wire Line
	14650 7950 14900 7950
$Comp
L Connector_Generic:Conn_01x08 J1
U 1 1 665749E8
P 14900 3150
F 0 "J1" V 14864 3530 50  0001 L CNN
F 1 "RPI_ILI9488" V 15050 3000 50  0000 L CNN
F 2 "0_my_footprints:myJSTx08" H 14900 3150 50  0001 C CNN
F 3 "~" H 14900 3150 50  0001 C CNN
	1    14900 3150
	0    1    -1   0   
$EndComp
Wire Wire Line
	8500 13500 8500 13350
Wire Wire Line
	8500 13500 9150 13500
Wire Wire Line
	12050 13350 12050 13500
Text GLabel 15900 5200 1    50   Output ~ 10
RPI_3V
Text GLabel 15800 6400 3    50   Input ~ 0
RPI_5V
Text GLabel 15900 6400 3    50   Input ~ 0
RPI_5V
Text GLabel 14500 3350 3    50   Input ~ 0
RPI_5V
$Comp
L Connector_Generic:Conn_01x06 J?
U 1 1 665B9DF3
P 8750 3150
F 0 "J?" V 8622 3430 50  0001 L CNN
F 1 "EXPR" V 8900 2900 50  0000 L CNN
F 2 "" H 8750 3150 50  0001 C CNN
F 3 "~" H 8750 3150 50  0001 C CNN
	1    8750 3150
	0    -1   -1   0   
$EndComp
Text GLabel 8550 3350 3    50   Input ~ 0
T4_3V
$Comp
L power:GND #PWR?
U 1 1 665B9DFE
P 9050 3350
F 0 "#PWR?" H 9050 3100 50  0001 C CNN
F 1 "GND" V 9050 3150 50  0000 C CNN
F 2 "" H 9050 3350 50  0001 C CNN
F 3 "" H 9050 3350 50  0001 C CNN
	1    9050 3350
	1    0    0    -1  
$EndComp
Text GLabel 8650 3350 3    50   Input ~ 0
EXPR1
Text GLabel 8750 3350 3    50   Input ~ 0
EXPR2
Text GLabel 8850 3350 3    50   Input ~ 0
EXPR3
Text GLabel 8950 3350 3    50   Input ~ 0
EXPR4
Text GLabel 17250 6200 1    50   Input ~ 0
I2S_FCLK
Text GLabel 17750 6200 1    50   Input ~ 0
RPI_SDA
Text GLabel 17650 6200 1    50   Input ~ 0
RPI_SCL
Text GLabel 18150 7650 3    50   Input ~ 0
I2S_MCLK
Text GLabel 17950 7650 3    50   Input ~ 0
I2S_BCLK
Text GLabel 18250 6200 1    50   Input ~ 0
I2S_RX
Text GLabel 17350 6200 1    50   Input ~ 0
I2S_TX
$Comp
L power:GND #PWR?
U 1 1 665731F1
P 17050 6200
F 0 "#PWR?" H 17050 5950 50  0001 C CNN
F 1 "GND" V 17050 6000 50  0000 C CNN
F 2 "" H 17050 6200 50  0001 C CNN
F 3 "" H 17050 6200 50  0001 C CNN
	1    17050 6200
	1    0    0    1   
$EndComp
Text GLabel 17150 6200 1    50   Input ~ 0
RPI_3V
$Comp
L 0_my_teensy:teensyAudioRevB U?
U 1 1 6655470A
P 17500 6900
F 0 "U?" H 18378 6928 60  0000 L CNN
F 1 "teensyAudioRevB" H 18378 6822 60  0000 L CNN
F 2 "" V 18450 6850 60  0000 C CNN
F 3 "" V 18450 6850 60  0000 C CNN
	1    17500 6900
	1    0    0    -1  
$EndComp
Text GLabel 17200 3750 1    50   Input ~ 0
RPI_3V
$Comp
L power:GND #PWR017
U 1 1 66504B0C
P 17000 5250
F 0 "#PWR017" H 17000 5000 50  0001 C CNN
F 1 "GND" V 17000 5050 50  0000 C CNN
F 2 "" H 17000 5250 50  0001 C CNN
F 3 "" H 17000 5250 50  0001 C CNN
	1    17000 5250
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 66504730
P 17100 3750
F 0 "#PWR04" H 17100 3500 50  0001 C CNN
F 1 "GND" V 17100 3550 50  0000 C CNN
F 2 "" H 17100 3750 50  0001 C CNN
F 3 "" H 17100 3750 50  0001 C CNN
	1    17100 3750
	1    0    0    1   
$EndComp
Text GLabel 17500 3750 1    50   Input ~ 0
I2S_BCLK
Text GLabel 17800 5200 3    50   Input ~ 0
I2S_TX
Text GLabel 17900 5200 3    50   Input ~ 0
I2S_RX
Text GLabel 17300 3750 1    50   Input ~ 0
I2S_MCLK
Text GLabel 17700 3750 1    50   Input ~ 0
RPI_SCL
Text GLabel 17800 3750 1    50   Input ~ 0
RPI_SDA
Text GLabel 17600 3750 1    50   Input ~ 0
I2S_FCLK
$Comp
L 0_my_teensy:teensyAudioRevD U1
U 1 1 664E10BD
P 17550 4450
F 0 "U1" H 18428 4478 60  0000 L CNN
F 1 "teensyAudioRevD" H 18428 4372 60  0000 L CNN
F 2 "0_my_teensy:audioShieldRevD" V 18500 4400 60  0001 C CNN
F 3 "" V 18500 4400 60  0000 C CNN
	1    17550 4450
	1    0    0    -1  
$EndComp
$EndSCHEMATC
