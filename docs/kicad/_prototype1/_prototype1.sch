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
L Device:R R1
U 1 1 64CCBE5B
P 6550 5600
F 0 "R1" H 6620 5646 50  0000 L CNN
F 1 "220" V 6550 5600 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 6480 5600 50  0001 C CNN
F 3 "~" H 6550 5600 50  0001 C CNN
	1    6550 5600
	0    -1   1    0   
$EndComp
Text GLabel 6950 5600 2    50   Input ~ 10
RPI_RUN
Text GLabel 6250 5600 0    50   Input ~ 0
SENS_PI_RUN
$Comp
L Transistor_BJT:BC547 Q1
U 1 1 64CF1712
P 6750 5300
F 0 "Q1" H 6941 5346 50  0000 L CNN
F 1 "BC547" H 6941 5255 50  0000 L CNN
F 2 "0_my_footprints2:transistor" H 6950 5225 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC547.pdf" H 6750 5300 50  0001 L CNN
	1    6750 5300
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR08
U 1 1 64CF464E
P 6850 5000
F 0 "#PWR08" H 6850 4750 50  0001 C CNN
F 1 "GND" H 6855 4827 50  0000 C CNN
F 2 "" H 6850 5000 50  0001 C CNN
F 3 "" H 6850 5000 50  0001 C CNN
	1    6850 5000
	1    0    0    1   
$EndComp
Text GLabel 6250 5300 0    50   Input ~ 0
PI_REBOOT
Wire Wire Line
	6700 5600 6850 5600
Wire Wire Line
	6850 5500 6850 5600
Connection ~ 6850 5600
Wire Wire Line
	6850 5600 6950 5600
Wire Wire Line
	6850 5100 6850 5000
$Comp
L Device:LED D5
U 1 1 662B77AA
P 3800 7850
F 0 "D5" H 3793 7595 50  0001 C CNN
F 1 "BLUE" H 3793 7686 50  0000 C CNN
F 2 "0_my_footprints2:led_blue" H 3800 7850 50  0001 C CNN
F 3 "~" H 3800 7850 50  0001 C CNN
	1    3800 7850
	-1   0    0    1   
$EndComp
$Comp
L Device:R R6
U 1 1 662B77B0
P 3500 7850
F 0 "R6" V 3650 7800 50  0001 L CNN
F 1 "560" V 3500 7850 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3430 7850 50  0001 C CNN
F 3 "~" H 3500 7850 50  0001 C CNN
	1    3500 7850
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR019
U 1 1 662B77B6
P 3950 7850
F 0 "#PWR019" H 3950 7600 50  0001 C CNN
F 1 "GND" V 3950 7650 50  0000 C CNN
F 2 "" H 3950 7850 50  0001 C CNN
F 3 "" H 3950 7850 50  0001 C CNN
	1    3950 7850
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D4
U 1 1 662BFCA9
P 3800 7350
F 0 "D4" H 3793 7095 50  0001 C CNN
F 1 "GREEN" H 3793 7186 50  0000 C CNN
F 2 "0_my_footprints2:led_green" H 3800 7350 50  0001 C CNN
F 3 "~" H 3800 7350 50  0001 C CNN
	1    3800 7350
	-1   0    0    1   
$EndComp
$Comp
L Device:R R3
U 1 1 662BFCAF
P 3500 7350
F 0 "R3" V 3650 7300 50  0001 L CNN
F 1 "220" V 3500 7350 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3430 7350 50  0001 C CNN
F 3 "~" H 3500 7350 50  0001 C CNN
	1    3500 7350
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR017
U 1 1 662BFCB5
P 3950 7350
F 0 "#PWR017" H 3950 7100 50  0001 C CNN
F 1 "GND" V 3950 7150 50  0000 C CNN
F 2 "" H 3950 7350 50  0001 C CNN
F 3 "" H 3950 7350 50  0001 C CNN
	1    3950 7350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6250 5300 6550 5300
Wire Wire Line
	6250 5600 6400 5600
