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
Text GLabel 10500 7000 3    50   Input ~ 0
T3_MISO
Text GLabel 10400 7000 3    50   Input ~ 0
T3_MOSI
Text GLabel 11200 7000 3    50   Input ~ 10
LED_DIN
$Comp
L power:+3V3 #PWR015
U 1 1 6637C7EA
P 10600 7000
F 0 "#PWR015" H 10600 6850 50  0001 C CNN
F 1 "+3V3" V 10600 7200 50  0000 C CNN
F 2 "" H 10600 7000 50  0001 C CNN
F 3 "" H 10600 7000 50  0001 C CNN
	1    10600 7000
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR014
U 1 1 6637C7F0
P 9200 7000
F 0 "#PWR014" H 9200 6750 50  0001 C CNN
F 1 "GND" V 9200 6800 50  0000 C CNN
F 2 "" H 9200 7000 50  0001 C CNN
F 3 "" H 9200 7000 50  0001 C CNN
	1    9200 7000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 66399467
P 10600 4950
F 0 "#PWR04" H 10600 4700 50  0001 C CNN
F 1 "GND" V 10600 4750 50  0000 C CNN
F 2 "" H 10600 4950 50  0001 C CNN
F 3 "" H 10600 4950 50  0001 C CNN
	1    10600 4950
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 66399473
P 9300 4950
F 0 "#PWR03" H 9300 4700 50  0001 C CNN
F 1 "GND" V 9300 4750 50  0000 C CNN
F 2 "" H 9300 4950 50  0001 C CNN
F 3 "" H 9300 4950 50  0001 C CNN
	1    9300 4950
	-1   0    0    1   
$EndComp
$Comp
L 0_my_teensy:teensyAudioRevB U2
U 1 1 663C0C59
P 6550 10050
F 0 "U2" H 7428 10078 60  0001 L CNN
F 1 "teensyAudioRevB" H 7000 10000 60  0000 R CNN
F 2 "0_my_teensy:audioShieldRevB" V 7500 10000 60  0001 C CNN
F 3 "" V 7500 10000 60  0000 C CNN
	1    6550 10050
	-1   0    0    1   
$EndComp
$Comp
L 0_my_teensy:myTeensy4.1 U1
U 1 1 6636A2E9
P 10300 5850
F 0 "U1" H 10550 5750 50  0000 R CNN
F 1 "myTeensy4.1" H 10800 5900 50  0000 R CNN
F 2 "0_my_teensy:myTeensy41" H 10000 4950 50  0001 C CNN
F 3 "" H 10000 4950 50  0001 C CNN
	1    10300 5850
	1    0    0    -1  
$EndComp
Text GLabel 9500 4950 1    50   Input ~ 0
MCLK1
Text GLabel 5900 9000 1    50   Input ~ 0
MCLK1
Text GLabel 9700 4950 1    50   Input ~ 0
BCLK1
Text GLabel 6100 9300 1    50   Input ~ 0
BCLK1
Text GLabel 9800 4950 1    50   Input ~ 0
LRCLK1
Text GLabel 6800 10750 3    50   Input ~ 0
LRCLK1
Text GLabel 9900 4950 1    50   Input ~ 0
I2C_SCL
Text GLabel 10000 4950 1    50   Input ~ 0
I2C_SCA
Text GLabel 6400 10750 3    50   Input ~ 0
I2C_SCL
Text GLabel 6300 10750 3    50   Input ~ 0
I2C_SCA
Text GLabel 5900 10750 3    50   Input ~ 0
I2C_SCLK
Text GLabel 10300 4950 1    50   Input ~ 0
I2C_SCLK
Text GLabel 5800 10750 3    50   Input ~ 0
I2S_RX1A
Text GLabel 6700 10750 3    50   Input ~ 0
I2S_TX1A
Text GLabel 10000 7000 3    50   Input ~ 0
I2S_TX1A
Text GLabel 10100 7000 3    50   Input ~ 0
I2S_RX1A
Text GLabel 11500 7000 3    50   Input ~ 0
I2S_TX1B
Text GLabel 9900 7000 3    50   Input ~ 0
I2S_RX1B
Text GLabel 9400 7000 3    50   Input ~ 0
T41_TX
Text GLabel 9300 7000 3    50   Input ~ 0
T41_RX
Text Notes 2600 1350 0    50   ~ 10
GPIO25 RPI_READY conflicts with Blue rPi 3.5" display RESET
Text Notes 2600 1500 0    50   ~ 10
GPIO23 OCTO_MULT2 conflics with Blue rPi 2.5" display CD pin
Text GLabel 10300 7000 3    50   Input ~ 0
T3_LED_CS
Text GLabel 11000 7000 3    50   Input ~ 10
T3_TOUCH_CS
$Comp
L Device:R R1
U 1 1 6652E99E
P 5900 9150
F 0 "R1" H 5850 9800 50  0001 L CNN
F 1 "100" V 5900 9150 50  0000 C CNN
F 2 "0_my_footprints:myResistor" V 5830 9150 50  0001 C CNN
F 3 "~" H 5900 9150 50  0001 C CNN
	1    5900 9150
	1    0    0    -1  
