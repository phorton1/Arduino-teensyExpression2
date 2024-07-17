EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr B 17000 11000
encoding utf-8
Sheet 1 9
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 3750 1100 1    50   BiDi ~ 0
IPAD
$Sheet
S 3250 1250 1000 850 
U 6665D5A7
F0 "hub" 50
F1 "hub.sch" 50
F2 "USB_IN1" B B 3650 2100 50 
F3 "USB_IN2" B B 3850 2100 50 
F4 "USB_IN3" B T 4000 1250 50 
F5 "USB_HUB" B T 3750 1250 50 
F6 "HUB_POWER" B T 3500 1250 50 
$EndSheet
Text GLabel 4000 1100 1    50   BiDi ~ 0
FTP
Text GLabel 1500 1150 1    50   BiDi ~ 0
E_USB
Text GLabel 3950 4450 3    50   BiDi ~ 0
R_DISPLAY
Text GLabel 3150 4450 3    50   BiDi ~ 0
P_DISPLAY
$Sheet
S 3600 3700 1150 550 
U 6665949E
F0 "rPi" 50
F1 "rPi.sch" 50
F2 "R_SERIAL" B T 4150 3700 50 
F3 "R_I2S" B T 4550 3700 50 
F4 "R_DISPLAY" B B 3950 4250 50 
F5 "5V" I T 3800 3700 50 
$EndSheet
$Sheet
S 2400 2500 1200 600 
U 66659584
F0 "pedal" 50
F1 "pedal.sch" 50
F2 "P_DISPLAY" B B 3150 3100 50 
F3 "P_USB" B T 3000 2500 50 
F4 "P_SERIAL_1" B B 2550 3100 50 
F5 "P_SERIAL_2" B B 3450 3100 50 
F6 "5V" I T 2700 2500 50 
F7 "ROT" B B 2700 3100 50 
F8 "ROW_OUT" B B 2850 3100 50 
$EndSheet
$Sheet
S 3900 2500 850  800 
U 66659540
F0 "audio" 50
F1 "audio.sch" 50
F2 "A_USB" B T 4300 2500 50 
F3 "A_SERIAL_1" B B 4000 3300 50 
F4 "A_I2S" B B 4550 3300 50 
F5 "A_SERIAL_2" B B 4150 3300 50 
F6 "5V" I T 4550 2500 50 
$EndSheet
$Sheet
S 950  2500 1050 600 
U 66659401
F0 "ESP32" 50
F1 "esp32.sch" 50
F2 "E_USB" B T 1500 2500 50 
F3 "E_SERIAL_1" B B 1200 3100 50 
F4 "E_SERIAL_2" B B 1800 3100 50 
F5 "5V" I T 1650 2500 50 
$EndSheet
Wire Wire Line
	4000 3450 4000 3300
Wire Wire Line
	4150 3300 4150 3700
Wire Wire Line
	4550 3300 4550 3700
Wire Wire Line
	3000 2500 3000 2300
Wire Wire Line
	3000 2300 3650 2300
Wire Wire Line
	3650 2300 3650 2100
Wire Wire Line
	4300 2500 4300 2300
Wire Wire Line
	4300 2300 3850 2300
Wire Wire Line
	3850 2300 3850 2100
Wire Wire Line
	3750 1100 3750 1250
Wire Wire Line
	4000 1100 4000 1250
Wire Wire Line
	3150 3100 3150 4450
Wire Wire Line
	3950 4200 3950 4450
Wire Wire Line
	1500 1150 1500 2500
Wire Wire Line
	4000 3450 3450 3450
Wire Wire Line
	2550 3100 2550 3250
Text GLabel 4550 2300 1    50   Input ~ 0
5V
Text GLabel 3400 3600 0    50   Input ~ 0
5V
Wire Wire Line
	3400 3600 3800 3600
Wire Wire Line
	3800 3600 3800 3700
Wire Wire Line
	4550 2300 4550 2500
Text GLabel 2700 2300 1    50   Input ~ 0
5V
Text GLabel 1650 2300 1    50   Input ~ 0
5V
Wire Wire Line
	1650 2300 1650 2500
Wire Wire Line
	2700 2300 2700 2500
$Sheet
S 1950 1300 600  600 
U 66682976
F0 "power" 50
F1 "power.sch" 50
F2 "USB_POWER" I T 2100 1300 50 
F3 "5V" O B 2250 1900 50 
F4 "HUB_POWER" O T 2400 1300 50 
$EndSheet
Text GLabel 2100 1150 1    50   Input ~ 0
USB_POWER
Wire Wire Line
	2100 1150 2100 1300
Wire Wire Line
	2400 1300 2400 1150
Wire Wire Line
	2400 1150 3500 1150
Wire Wire Line
	3500 1150 3500 1250
Text GLabel 2250 2000 3    50   Output ~ 0
5V
Wire Wire Line
	2250 1900 2250 2000
Wire Wire Line
	3450 3100 3450 3450
Wire Wire Line
	1200 3250 2550 3250
Wire Wire Line
	1200 3250 1200 3100
$Sheet
S 950  3400 600  500 
U 66687B45
F0 "rotaries" 50
F1 "rotaries.sch" 50
F2 "ROT" B R 1550 3650 50 
$EndSheet
$Sheet
S 1800 3700 1200 500 
U 66687BE1
F0 "row_board" 50
F1 "row_board.sch" 50
$EndSheet
Text GLabel 2700 4450 3    50   Input ~ 0
ROW_OUT
Wire Wire Line
	1550 3650 1700 3650
Wire Wire Line
	1700 3650 1700 3400
Wire Wire Line
	1700 3400 2700 3400
Wire Wire Line
	2700 3400 2700 3100
Text HLabel 2700 4200 1    50   BiDi ~ 0
ROW_OUT
Text HLabel 2850 3700 3    50   BiDi ~ 0
ROW_IN
Wire Wire Line
	2850 3100 2850 3700
Wire Wire Line
	2700 4200 2700 4450
$EndSCHEMATC
