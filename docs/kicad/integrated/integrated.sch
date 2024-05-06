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
L Connector_Generic:Conn_01x05 J6
U 1 1 64C5D02E
P 13300 1600
F 0 "J6" V 13550 1550 50  0000 L CNN
F 1 "ROTARY12" V 13450 1400 50  0000 L CNN
F 2 "0_my_footprints:myPinSocket_1x05" H 13300 1600 50  0001 C CNN
F 3 "~" H 13300 1600 50  0001 C CNN
	1    13300 1600
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J1
U 1 1 64C62D8F
P 11200 1600
F 0 "J1" V 11450 1600 50  0000 R CNN
F 1 "EXPR" V 11350 1650 50  0000 R CNN
F 2 "0_my_footprints:myPinSocket_1x06" H 11200 1600 50  0001 C CNN
F 3 "~" H 11200 1600 50  0001 C CNN
	1    11200 1600
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J7
U 1 1 64C75495
P 13900 1600
F 0 "J7" V 14150 1550 50  0000 L CNN
F 1 "ROTARY34" V 14050 1400 50  0000 L CNN
F 2 "0_my_footprints:myPinSocket_1x05" H 13900 1600 50  0001 C CNN
F 3 "~" H 13900 1600 50  0001 C CNN
	1    13900 1600
	0    -1   -1   0   
$EndComp
$Comp
L power:+3V3 #PWR016
U 1 1 64C9FDB5
P 14100 1800
F 0 "#PWR016" H 14100 1650 50  0001 C CNN
F 1 "+3V3" V 14100 2000 50  0000 C CNN
F 2 "" H 14100 1800 50  0001 C CNN
F 3 "" H 14100 1800 50  0001 C CNN
	1    14100 1800
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR015
U 1 1 64CA12B5
P 13500 1800
F 0 "#PWR015" H 13500 1550 50  0001 C CNN
F 1 "GND" V 13500 1600 50  0000 C CNN
F 2 "" H 13500 1800 50  0001 C CNN
F 3 "" H 13500 1800 50  0001 C CNN
	1    13500 1800
	1    0    0    -1  
$EndComp
Text GLabel 13100 1800 3    50   Input ~ 0
ROT1A
Text GLabel 13200 1800 3    50   Input ~ 0
ROT1B
Text GLabel 13400 1800 3    50   Input ~ 0
ROT2B
Text GLabel 13300 1800 3    50   Input ~ 0
ROT2A
Text GLabel 13700 1800 3    50   Input ~ 0
ROT3A
Text GLabel 13800 1800 3    50   Input ~ 0
ROT3B
Text GLabel 14000 1800 3    50   Input ~ 0
ROT4B
Text GLabel 13900 1800 3    50   Input ~ 0
ROT4A
$Comp
L power:GND #PWR01
U 1 1 64CFCC09
P 11500 1800
F 0 "#PWR01" H 11500 1550 50  0001 C CNN
F 1 "GND" V 11500 1600 50  0000 C CNN
F 2 "" H 11500 1800 50  0001 C CNN
F 3 "" H 11500 1800 50  0001 C CNN
	1    11500 1800
	-1   0    0    -1  
$EndComp
Text GLabel 11300 1800 3    50   Input ~ 0
EXPR1
Text GLabel 11200 1800 3    50   Input ~ 0
EXPR2
Text GLabel 11100 1800 3    50   Input ~ 0
EXPR3
Text GLabel 11000 1800 3    50   Input ~ 0
EXPR4
Text GLabel 10300 1850 1    50   Input ~ 0
EXPR1
Text GLabel 10400 1850 1    50   Input ~ 0
EXPR2
Text GLabel 10500 1850 1    50   Input ~ 0
EXPR3
Text GLabel 10600 1850 1    50   Input ~ 0
EXPR4
$Comp
L Device:R R2
U 1 1 64E4ADF9
P 10400 2000
F 0 "R2" V 10400 1450 50  0000 L CNN
F 1 "10K" V 10400 2000 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 10330 2000 50  0001 C CNN
F 3 "~" H 10400 2000 50  0001 C CNN
	1    10400 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 64E4B2A6
P 10500 2000
F 0 "R3" V 10500 1450 50  0000 L CNN
F 1 "10K" V 10500 2000 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 10430 2000 50  0001 C CNN
F 3 "~" H 10500 2000 50  0001 C CNN
	1    10500 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 64E4B7AB