$EndComp
Text Notes 11150 7550 0    50   ~ 10
LED_IN chosen using teensy w2812Serial alternate pin options
Text GLabel 10200 7000 3    50   Input ~ 0
T3_CD
Text Notes 10950 7800 0    50   ~ 10
T3_TOUCH_CS chosen as "can use any digital pin" from ILI9341 page
Text Notes 2600 1200 0    50   ~ 10
BOLD pins are reserved for future potential use
Text GLabel 9200 4950 1    50   Input ~ 0
T41_VIN
Text GLabel 9400 4950 1    50   Input ~ 0
T41_3.3V
$Comp
L Connector_Generic:Conn_01x08 J2
U 1 1 666229FF
P 3450 3600
F 0 "J2" V 3450 4150 50  0001 R CNN
F 1 "RPI_ILI9488" V 3600 3750 50  0000 R CNN
F 2 "0_my_footprints:myJSTx08" H 3450 3600 50  0001 C CNN
F 3 "~" H 3450 3600 50  0001 C CNN
	1    3450 3600
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR02
U 1 1 666282FE
P 3150 3400
F 0 "#PWR02" H 3150 3150 50  0001 C CNN
F 1 "GND" V 3150 3200 50  0000 C CNN
F 2 "" H 3150 3400 50  0001 C CNN
F 3 "" H 3150 3400 50  0001 C CNN
	1    3150 3400
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J1
U 1 1 6663CA44
P 10000 3600
F 0 "J1" V 10000 4150 50  0001 R CNN
F 1 "T3_ILI9488" V 10150 3750 50  0000 R CNN
F 2 "0_my_footprints:myJSTx08" H 10000 3600 50  0001 C CNN
F 3 "~" H 10000 3600 50  0001 C CNN
	1    10000 3600
	0    1    1    0   
$EndComp
Text GLabel 10300 3400 1    50   Input ~ 0
T41_VIN
$Comp
L power:GND #PWR01
U 1 1 6663CA4B
P 9700 3400
F 0 "#PWR01" H 9700 3150 50  0001 C CNN
F 1 "GND" V 9700 3200 50  0000 C CNN
F 2 "" H 9700 3400 50  0001 C CNN
F 3 "" H 9700 3400 50  0001 C CNN
	1    9700 3400
	-1   0    0    1   