Text GLabel 3350 7850 0    50   Input ~ 0
LED_PI_READY
Text GLabel 3350 7350 0    50   Input ~ 0
LED_PI_RUN
Text GLabel 4000 5050 2    50   Input ~ 0
RPI_READY
$Comp
L 0_my_symbols:ESP32_DEV U2
U 1 1 66564A96
P 3450 4750
F 0 "U2" H 4578 4846 50  0000 L CNN
F 1 "ESP32_DEV" H 4578 4755 50  0000 L CNN
F 2 "0_my_footprints2:esp32_dev" H 3100 4850 50  0001 C CNN
F 3 "" H 3100 4850 50  0001 C CNN
	1    3450 4750
	0    1    1    0   
$EndComp
Text GLabel 3000 5550 0    50   Input ~ 0
LED_ALIVE
Text GLabel 3000 4350 0    50   Input ~ 0
LED_PI_READY
Text GLabel 3000 5250 0    50   Input ~ 0
LED_PI_RUN
Text GLabel 4000 5250 2    50   Input ~ 0
SENS_PI_RUN
Text GLabel 4000 5150 2    50   Input ~ 0
PI_REBOOT
$Comp
L power:GND #PWR013
U 1 1 66610187
P 13350 5350
F 0 "#PWR013" H 13350 5100 50  0001 C CNN
F 1 "GND" V 13350 5150 50  0000 C CNN
F 2 "" H 13350 5350 50  0001 C CNN
F 3 "" H 13350 5350 50  0001 C CNN
	1    13350 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 6660F832
P 13750 5350
F 0 "#PWR014" H 13750 5100 50  0001 C CNN
F 1 "GND" V 13750 5150 50  0000 C CNN
F 2 "" H 13750 5350 50  0001 C CNN
F 3 "" H 13750 5350 50  0001 C CNN
	1    13750 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 6660E5AB
P 13550 4150
F 0 "#PWR05" H 13550 3900 50  0001 C CNN
F 1 "GND" V 13550 3950 50  0000 C CNN
F 2 "" H 13550 4150 50  0001 C CNN
F 3 "" H 13550 4150 50  0001 C CNN
	1    13550 4150
	-1   0    0    1   
$EndComp
Text GLabel 13450 4150 1    50   Input Italic 0
RPI_CD
Text GLabel 13650 5350 3    50   UnSpc ~ 0
RPI_TX
Text GLabel 13550 5350 3    50   UnSpc ~ 0
RPI_RX
Text GLabel 13450 5350 3    50   Output ~ 10
I2S_BCLK
Text GLabel 12600 7100 0    50   Input ~ 0
RPI_I2S_TX_ACTIVE
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 66503904
P 11550 3900
F 0 "J2" H 11500 4050 50  0001 L CNN
F 1 "RPI_RUN" H 11630 3851 50  0001 L CNN
F 2 "0_my_footprints2:Oval1Pin" H 11550 3900 50  0001 C CNN
F 3 "~" H 11550 3900 50  0001 C CNN
	1    11550 3900
	0    -1   -1   0   
$EndComp
Text GLabel 11550 4100 3    50   UnSpc ~ 0
RPI_RUN
Text GLabel 12250 4150 1    50   Output ~ 10
I2S_FCLK
Text GLabel 12150 5350 3    50   Output ~ 10
I2S_RX
Text GLabel 12050 5350 3    50   Output ~ 10
I2S_TX
Text GLabel 13050 4150 1    50   Input ~ 0
RPI_MOSI
Text GLabel 12950 4150 1    50   Input ~ 0
RPI_MISO
Text GLabel 12850 4150 1    50   Input ~ 0
RPI_SCLK
Text GLabel 12850 5350 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 12750 5350 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 12950 5350 3    50   UnSpc ~ 0
RPI_READY
Text GLabel 13350 7100 2    50   Input ~ 0
RPI_I2S_RX_ACTIVE
$Comp
L power:GND #PWR04
U 1 1 6660E9EA
P 12750 4150
F 0 "#PWR04" H 12750 3900 50  0001 C CNN
F 1 "GND" V 12750 3950 50  0000 C CNN
F 2 "" H 12750 4150 50  0001 C CNN
F 3 "" H 12750 4150 50  0001 C CNN
	1    12750 4150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 6660EE40