P 10600 2000
F 0 "R4" V 10600 1450 50  0000 L CNN
F 1 "10K" V 10600 2000 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 10530 2000 50  0001 C CNN
F 3 "~" H 10600 2000 50  0001 C CNN
	1    10600 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 64CFCDE5
P 10300 2000
F 0 "R1" V 10300 1450 50  0000 L CNN
F 1 "10K" V 10300 2000 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 10230 2000 50  0001 C CNN
F 3 "~" H 10300 2000 50  0001 C CNN
	1    10300 2000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 64E50A7C
P 10300 2150
F 0 "#PWR0101" H 10300 1900 50  0001 C CNN
F 1 "GND" V 10300 1950 50  0000 C CNN
F 2 "" H 10300 2150 50  0001 C CNN
F 3 "" H 10300 2150 50  0001 C CNN
	1    10300 2150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 64E50B86
P 10400 2150
F 0 "#PWR0102" H 10400 1900 50  0001 C CNN
F 1 "GND" V 10400 1950 50  0000 C CNN
F 2 "" H 10400 2150 50  0001 C CNN
F 3 "" H 10400 2150 50  0001 C CNN
	1    10400 2150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 64E50E20
P 10500 2150
F 0 "#PWR0103" H 10500 1900 50  0001 C CNN
F 1 "GND" V 10500 1950 50  0000 C CNN
F 2 "" H 10500 2150 50  0001 C CNN
F 3 "" H 10500 2150 50  0001 C CNN
	1    10500 2150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 64E50F3A
P 10600 2150
F 0 "#PWR0104" H 10600 1900 50  0001 C CNN
F 1 "GND" V 10600 1950 50  0000 C CNN
F 2 "" H 10600 2150 50  0001 C CNN
F 3 "" H 10600 2150 50  0001 C CNN
	1    10600 2150
	1    0    0    -1  
$EndComp
Text Notes 12600 2100 2    50   ~ 0
This 1K resistor is\nimportant so that\nthe 3V power is not \nshorted to ground when \ninserting pedal plugs!\n
$Comp
L Device:R R5
U 1 1 64E70350
P 11400 1950
F 0 "R5" V 11400 1900 50  0001 L CNN
F 1 "1K" V 11400 1950 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 11330 1950 50  0001 C CNN
F 3 "~" H 11400 1950 50  0001 C CNN
	1    11400 1950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 64CCBE5B
P 21350 4900
F 0 "R?" H 21420 4946 50  0000 L CNN
F 1 "220" V 21350 4900 50  0000 C CNN
F 2 "" V 21280 4900 50  0001 C CNN
F 3 "~" H 21350 4900 50  0001 C CNN
	1    21350 4900
	0    1    1    0   
$EndComp
Text GLabel 21500 4900 2    50   Input ~ 0
SENS_PI_RUN
$Comp
L Transistor_BJT:BC547 Q?
U 1 1 64CF1712
P 21150 4600
F 0 "Q?" H 21341 4646 50  0000 L CNN
F 1 "BC547" H 21341 4555 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 21350 4525 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC547.pdf" H 21150 4600 50  0001 L CNN
	1    21150 4600
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 64CF464E
P 21050 4300
F 0 "#PWR?" H 21050 4050 50  0001 C CNN
F 1 "GND" H 21055 4127 50  0000 C CNN
F 2 "" H 21050 4300 50  0001 C CNN
F 3 "" H 21050 4300 50  0001 C CNN
	1    21050 4300
	-1   0    0    1   
$EndComp
Text GLabel 21350 4600 2    50   Input ~ 0
PI_REBOOT
Wire Wire Line
	21200 4900 21050 4900
Wire Wire Line
	21050 4800 21050 4900
Connection ~ 21050 4900
Wire Wire Line
	21050 4900 20950 4900
Wire Wire Line
	21050 4400 21050 4300
Text GLabel 19650 5150 3    50   Input ~ 0
VUSB
Text GLabel 19850 5150 3    50   Input ~ 0
RX1
Text GLabel 19750 5150 3    50   Input ~ 0
TX1
$Comp
L power:GND #PWR?
U 1 1 64D08031
P 19950 5150
F 0 "#PWR?" H 19950 4900 50  0001 C CNN
F 1 "GND" V 19950 4950 50  0000 C CNN
F 2 "" H 19950 5150 50  0001 C CNN
F 3 "" H 19950 5150 50  0001 C CNN
	1    19950 5150
	1    0    0    -1  