$EndComp
Text GLabel 10000 3400 1    50   Input ~ 0
T3_MOSI
Text GLabel 9900 3400 1    50   Input ~ 0
T3_MISO
Text GLabel 9600 3400 1    50   Input ~ 0
T3_SCLK
Text GLabel 10200 3400 1    50   Input ~ 0
T3_CD
Text GLabel 10100 3400 1    50   Input ~ 0
T3_LED_CS
Text GLabel 9800 3400 1    50   Input ~ 0
T3_TOUCH_CS
Text GLabel 6850 4250 0    50   Input ~ 0
RPI_5V
Text GLabel 6950 4250 2    50   Input ~ 0
T41_VIN
Text Notes 2650 950  0    79   ~ 16
rPi Connector
Text Notes 5650 8050 0    79   ~ 16
teensy Audio Shield Rev B
Text Notes 5650 8350 0    50   ~ 0
This PCB is only needed until I get the RevD audio shields,\nat which time it can be stacked with the 4.1
Text Notes 9250 2200 0    50   ~ 10
ILI9488_T3 pins are assumed to match ILI9341_T3 pins
Text GLabel 10500 4950 1    50   Input ~ 0
T3_SCLK
Text Notes 9250 900  0    79   ~ 16
Teensy 4.1 as test Sound/USB/MIDI device
Text Notes 9250 1500 0    50   ~ 0
Note that I do not see a Audio-Midi4-Serial device, so these 4.x based\ndevices will need attention to not present a huge list of midi ports
Text Notes 9250 1850 0    50   ~ 0
Test configuration is to do the USB stuff and Audio while being a MIDIx16 devices \nyet there is no actual MIDI in this test phase except for the USB host.  \nAll other MIDI will need to come in over serial somehow.
Text Notes 9250 2000 0    50   ~ 0
I will allow for a test ILI9488 display, but not sure I'll hook it up
Text Notes 9250 1050 0    50   ~ 0
No buttons, expression pedals or rotaries in this version.
Text Notes 9250 1250 0    50   ~ 0
I WILL bring all the other pins out for potential development. \nI WILL add a connector a LED strip for potential development.
$Comp
L Connector_Generic:Conn_01x03 J10
U 1 1 663DCA90
P 10650 9100
F 0 "J10" H 10730 9142 50  0001 L CNN
F 1 "LEDs" H 10730 9096 50  0000 L CNN
F 2 "0_my_footprints:myJSTx03" H 10650 9100 50  0001 C CNN
F 3 "~" H 10650 9100 50  0001 C CNN
	1    10650 9100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 663E0829
P 10450 9200
F 0 "#PWR016" H 10450 8950 50  0001 C CNN
F 1 "GND" V 10450 9000 50  0000 C CNN
F 2 "" H 10450 9200 50  0001 C CNN
F 3 "" H 10450 9200 50  0001 C CNN
	1    10450 9200
	0    1    1    0   
$EndComp
Text GLabel 10450 9100 0    50   Input ~ 0
LED_DIN
Text GLabel 3250 3400 1    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 3550 3400 1    50   Input ~ 0
RPI_LED_CS
Text GLabel 3650 3400 1    50   Input ~ 0
RPI_CD
Text GLabel 3050 3400 1    50   Input ~ 0
RPI_SCLK
Text GLabel 3350 3400 1    50   Input ~ 0
RPI_MISO
Text GLabel 3450 3400 1    50   Input ~ 0
RPI_MOSI
Text GLabel 3750 3400 1    50   Input ~ 0
RPI_5V
$Comp
L power:GND #PWR09
U 1 1 66611616
P 2850 6750
F 0 "#PWR09" H 2850 6500 50  0001 C CNN
F 1 "GND" V 2850 6550 50  0000 C CNN
F 2 "" H 2850 6750 50  0001 C CNN
F 3 "" H 2850 6750 50  0001 C CNN
	1    2850 6750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 666110E8
P 3050 6750
F 0 "#PWR010" H 3050 6500 50  0001 C CNN
F 1 "GND" V 3050 6550 50  0000 C CNN
F 2 "" H 3050 6750 50  0001 C CNN
F 3 "" H 3050 6750 50  0001 C CNN
	1    3050 6750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 666109AB
P 3550 6750
F 0 "#PWR011" H 3550 6500 50  0001 C CNN
F 1 "GND" V 3550 6550 50  0000 C CNN
F 2 "" H 3550 6750 50  0001 C CNN
F 3 "" H 3550 6750 50  0001 C CNN
	1    3550 6750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 66610187
P 3850 6750
F 0 "#PWR012" H 3850 6500 50  0001 C CNN
F 1 "GND" V 3850 6550 50  0000 C CNN
F 2 "" H 3850 6750 50  0001 C CNN
F 3 "" H 3850 6750 50  0001 C CNN
	1    3850 6750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 6660F832
P 4250 6750
F 0 "#PWR013" H 4250 6500 50  0001 C CNN
F 1 "GND" V 4250 6550 50  0000 C CNN
F 2 "" H 4250 6750 50  0001 C CNN
F 3 "" H 4250 6750 50  0001 C CNN
	1    4250 6750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 6660EE40