P 12050 4150
F 0 "#PWR03" H 12050 3900 50  0001 C CNN
F 1 "GND" V 12050 3950 50  0000 C CNN
F 2 "" H 12050 4150 50  0001 C CNN
F 3 "" H 12050 4150 50  0001 C CNN
	1    12050 4150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR012
U 1 1 666109AB
P 13050 5350
F 0 "#PWR012" H 13050 5100 50  0001 C CNN
F 1 "GND" V 13050 5150 50  0000 C CNN
F 2 "" H 13050 5350 50  0001 C CNN
F 3 "" H 13050 5350 50  0001 C CNN
	1    13050 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 666110E8
P 12550 5350
F 0 "#PWR011" H 12550 5100 50  0001 C CNN
F 1 "GND" V 12550 5150 50  0000 C CNN
F 2 "" H 12550 5350 50  0001 C CNN
F 3 "" H 12550 5350 50  0001 C CNN
	1    12550 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 66611616
P 12350 5350
F 0 "#PWR010" H 12350 5100 50  0001 C CNN
F 1 "GND" V 12350 5150 50  0000 C CNN
F 2 "" H 12350 5350 50  0001 C CNN
F 3 "" H 12350 5350 50  0001 C CNN
	1    12350 5350
	1    0    0    -1  
$EndComp
$Comp
L 0_my_symbols:rpiA J4
U 1 1 66521D2F
P 13000 5000
F 0 "J4" H 13050 5150 50  0001 C CNN
F 1 "rpiA" H 13100 5150 50  0000 C CNN
F 2 "0_my_footprints2:pinSocket1x20" H 12850 4850 50  0001 C CNN
F 3 "" H 12850 4850 50  0001 C CNN
	1    13000 5000
	1    0    0    -1  
$EndComp
$Comp
L 0_my_symbols:rpiB J3
U 1 1 66525118
P 13000 4500
F 0 "J3" H 13200 4450 50  0001 C CNN
F 1 "rpiB" H 13350 4450 50  0000 C CNN
F 2 "0_my_footprints2:pinSocket1x20" H 12900 4100 50  0001 C CNN
F 3 "" H 12900 4100 50  0001 C CNN
	1    13000 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 6645117F
P 9300 7500
F 0 "#PWR018" H 9300 7250 50  0001 C CNN
F 1 "GND" V 9300 7300 50  0000 C CNN
F 2 "" H 9300 7500 50  0001 C CNN
F 3 "" H 9300 7500 50  0001 C CNN
	1    9300 7500
	-1   0    0    -1  
$EndComp
Text GLabel 9100 7500 3    50   Input ~ 0
RPI_MOSI
Text GLabel 9000 7500 3    50   Input ~ 0
RPI_MISO
Text GLabel 8900 7500 3    50   Input ~ 0
RPI_SCLK
Text GLabel 8700 7500 3    50   Input ~ 0
RPI_LED_CS
Text GLabel 8800 7500 3    50   Input ~ 0
RPI_TOUCH_CS
Text GLabel 9200 7500 3    50   Input ~ 0
RPI_CD
$Comp
L Device:LED D2
U 1 1 6647603F
P 12750 7600
F 0 "D2" H 12743 7345 50  0001 C CNN
F 1 "yellow" H 12743 7437 50  0000 C CNN
F 2 "0_my_footprints2:led_yellow" H 12750 7600 50  0001 C CNN
F 3 "~" H 12750 7600 50  0001 C CNN
	1    12750 7600
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R4
U 1 1 66478725
P 12750 7300
F 0 "R4" V 12543 7300 50  0001 C CNN
F 1 "220" V 12750 7300 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 12680 7300 50  0001 C CNN
F 3 "~" H 12750 7300 50  0001 C CNN
	1    12750 7300
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D3
U 1 1 66485268
P 13250 7600
F 0 "D3" H 13243 7345 50  0001 C CNN
F 1 "yellow" H 13243 7437 50  0000 C CNN
F 2 "0_my_footprints2:led_yellow" H 13250 7600 50  0001 C CNN
F 3 "~" H 13250 7600 50  0001 C CNN
	1    13250 7600
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R5
U 1 1 6648526E
P 13250 7300
F 0 "R5" V 13043 7300 50  0001 C CNN
F 1 "220" V 13250 7300 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 13180 7300 50  0001 C CNN
F 3 "~" H 13250 7300 50  0001 C CNN
	1    13250 7300
	-1   0    0    1   