$EndComp
Text GLabel 19550 5150 3    50   Input ~ 0
PI_RUN
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 64CE18FB
P 19650 4950
F 0 "J?" V 19650 5500 50  0000 R CNN
F 1 "RPI" V 19800 4950 50  0000 R CNN
F 2 "" H 19650 4950 50  0001 C CNN
F 3 "~" H 19650 4950 50  0001 C CNN
	1    19650 4950
	0    1    -1   0   
$EndComp
Text Notes 19900 4700 1    50   ~ 0
RPI TX PIN  8
Text Notes 20000 4700 1    50   ~ 0
RPI GND PIN  6
Text Notes 19800 4700 1    50   ~ 0
RPI RX PIN 10
Text Notes 19700 4700 1    50   ~ 0
RPI 5V PIN  2
Text Notes 19600 4700 1    50   ~ 0
RPI RUN PIN
Text Notes 19500 4700 1    50   ~ 0
RPI GPIOXX PIN TBD
Text Notes 19400 4700 1    50   ~ 0
RPI GPIO12 PIN 32
Text Notes 19300 4700 1    50   ~ 0
RPI GPIO16 PIN 36
Text GLabel 19250 5150 3    50   Input ~ 0
I2S_TX_LED
Text GLabel 19350 5150 3    50   Input ~ 0
I2S_RX_LED
$Comp
L Connector_Generic:Conn_01x06 J?
U 1 1 661B33A0
P 14350 3850
F 0 "J?" V 14550 3450 50  0000 L CNN
F 1 "row_boards" V 14550 3600 50  0000 L CNN
F 2 "" H 14350 3850 50  0001 C CNN
F 3 "~" H 14350 3850 50  0001 C CNN
	1    14350 3850
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 661B7D82
P 13800 3650
F 0 "#PWR?" H 13800 3400 50  0001 C CNN
F 1 "GND" V 13800 3450 50  0000 C CNN
F 2 "" H 13800 3650 50  0001 C CNN
F 3 "" H 13800 3650 50  0001 C CNN
	1    13800 3650
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 661BAF69
P 14550 3650
F 0 "#PWR?" H 14550 3500 50  0001 C CNN
F 1 "+5V" V 14550 3850 50  0000 C CNN
F 2 "" H 14550 3650 50  0001 C CNN
F 3 "" H 14550 3650 50  0001 C CNN
	1    14550 3650
	1    0    0    -1  
$EndComp
Text GLabel 14450 3650 1    50   Input ~ 0
LED_DIN
Text GLabel 14250 3650 1    50   Input ~ 0
SW_CLK
Text GLabel 14350 3650 1    50   Input ~ 0
SW_DIN
Text GLabel 14150 3050 1    50   Input ~ 0
SW_DOUT_5V
Text GLabel 13550 3350 0    50   Input ~ 0
SW_DOUT_3V
$Comp
L Device:R R?
U 1 1 661E14AF
P 13800 3500
F 0 "R?" V 13700 3450 50  0000 L CNN
F 1 "2K" V 13800 3500 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 13730 3500 50  0001 C CNN
F 3 "~" H 13800 3500 50  0001 C CNN
	1    13800 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 661E21C0
P 13800 3200
F 0 "R?" V 13700 3150 50  0000 L CNN
F 1 "1K" V 13800 3200 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 13730 3200 50  0001 C CNN
F 3 "~" H 13800 3200 50  0001 C CNN
	1    13800 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	14050 3650 13800 3650
Connection ~ 13800 3650
Wire Wire Line
	13550 3350 13800 3350
Connection ~ 13800 3350
Wire Wire Line
	14150 3650 14150 3050
Wire Wire Line
	14150 3050 13800 3050
$Comp
L 0_my_symbols:rpi_connector J?
U 1 1 6636D4F6
P 7400 5800
F 0 "J?" H 8428 5821 50  0000 L CNN
F 1 "rpi_connector" H 8428 5730 50  0000 L CNN
F 2 "" H 7300 5700 50  0001 C CNN
F 3 "" H 7300 5700 50  0001 C CNN
	1    7400 5800
	1    0    0    -1  