P 2550 5400
F 0 "#PWR05" H 2550 5150 50  0001 C CNN
F 1 "GND" V 2550 5200 50  0000 C CNN
F 2 "" H 2550 5400 50  0001 C CNN
F 3 "" H 2550 5400 50  0001 C CNN
	1    2550 5400
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 6660E9EA
P 3250 5400
F 0 "#PWR06" H 3250 5150 50  0001 C CNN
F 1 "GND" V 3250 5200 50  0000 C CNN
F 2 "" H 3250 5400 50  0001 C CNN
F 3 "" H 3250 5400 50  0001 C CNN
	1    3250 5400
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR07
U 1 1 6660E5AB
P 4050 5400
F 0 "#PWR07" H 4050 5150 50  0001 C CNN
F 1 "GND" V 4050 5200 50  0000 C CNN
F 2 "" H 4050 5400 50  0001 C CNN
F 3 "" H 4050 5400 50  0001 C CNN
	1    4050 5400
	-1   0    0    1   
$EndComp
Text GLabel 4450 5400 1    50   Input ~ 0
RPI_3.3V
Text GLabel 4450 6750 3    50   Input ~ 0
RPI_5V
Text GLabel 4250 5400 1    50   Input ~ 10
OCTO_SCL
Text GLabel 4350 5400 1    50   Input ~ 10
OCTO_SDA
Text GLabel 3950 5400 1    50   Input ~ 10
UNUSED_DISPLAY_TOUCH_IRQ
Text GLabel 2950 6750 3    50   Input ~ 0
RPI_I2S_RX_ACTIVE
Text GLabel 2750 6750 3    50   Input ~ 0
RPI_I2S_TX_ACTIVE
Text GLabel 3450 6750 3    50   Input ~ 0
RPI_READY
Text GLabel 3650 6750 3    50   Input ~ 10
OCTO_MULT3
Text GLabel 3750 6750 3    50   Input ~ 10
OCTO_MULT2
Text GLabel 3850 5400 1    50   Input ~ 10
OCTO_MULT0
Text GLabel 3750 5400 1    50   Input ~ 10
OCTO_MULT1
Text GLabel 3050 5400 1    50   Input ~ 10
OCTO_RESET
Text GLabel 3250 6750 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 3350 6750 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 4150 5400 1    50   Input ~ 0
RPI_CD
Text GLabel 3350 5400 1    50   Input ~ 0
RPI_SCLK
Text GLabel 3450 5400 1    50   Input ~ 0
RPI_MISO
Text GLabel 3550 5400 1    50   Input ~ 0
RPI_MOSI
Text GLabel 4150 6750 3    50   Input ~ 0
RPI_TX
Text GLabel 4050 6750 3    50   Input ~ 0
RPI_RX
Text GLabel 2550 6750 3    50   Input ~ 0
I2S_RX1B
Text GLabel 2650 6750 3    50   Input ~ 0
I2S_TX1B
Text GLabel 2750 5400 1    50   Input ~ 0
LRCLK1
Text GLabel 3950 6750 3    50   Input ~ 0
BCLK1
$Comp
L 0_my_symbols:rpi_connector J8
U 1 1 6636D4F6
P 3500 6050
F 0 "J8" H 4528 6071 50  0001 L CNN
F 1 "rpi_connector" H 3200 6050 50  0000 L CNN
F 2 "0_my_footprints:myRpiConnector" H 3400 5950 50  0001 C CNN
F 3 "" H 3400 5950 50  0001 C CNN
	1    3500 6050
	1    0    0    -1  
$EndComp
Text Notes 2600 2400 0    50   ~ 10
ILI9488 connector pins should be the same for rPi and T41\nso I can use same cables and interface boards.  \nThe final order is yet to be determined.
Text Notes 15200 4550 0    79   ~ 16
Pins to teensy 3.2
Text Notes 14850 4900 0    50   ~ 10
I have yet to determine what kind of connector to use,\nor the exact order of these pins; 10, 6+4, 6+3, 8+2
Text GLabel 2500 8700 0    50   Input ~ 10
RPI_RUN
$Comp
L Connector_Generic:Conn_01x01 J9
U 1 1 66503904
P 2700 8700
F 0 "J9" H 2650 8850 50  0001 L CNN
F 1 "RPI_RUN" H 2780 8651 50  0001 L CNN
F 2 "cnc3018-PCB:my1pin" H 2700 8700 50  0001 C CNN
F 3 "~" H 2700 8700 50  0001 C CNN
	1    2700 8700
	1    0    0    -1  