$EndComp
Wire Wire Line
	13250 7750 13250 7850
Wire Wire Line
	12750 7750 12750 7850
Text GLabel 13850 4150 1    50   Output ~ 10
RPI_SDA
Text GLabel 13750 4150 1    50   Output ~ 10
RPI_SCL
Text GLabel 13650 4150 1    50   Output ~ 10
I2S_MCLK
Wire Wire Line
	13350 7100 13250 7100
Wire Wire Line
	13250 7100 13250 7150
Wire Wire Line
	12600 7100 12750 7100
Wire Wire Line
	12750 7100 12750 7150
Wire Wire Line
	12750 7850 13000 7850
$Comp
L power:GND #PWR020
U 1 1 66562CCD
P 13000 7950
F 0 "#PWR020" H 13000 7700 50  0001 C CNN
F 1 "GND" V 13000 7750 50  0000 C CNN
F 2 "" H 13000 7950 50  0001 C CNN
F 3 "" H 13000 7950 50  0001 C CNN
	1    13000 7950
	1    0    0    -1  
$EndComp
Wire Wire Line
	13000 7950 13000 7850
Connection ~ 13000 7850
Wire Wire Line
	13000 7850 13250 7850
$Comp
L Connector_Generic:Conn_01x08 J5
U 1 1 665749E8
P 9000 7300
F 0 "J5" V 8964 7680 50  0001 L CNN
F 1 "RPI_ILI9488" V 9150 7150 50  0000 L CNN
F 2 "0_my_footprints2:JST8" H 9000 7300 50  0001 C CNN
F 3 "~" H 9000 7300 50  0001 C CNN
	1    9000 7300
	0    1    -1   0   
$EndComp
Text GLabel 13950 4150 1    50   Output ~ 10
RPI_3V
Text GLabel 13850 5350 3    50   Input ~ 0
RPI_5V
Text GLabel 13950 5350 3    50   Input ~ 0
RPI_5V
Text GLabel 8600 7500 3    50   Input ~ 0
RPI_5V
Text GLabel 10300 4300 2    50   Input ~ 0
RPI_3V
$Comp
L power:GND #PWR02
U 1 1 66504B0C
P 8800 4100
F 0 "#PWR02" H 8800 3850 50  0001 C CNN
F 1 "GND" V 8800 3900 50  0000 C CNN
F 2 "" H 8800 4100 50  0001 C CNN
F 3 "" H 8800 4100 50  0001 C CNN
	1    8800 4100
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 66504730
P 10300 4200
F 0 "#PWR06" H 10300 3950 50  0001 C CNN
F 1 "GND" V 10300 4000 50  0000 C CNN
F 2 "" H 10300 4200 50  0001 C CNN
F 3 "" H 10300 4200 50  0001 C CNN
	1    10300 4200
	0    -1   1    0   
$EndComp
Text GLabel 10300 4600 2    50   Input ~ 0
I2S_BCLK
Text GLabel 8850 4900 0    50   Input ~ 0
I2S_TX
Text GLabel 8850 5000 0    50   Input ~ 0
I2S_RX
Text GLabel 10300 4400 2    50   Input ~ 0
I2S_MCLK
Text GLabel 10300 4800 2    50   Input ~ 0
RPI_SCL
Text GLabel 10300 4900 2    50   Input ~ 0
RPI_SDA
Text GLabel 10300 4700 2    50   Input ~ 0
I2S_FCLK
$Comp
L 0_my_teensy:teensyAudioRevD U1
U 1 1 664E10BD
P 9600 4650
F 0 "U1" H 10478 4678 60  0000 L CNN
F 1 "teensyAudioRevD" H 10478 4572 60  0000 L CNN
F 2 "0_my_teensy:audioShieldRevD" V 10550 4600 60  0001 C CNN
F 3 "" V 10550 4600 60  0000 C CNN
	1    9600 4650
	0    1    1    0   