$EndComp
Text GLabel 11400 3650 1    50   Input ~ 0
ROT3A
Text GLabel 11300 3650 1    50   Input ~ 0
ROT3B
Text GLabel 11100 3650 1    50   Input ~ 0
ROT4B
Text GLabel 11200 3650 1    50   Input ~ 0
ROT4A
Text GLabel 12000 9300 3    50   Input ~ 0
T3_MISO
Text GLabel 11900 9300 3    50   Input ~ 0
T3_MOSI
Text GLabel 12700 9300 3    50   Input ~ 10
LED_DIN
Text GLabel 11500 3650 1    50   Input ~ 0
ROT2B
Text GLabel 11600 3650 1    50   Input ~ 0
ROT2A
Text GLabel 11700 3650 1    50   Input ~ 0
ROT1B
Text GLabel 11800 3650 1    50   Input ~ 0
ROT1A
$Comp
L power:+3V3 #PWR?
U 1 1 6637C7EA
P 12100 9300
F 0 "#PWR?" H 12100 9150 50  0001 C CNN
F 1 "+3V3" V 12100 9500 50  0000 C CNN
F 2 "" H 12100 9300 50  0001 C CNN
F 3 "" H 12100 9300 50  0001 C CNN
	1    12100 9300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6637C7F0
P 10700 9300
F 0 "#PWR?" H 10700 9050 50  0001 C CNN
F 1 "GND" V 10700 9100 50  0000 C CNN
F 2 "" H 10700 9300 50  0001 C CNN
F 3 "" H 10700 9300 50  0001 C CNN
	1    10700 9300
	1    0    0    -1  
$EndComp
Text GLabel 12000 7250 1    50   Input ~ 0
T3_SPI_SCLK
Text GLabel 12350 3650 1    50   Input ~ 0
SW_DOUT_3V
Text GLabel 12250 3650 1    50   Input ~ 0
SW_DIN
Text GLabel 12150 3650 1    50   Input ~ 0
SW_CLK
Text GLabel 10700 3650 1    50   Input ~ 0
EXPR4
Text GLabel 10600 3650 1    50   Input ~ 0
EXPR3
Text GLabel 10500 3650 1    50   Input ~ 0
EXPR2
Text GLabel 10400 3650 1    50   Input ~ 0
EXPR1
$Comp
L power:GND #PWR?
U 1 1 66399467
P 12100 7250
F 0 "#PWR?" H 12100 7000 50  0001 C CNN
F 1 "GND" V 12100 7050 50  0000 C CNN
F 2 "" H 12100 7250 50  0001 C CNN
F 3 "" H 12100 7250 50  0001 C CNN
	1    12100 7250
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66399473
P 10800 7250
F 0 "#PWR?" H 10800 7000 50  0001 C CNN
F 1 "GND" V 10800 7050 50  0000 C CNN
F 2 "" H 10800 7250 50  0001 C CNN
F 3 "" H 10800 7250 50  0001 C CNN
	1    10800 7250
	-1   0    0    1   
$EndComp
$Comp
L 0_my_teensy:teensyAudioRevB U?
U 1 1 663C0C59
P 7500 9200
F 0 "U?" H 8378 9228 60  0001 L CNN
F 1 "teensyAudioRevB" H 7950 9150 60  0000 R CNN
F 2 "" V 8450 9150 60  0000 C CNN
F 3 "" V 8450 9150 60  0000 C CNN
	1    7500 9200
	1    0    0    -1  
$EndComp
$Comp
L 0_my_teensy:myTeensy4.1 U?
U 1 1 6636A2E9
P 11800 8150
F 0 "U?" H 10223 8088 50  0001 R CNN
F 1 "myTeensy4.1" H 12300 8200 50  0000 R CNN
F 2 "" H 11500 7250 50  0001 C CNN
F 3 "" H 11500 7250 50  0001 C CNN
	1    11800 8150
	1    0    0    -1  