$EndComp
Text Notes 1450 8350 0    50   ~ 0
The rPI run pin is a separate cable and dupont connector,\npossibly through the board to the teensy3.2 connector
Text GLabel 15800 5150 0    50   Input ~ 10
RPI_RUN
Text GLabel 15800 5450 0    50   Input ~ 0
RPI_READY
Text GLabel 15800 5350 0    50   Input ~ 0
RPI_I2S_RX_ACTIVE
Text GLabel 15800 5250 0    50   Input ~ 0
RPI_I2S_TX_ACTIVE
Text GLabel 15800 5650 0    50   Input ~ 0
RPI_TX
Text GLabel 15800 5550 0    50   Input ~ 0
RPI_RX
Text GLabel 15800 5950 0    50   Input ~ 0
T41_TX
Text GLabel 15800 5850 0    50   Input ~ 0
T41_RX
$Comp
L power:GND #PWR08
U 1 1 66516B43
P 15800 6050
F 0 "#PWR08" H 15800 5800 50  0001 C CNN
F 1 "GND" V 15800 5850 50  0000 C CNN
F 2 "" H 15800 6050 50  0001 C CNN
F 3 "" H 15800 6050 50  0001 C CNN
	1    15800 6050
	0    1    1    0   
$EndComp
Text GLabel 15800 6150 0    50   Input ~ 0
T32_VIN
Text Notes 5800 3800 0    50   ~ 10
A jumper will exist to optionally power the teensy 4.1 from the RPI
Text GLabel 6800 6150 0    50   Input ~ 0
RPI_5V
Text GLabel 6900 6150 2    50   Input ~ 0
T32_VIN
Text Notes 5650 5700 0    50   ~ 10
A jumper will exist to optionally power the teensy 3.2 from the RPI
$Comp
L Connector_Generic:Conn_01x06 J5
U 1 1 66580838
P 16000 5350
F 0 "J5" H 16080 5342 50  0001 L CNN
F 1 "rPi to T32" H 16080 5296 50  0000 L CNN
F 2 "0_my_footprints:myJSTx06" H 16000 5350 50  0001 C CNN
F 3 "~" H 16000 5350 50  0001 C CNN
	1    16000 5350
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J7
U 1 1 6658130B
P 16000 5950
F 0 "J7" H 16080 5942 50  0001 L CNN
F 1 "T41 to T32" H 16080 5896 50  0000 L CNN
F 2 "0_my_footprints:myJSTx04" H 16000 5950 50  0001 C CNN
F 3 "~" H 16000 5950 50  0001 C CNN
	1    16000 5950
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 66582C3E
P 6850 4050
F 0 "J3" H 6930 4042 50  0001 L CNN
F 1 "T41 Power" V 6950 4200 50  0000 R CNN
F 2 "0_my_footprints:myPinHeader_1x02" H 6850 4050 50  0001 C CNN
F 3 "~" H 6850 4050 50  0001 C CNN
	1    6850 4050
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J6
U 1 1 665ADEF6
P 6800 5950
F 0 "J6" H 6880 5942 50  0001 L CNN
F 1 "T32 Power" V 6900 6100 50  0000 R CNN
F 2 "0_my_footprints:myPinHeader_1x02" H 6800 5950 50  0001 C CNN
F 3 "~" H 6800 5950 50  0001 C CNN
	1    6800 5950
	0    -1   -1   0   
$EndComp
Text GLabel 10450 9000 0    50   Input ~ 0
T41_VIN
$Comp
L power:GND #PWR0101
U 1 1 6634081F
P 7100 9300
F 0 "#PWR0101" H 7100 9050 50  0001 C CNN
F 1 "GND" V 7100 9100 50  0000 C CNN
F 2 "" H 7100 9300 50  0001 C CNN
F 3 "" H 7100 9300 50  0001 C CNN
	1    7100 9300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 66341ED9
P 7000 10750
F 0 "#PWR0102" H 7000 10500 50  0001 C CNN
F 1 "GND" V 7000 10550 50  0000 C CNN
F 2 "" H 7000 10750 50  0001 C CNN
F 3 "" H 7000 10750 50  0001 C CNN
	1    7000 10750
	1    0    0    -1  
$EndComp
Text GLabel 6900 10750 3    50   Input ~ 0
T41_3.3V
$EndSCHEMATC