$EndComp
Text GLabel 12450 5350 3    50   Input ~ 0
RPI_I2S_RX_ACTIVE
Text GLabel 12250 5350 3    50   Input ~ 0
RPI_I2S_TX_ACTIVE
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 66560AC2
P 6350 3550
F 0 "J1" V 6314 3362 50  0001 R CNN
F 1 "T4 Serial" V 6500 3700 50  0000 R CNN
F 2 "0_my_footprints2:JST3" H 6350 3550 50  0001 C CNN
F 3 "~" H 6350 3550 50  0001 C CNN
	1    6350 3550
	0    -1   -1   0   
$EndComp
Text GLabel 6350 3750 3    50   Input ~ 0
T4_RX1
Text GLabel 6450 3750 3    50   Input ~ 0
T4_TX1
$Comp
L power:GND #PWR015
U 1 1 665760CE
P 3000 5750
F 0 "#PWR015" H 3000 5500 50  0001 C CNN
F 1 "GND" V 3000 5550 50  0000 C CNN
F 2 "" H 3000 5750 50  0001 C CNN
F 3 "" H 3000 5750 50  0001 C CNN
	1    3000 5750
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR07
U 1 1 665774AD
P 4000 4450
F 0 "#PWR07" H 4000 4200 50  0001 C CNN
F 1 "GND" V 4000 4250 50  0000 C CNN
F 2 "" H 4000 4450 50  0001 C CNN
F 3 "" H 4000 4450 50  0001 C CNN
	1    4000 4450
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR09
U 1 1 66577A49
P 3000 5150
F 0 "#PWR09" H 3000 4900 50  0001 C CNN
F 1 "GND" V 3000 4950 50  0000 C CNN
F 2 "" H 3000 5150 50  0001 C CNN
F 3 "" H 3000 5150 50  0001 C CNN
	1    3000 5150
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR01
U 1 1 66578200
P 6250 3750
F 0 "#PWR01" H 6250 3500 50  0001 C CNN
F 1 "GND" V 6250 3550 50  0000 C CNN
F 2 "" H 6250 3750 50  0001 C CNN
F 3 "" H 6250 3750 50  0001 C CNN
	1    6250 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 665AB6F1
P 3800 6850
F 0 "D1" H 3793 6595 50  0001 C CNN
F 1 "RED" H 3793 6686 50  0000 C CNN
F 2 "0_my_footprints2:led_red" H 3800 6850 50  0001 C CNN
F 3 "~" H 3800 6850 50  0001 C CNN
	1    3800 6850
	-1   0    0    1   
$EndComp
$Comp
L Device:R R2
U 1 1 665AB6F7
P 3500 6850
F 0 "R2" V 3650 6800 50  0001 L CNN
F 1 "220" V 3500 6850 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3430 6850 50  0001 C CNN
F 3 "~" H 3500 6850 50  0001 C CNN
	1    3500 6850
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR016
U 1 1 665AB6FD
P 3950 6850
F 0 "#PWR016" H 3950 6600 50  0001 C CNN
F 1 "GND" V 3950 6650 50  0000 C CNN
F 2 "" H 3950 6850 50  0001 C CNN
F 3 "" H 3950 6850 50  0001 C CNN
	1    3950 6850
	0    -1   -1   0   
$EndComp
Text GLabel 3350 6850 0    50   Input ~ 0
LED_ALIVE
Text GLabel 4000 4850 2    50   Input ~ 0
RPI_TX
Text GLabel 4000 4750 2    50   Input ~ 0
RPI_RX
Text GLabel 3000 4750 0    50   Input ~ 0
T4_RX1
Text GLabel 3000 4650 0    50   Input ~ 0
T4_TX1
$EndSCHEMATC