$EndComp
Text GLabel 11000 7250 1    50   Input ~ 0
MCLK1
Text GLabel 8150 10250 3    50   Input ~ 0
MCLK1
Text GLabel 11200 7250 1    50   Input ~ 0
BCLK1
Text GLabel 7950 9950 3    50   Input ~ 0
BCLK1
Text GLabel 11300 7250 1    50   Input ~ 0
LRCLK1
Text GLabel 7250 8500 1    50   Input ~ 0
LRCLK1
Text GLabel 11400 7250 1    50   Input ~ 0
I2C_SCL
Text GLabel 11500 7250 1    50   Input ~ 0
I2C_SCA
Text GLabel 7650 8500 1    50   Input ~ 0
I2C_SCL
Text GLabel 7750 8500 1    50   Input ~ 0
I2C_SCA
Text GLabel 8150 8500 1    50   Input ~ 0
I2C_SCLK
Text GLabel 11800 7250 1    50   Input ~ 0
I2C_SCLK
Text GLabel 8250 8500 1    50   Input ~ 0
I2A_RX1A
Text GLabel 7350 8500 1    50   Input ~ 0
I2S_TX1A
Text GLabel 11500 9300 3    50   Input ~ 0
I2S_TX1A
Text GLabel 11600 9300 3    50   Input ~ 0
I2S_RX1A
Text GLabel 7850 6500 3    50   Input ~ 0
BCLK1
Text GLabel 6650 5150 1    50   Input ~ 0
LRCLK1
Text GLabel 13000 9300 3    50   Input ~ 0
I2S_TX1B
Text GLabel 11400 9300 3    50   Input ~ 0
I2S_RX1B
Text GLabel 6550 6500 3    50   Input ~ 0
I2S_TX1B
Text GLabel 6450 6500 3    50   Input ~ 0
I2S_RX1B
Text GLabel 10900 9300 3    50   Input ~ 0
T41_TX
Text GLabel 10800 9300 3    50   Input ~ 0
T41_RX
Text GLabel 19450 5150 3    50   Input ~ 0
SENS_PI_READY
Text GLabel 19900 8850 3    50   Input ~ 0
SENS_PI_READY
Text GLabel 20600 8850 3    50   Input ~ 0
SENS_PI_RUN
Text GLabel 20700 8850 3    50   Input ~ 0
PI_REBOOT
$Comp
L power:GND #PWR?
U 1 1 64D0323E
P 19400 8850
F 0 "#PWR?" H 19400 8600 50  0001 C CNN
F 1 "GND" V 19400 8650 50  0000 C CNN
F 2 "" H 19400 8850 50  0001 C CNN
F 3 "" H 19400 8850 50  0001 C CNN
	1    19400 8850
	1    0    0    -1  
$EndComp
Text GLabel 19600 8850 3    50   Input ~ 0
RPI_RX
Text GLabel 19500 8850 3    50   Input ~ 0
RPI_TX
Text GLabel 19400 7400 1    50   Input ~ 0
T2_VIN
Text GLabel 20500 7400 1    50   Input ~ 0
LED_PI_RUN
Text GLabel 20400 8850 3    50   Input ~ 0
T41_TX
Text GLabel 20500 8850 3    50   Input ~ 0
T41_RX
Text GLabel 20950 4900 0    50   Input ~ 0
PI_RUN
Text GLabel 21000 8100 2    50   Input ~ 0
TEENSY_PROG
Text GLabel 20700 7400 1    50   Input ~ 0
ONBOARD_LED
Text GLabel 20600 7400 1    50   Input ~ 0
LED_PI_READY
Text GLabel 20000 11250 1    50   Input ~ 0
I2S_TX_LED
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 64CCC22C
P 20100 11450
F 0 "J?" V 20100 10850 50  0000 R CNN
F 1 "LED_BOARD" V 20250 11550 50  0000 R CNN
F 2 "" H 20100 11450 50  0001 C CNN
F 3 "~" H 20100 11450 50  0001 C CNN
	1    20100 11450
	0    -1   1    0   
$EndComp
Text GLabel 19800 11250 1    50   Input ~ 0
PI_RUN
Text GLabel 19900 11250 1    50   Input ~ 0
TEENSY_PROG
$Comp
L power:GND #PWR?
U 1 1 64D14989
P 20400 11250
F 0 "#PWR?" H 20400 11000 50  0001 C CNN
F 1 "GND" V 20400 11050 50  0000 C CNN
F 2 "" H 20400 11250 50  0001 C CNN
F 3 "" H 20400 11250 50  0001 C CNN
	1    20400 11250
	-1   0    0    1   
