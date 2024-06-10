EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr B 17000 11000
encoding utf-8
Sheet 1 6
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	8250 6250 8250 6850
Wire Wire Line
	2250 6250 8250 6250
Wire Wire Line
	7750 1500 7750 1950
Wire Wire Line
	7450 1950 7450 1500
Text GLabel 7450 1500 1    50   BiDi ~ 0
IPAD
Wire Wire Line
	2500 3850 2500 2950
Wire Wire Line
	8100 3200 8100 3950
Wire Wire Line
	7150 3200 8100 3200
Wire Wire Line
	7150 2800 7150 3200
Wire Wire Line
	5000 3200 5000 3850
Wire Wire Line
	6650 3200 5000 3200
Wire Wire Line
	6650 2800 6650 3200
$Sheet
S 6150 1950 1850 850 
U 6665D5A7
F0 "hub" 50
F1 "hub.sch" 50
F2 "USB_IN1" B B 6650 2800 50 
F3 "USB_IN2" B B 7150 2800 50 
F4 "USB_IN3" B T 7750 1950 50 
F5 "USB_HUB" B T 7450 1950 50 
F6 "HUB_POWER" B T 6400 1950 50 
$EndSheet
Text GLabel 7750 1500 1    50   BiDi ~ 0
FTP
Text GLabel 2500 2950 1    50   BiDi ~ 0
E_USB
Text GLabel 9750 7850 3    50   BiDi ~ 0
R_DISPLAY
Text GLabel 5150 5250 3    50   BiDi ~ 0
P_DISPLAY
Wire Wire Line
	9300 5300 9300 6850
Wire Wire Line
	2250 4900 2250 6250
Wire Wire Line
	8250 5800 8250 5300
Wire Wire Line
	5800 5800 8250 5800
Wire Wire Line
	5800 5250 5800 5800
Wire Wire Line
	4800 5800 4800 5250
Wire Wire Line
	2750 5800 4800 5800
Wire Wire Line
	2750 4900 2750 5800
$Sheet
S 7950 6850 3150 1000
U 6665949E
F0 "rPi" 50
F1 "rPi.sch" 50
F2 "R_SERIAL" B T 8250 6850 50 
F3 "R_I2S" B T 9300 6850 50 
F4 "R_DISPLAY" B B 9750 7850 50 
$EndSheet
$Sheet
S 4500 3850 1900 1400
U 66659584
F0 "pedal" 50
F1 "pedal.sch" 50
F2 "P_DISPLAY" B B 5150 5250 50 
F3 "P_USB" B T 5000 3850 50 
F4 "P_SERIAL_1" B B 4800 5250 50 
F5 "P_SERIAL_2" B B 5800 5250 50 
$EndSheet
$Sheet
S 7850 3950 1900 1350
U 66659540
F0 "audio" 50
F1 "audio.sch" 50
F2 "A_USB" B T 8100 3950 50 
F3 "A_SERIAL" B B 8250 5300 50 
F4 "A_I2S" B B 9300 5300 50 
$EndSheet
$Sheet
S 1800 3850 1450 1050
U 66659401
F0 "ESP32" 50
F1 "esp32.sch" 50
F2 "E_USB" B T 2500 3850 50 
F3 "E_SERIAL_1" B B 2250 4900 50 
F4 "E_SERIAL_2" B B 2750 4900 50 
$EndSheet
$EndSCHEMATC