$EndComp
Text GLabel 20100 11250 1    50   Input ~ 0
I2S_RX_LED
Text GLabel 20200 11250 1    50   Input ~ 0
LED_PI_READY
Text GLabel 20300 11250 1    50   Input ~ 0
LED_PI_RUN
Text GLabel 20500 11250 1    50   Input ~ 0
ONBOARD_LED
$Comp
L 0_my_teensy:myTeensy3.2 U?
U 1 1 64CC40AE
P 19950 8100
F 0 "U?" H 20100 8100 60  0000 L CNN
F 1 "myTeensy3.2" H 20000 8250 60  0000 L CNN
F 2 "" H 19950 7350 60  0000 C CNN
F 3 "" H 19950 7350 60  0000 C CNN
	1    19950 8100
	1    0    0    -1  
$EndComp
Text GLabel 7950 6500 3    50   Input ~ 0
RPI_RX
Text GLabel 8050 6500 3    50   Input ~ 0
RPI_TX
Text GLabel 7450 5150 1    50   Input ~ 0
RPI_MOSI
Text GLabel 7350 5150 1    50   Input ~ 0
RPI_MISO
Text GLabel 7250 5150 1    50   Input ~ 0
RPI_SCLK
Text GLabel 8050 5150 1    50   Input ~ 0
RPI_CD
Text GLabel 7250 6500 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 7150 6500 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 6950 5150 1    50   Input ~ 10
OCTO_RESET
Text GLabel 7650 5150 1    50   Input ~ 10
OCTO_MULT1
Text GLabel 7750 5150 1    50   Input ~ 10
OCTO_MULT0
Text GLabel 7650 6500 3    50   Input ~ 10
OCTO_MULT2
Text GLabel 7550 6500 3    50   Input ~ 10
OCTO_MULT3
Text GLabel 7350 6500 3    50   Input ~ 0
RPI_READY
Text GLabel 6650 6500 3    50   Input ~ 0
RPI_I2S_TX_ACTIVE
Text GLabel 6850 6500 3    50   Input ~ 0
RPI_I2S_RX_ACTIVE
Text GLabel 7850 5150 1    50   Input ~ 10
UNUSED_DISPLAY_TOUCH_IRQ
Text GLabel 8250 5150 1    50   Input ~ 10
OCTO_SDA
Text GLabel 8150 5150 1    50   Input ~ 10
OCTO_SCL
Text Notes 4050 4450 0    50   ~ 10
GPIO25 RPI_READY conflicts with Blue rPi 3.5" display RESET
Text Notes 4050 4600 0    50   ~ 10
GPIO23 OCTO_MULT2 conflics with Blue rPi 2.5" display CD pin
Text GLabel 11800 9300 3    50   Input ~ 0
T3_LED_CS
Text GLabel 12500 9300 3    50   Input ~ 10
T3_TOUCH_CS
$Comp
L Device:R R?
U 1 1 6652E99E
P 8150 10100
F 0 "R?" H 8100 10750 50  0001 L CNN
F 1 "100" V 8150 10100 50  0000 C CNN
F 2 "" V 8080 10100 50  0001 C CNN
F 3 "~" H 8150 10100 50  0001 C CNN
	1    8150 10100
	-1   0    0    1   
$EndComp
Text Notes 12650 9900 0    50   ~ 10
LED_IN chosen using teensy w2812Serial alternate pin options
Text GLabel 11700 9300 3    50   Input ~ 0
T3_CD
Text Notes 11650 10500 0    50   ~ 10
ILI9488_T3 pins are assumed to match ILI9341_T3 pins
Text Notes 12450 10250 0    50   ~ 10
T3_TOUCH_CS chosen as "can use any digital pin" from ILI9341 page
Text Notes 4050 4300 0    50   ~ 10
BOLD pins are reserved for future potential use
Text Notes 11600 850  0    50   ~ 10
Would still need many more pins to handle\nbuttons, rotaries, and expression pedals
Text GLabel 10700 7250 1    50   Input ~ 0
T41_VIN
Text GLabel 8250 6500 3    50   Input ~ 0
RPI_5V
Text GLabel 8350 5150 1    50   Input ~ 0
RPI_3.3V
$Comp
L power:GND #PWR?
U 1 1 6660E5AB
P 7950 5150
F 0 "#PWR?" H 7950 4900 50  0001 C CNN
F 1 "GND" V 7950 4950 50  0000 C CNN
F 2 "" H 7950 5150 50  0001 C CNN
F 3 "" H 7950 5150 50  0001 C CNN
	1    7950 5150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6660E9EA
P 7150 5150
F 0 "#PWR?" H 7150 4900 50  0001 C CNN
F 1 "GND" V 7150 4950 50  0000 C CNN
F 2 "" H 7150 5150 50  0001 C CNN
F 3 "" H 7150 5150 50  0001 C CNN
	1    7150 5150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6660EE40
P 6450 5150
F 0 "#PWR?" H 6450 4900 50  0001 C CNN
F 1 "GND" V 6450 4950 50  0000 C CNN
F 2 "" H 6450 5150 50  0001 C CNN
F 3 "" H 6450 5150 50  0001 C CNN
	1    6450 5150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6660F832
P 8150 6500
F 0 "#PWR?" H 8150 6250 50  0001 C CNN
F 1 "GND" V 8150 6300 50  0000 C CNN
F 2 "" H 8150 6500 50  0001 C CNN
F 3 "" H 8150 6500 50  0001 C CNN
	1    8150 6500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66610187
P 7750 6500
F 0 "#PWR?" H 7750 6250 50  0001 C CNN
F 1 "GND" V 7750 6300 50  0000 C CNN
F 2 "" H 7750 6500 50  0001 C CNN
F 3 "" H 7750 6500 50  0001 C CNN
	1    7750 6500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 666109AB
P 7450 6500
F 0 "#PWR?" H 7450 6250 50  0001 C CNN
F 1 "GND" V 7450 6300 50  0000 C CNN
F 2 "" H 7450 6500 50  0001 C CNN
F 3 "" H 7450 6500 50  0001 C CNN
	1    7450 6500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 666110E8
P 6950 6500
F 0 "#PWR?" H 6950 6250 50  0001 C CNN
F 1 "GND" V 6950 6300 50  0000 C CNN
F 2 "" H 6950 6500 50  0001 C CNN
F 3 "" H 6950 6500 50  0001 C CNN
	1    6950 6500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 66611616
P 6750 6500
F 0 "#PWR?" H 6750 6250 50  0001 C CNN
F 1 "GND" V 6750 6300 50  0000 C CNN
F 2 "" H 6750 6500 50  0001 C CNN
F 3 "" H 6750 6500 50  0001 C CNN
	1    6750 6500
	1    0    0    -1  
$EndComp
Text GLabel 10900 7250 1    50   Input ~ 0
T41_3.3V
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 666229FF
P 4050 5500
F 0 "J?" V 4050 6050 50  0000 R CNN
F 1 "RPI_ILI9488" V 4200 5500 50  0000 R CNN
F 2 "" H 4050 5500 50  0001 C CNN
F 3 "~" H 4050 5500 50  0001 C CNN
	1    4050 5500
	0    1    -1   0   
$EndComp
Text GLabel 4350 5700 3    50   Input ~ 0
RPI_5V
$Comp
L power:GND #PWR?
U 1 1 666282FE
P 4250 5700
F 0 "#PWR?" H 4250 5450 50  0001 C CNN
F 1 "GND" V 4250 5500 50  0000 C CNN
F 2 "" H 4250 5700 50  0001 C CNN
F 3 "" H 4250 5700 50  0001 C CNN
	1    4250 5700
	1    0    0    -1  
$EndComp
Text GLabel 3950 5700 3    50   Input ~ 0
RPI_MOSI
Text GLabel 4050 5700 3    50   Input ~ 0
RPI_MISO
Text GLabel 4150 5700 3    50   Input ~ 0
RPI_SCLK
Text GLabel 3850 5700 3    50   Input ~ 0
RPI_CD
Text GLabel 3750 5700 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 3650 5700 3    50   Input ~ 0
RPI_TOUCH_CS
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 6663CA44
P 4050 6850
F 0 "J?" V 4050 7400 50  0000 R CNN
F 1 "T3_ILI9488" V 4200 6850 50  0000 R CNN
F 2 "" H 4050 6850 50  0001 C CNN
F 3 "~" H 4050 6850 50  0001 C CNN
	1    4050 6850
	0    1    -1   0   
$EndComp
Text GLabel 4350 7050 3    50   Input ~ 0
T3_VIN
$Comp
L power:GND #PWR?
U 1 1 6663CA4B
P 4250 7050
F 0 "#PWR?" H 4250 6800 50  0001 C CNN
F 1 "GND" V 4250 6850 50  0000 C CNN
F 2 "" H 4250 7050 50  0001 C CNN
F 3 "" H 4250 7050 50  0001 C CNN
	1    4250 7050
	1    0    0    -1  
$EndComp
Text GLabel 3950 7050 3    50   Input ~ 0
T3_MOSI
Text GLabel 4050 7050 3    50   Input ~ 0
T3_MISO
Text GLabel 4150 7050 3    50   Input ~ 0
T3_SCLK
Text GLabel 3850 7050 3    50   Input ~ 0
T3_CD
Text GLabel 3750 7050 3    50   Input ~ 0
T3_LED_CS
Text GLabel 3650 7050 3    50   Input ~ 0
T3_TOUCH_CS
Text GLabel 11400 2100 3    50   Input ~ 0
EXPR_3.3V
Text GLabel 9700 4800 0    50   Input ~ 0
RPI_5V
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66683239
P 9900 4800
F 0 "J?" H 9850 4950 50  0001 L CNN
F 1 "rpi5V" H 9980 4751 50  0001 L CNN
F 2 "" H 9900 4800 50  0001 C CNN
F 3 "~" H 9900 4800 50  0001 C CNN
	1    9900 4800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 6668B907
P 10200 4800
F 0 "J?" H 10150 4950 50  0001 L CNN
F 1 "rpi5V" H 10280 4751 50  0001 L CNN
F 2 "" H 10200 4800 50  0001 C CNN
F 3 "~" H 10200 4800 50  0001 C CNN
	1    10200 4800
	-1   0    0    1   
$EndComp
Text GLabel 10400 4800 2    50   Input ~ 0
T4_VIN
$Comp
L 0_my_teensy:myTeensy4.0 U?
U 1 1 663299A8
P 16250 6900
F 0 "U?" H 15406 6903 60  0000 R CNN
F 1 "myTeensy4.0" H 15406 6797 60  0000 R CNN
F 2 "" V 17150 6950 60  0000 C CNN
F 3 "" V 17150 6950 60  0000 C CNN
	1    16250 6900
	1    0    0    -1  
$EndComp
Text GLabel 17250 6700 2    50   Input ~ 10
32_OUT1B
Text GLabel 17250 6800 2    50   Input ~ 10
HOST_D-
Text GLabel 17250 7100 2    50   Input ~ 10
HOST_D+
Text Notes 17250 6400 0    50   ~ 10
Pins from underside of 4.0\nneeded for Host and teensyQuad
Text GLabel 20200 8850 3    50   Input ~ 10
T40_TX
Text GLabel 20300 8850 3    50   Input ~ 10
T40_RX
Text Notes 20100 9800 0    50   ~ 10
Additional Serial Port added\nfor T40 as main USB/Sound device
Text Notes 15200 4900 0    79   ~ 16
Teensy 4.0 as Main Sound/USB device
Text GLabel 17900 6700 2    50   Input ~ 0
I2S_TX1B
Wire Wire Line
	17250 6700 17250 6600
Wire Wire Line
	17250 6600 17900 6600
Wire Wire Line
	17900 6600 17900 6700
Text Notes 10450 5800 0    79   ~ 16
Teensy 4.1 as Combined Sound/USB/UI device
Text GLabel 15900 8000 3    50   Input ~ 0
T40_TX
Text GLabel 15800 8000 3    50   Input ~ 0
T40_RX
Text GLabel 16500 8000 3    50   Input ~ 0
I2S_TX1A
Text GLabel 16600 8000 3    50   Input ~ 0
I2S_RX1A
Text GLabel 16400 8000 3    50   Input ~ 0
I2S_RX1B
Text GLabel 16000 5900 1    50   Input ~ 0
MCLK1
Text GLabel 16200 5900 1    50   Input ~ 0
BCLK1
Text GLabel 16300 5900 1    50   Input ~ 0
LRCLK1
Text GLabel 16400 5900 1    50   Input ~ 0
I2C_SCL
Text GLabel 16500 5900 1    50   Input ~ 0
I2C_SCA
Text GLabel 16800 5900 1    50   Input ~ 0
I2C_SCLK
Text Notes 15000 5050 0    50   ~ 0
means that all MIDI from the Pedal has to come into this as serial data
Text Notes 10650 6050 0    50   ~ 0
Note that in general I do not see a Audio-Midi4-Serial device\nso these 4.x based audio devices will present a lot of midi ports
Text Notes 11800 5350 0    50   ~ 0
Perhaps another alternative is to use an IO multiplexer for\nreading the expressions and rotaries
Text Notes 15450 5150 0    50   ~ 0
TX4/RX4 are still available in this configuration
$EndSCHEMATC
