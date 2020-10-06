EESchema Schematic File Version 5
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title ""
Date "12 mar 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
Comment5 ""
Comment6 ""
Comment7 ""
Comment8 ""
Comment9 ""
$EndDescr
$Comp
L atmegax8_with_F-3188-rescue:+5V-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR01
U 1 1 53207304
P 10300 5775
F 0 "#PWR01" H 10300 5865 20  0001 C CNN
F 1 "+5V" H 10300 5865 30  0000 C CNN
F 2 "" H 10300 5775 60  0001 C CNN
F 3 "" H 10300 5775 60  0001 C CNN
	1    10300 5775
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R3
U 1 1 532072FD
P 11025 6275
F 0 "R3" V 11233 6275 50  0000 C CNN
F 1 "100" V 11141 6275 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 11025 6275 60  0001 C CNN
F 3 "" H 11025 6275 60  0001 C CNN
	1    11025 6275
	0    -1   -1   0   
$EndComp
Text Label 3500 7500 2    60   ~ 0
DataIn
Text Label 3500 8350 2    60   ~ 0
Clock
Text Label 4450 6350 2    60   ~ 0
DataOut
$Comp
L atmegax8_with_F-3188-rescue:+12V-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR02
U 1 1 530C8041
P 1700 9550
F 0 "#PWR02" H 1700 9500 20  0001 C CNN
F 1 "+12V" H 1700 9650 30  0000 C CNN
F 2 "" H 1700 9550 60  0001 C CNN
F 3 "" H 1700 9550 60  0001 C CNN
	1    1700 9550
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:+5V-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR03
U 1 1 530C8014
P 14075 8025
F 0 "#PWR03" H 14075 8115 20  0001 C CNN
F 1 "+5V" H 14075 8115 30  0000 C CNN
F 2 "" H 14075 8025 60  0001 C CNN
F 3 "" H 14075 8025 60  0001 C CNN
	1    14075 8025
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C4
U 1 1 530C7FE2
P 14075 8325
F 0 "C4" H 14125 8425 50  0000 L CNN
F 1 "100n" H 14125 8225 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 14075 8325 60  0001 C CNN
F 3 "" H 14075 8325 60  0001 C CNN
	1    14075 8325
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C3
U 1 1 530C7FD9
P 12725 8325
F 0 "C3" H 12840 8371 50  0000 L CNN
F 1 "100n" H 12840 8279 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 12725 8325 60  0001 C CNN
F 3 "" H 12725 8325 60  0001 C CNN
	1    12725 8325
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:+5V-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR04
U 1 1 530C77C0
P 1000 6150
F 0 "#PWR04" H 1000 6240 20  0001 C CNN
F 1 "+5V" H 1000 6240 30  0000 C CNN
F 2 "" H 1000 6150 60  0001 C CNN
F 3 "" H 1000 6150 60  0001 C CNN
	1    1000 6150
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:ATMEGA88-A-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue IC1
U 1 1 530C76C5
P 1950 7450
F 0 "IC1" H 1250 8700 50  0000 L BNN
F 1 "ATMEGA88-A" H 2250 6050 50  0000 L BNN
F 2 "Housings_QFP:TQFP-32_7x7mm_Pitch0.8mm" H 1400 6100 50  0001 C CNN
F 3 "" H 1950 7450 60  0001 C CNN
	1    1950 7450
	1    0    0    -1  
$EndComp
NoConn ~ 2950 8250
NoConn ~ 2950 8350
NoConn ~ 2950 8450
NoConn ~ 2950 8550
NoConn ~ 2950 8650
NoConn ~ 2950 7700
NoConn ~ 2950 7600
NoConn ~ 2950 7400
NoConn ~ 2950 7300
NoConn ~ 2950 7200
NoConn ~ 2950 7050
NoConn ~ 2950 6950
NoConn ~ 2950 6550
NoConn ~ 2950 6450
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR06
U 1 1 564E2443
P 1000 8800
F 0 "#PWR06" H 1000 8550 50  0001 C CNN
F 1 "GND" H 1000 8650 50  0000 C CNN
F 2 "" H 1000 8800 60  0000 C CNN
F 3 "" H 1000 8800 60  0000 C CNN
	1    1000 8800
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR07
U 1 1 564E2722
P 5350 7600
F 0 "#PWR07" H 5350 7350 50  0001 C CNN
F 1 "GND" H 5350 7450 50  0000 C CNN
F 2 "" H 5350 7600 60  0000 C CNN
F 3 "" H 5350 7600 60  0000 C CNN
	1    5350 7600
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR08
U 1 1 564E291B
P 12725 8725
F 0 "#PWR08" H 12725 8475 50  0001 C CNN
F 1 "GND" H 12725 8575 50  0000 C CNN
F 2 "" H 12725 8725 60  0000 C CNN
F 3 "" H 12725 8725 60  0000 C CNN
	1    12725 8725
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR09
U 1 1 564E2BE7
P 13400 8725
F 0 "#PWR09" H 13400 8475 50  0001 C CNN
F 1 "GND" H 13400 8575 50  0000 C CNN
F 2 "" H 13400 8725 60  0000 C CNN
F 3 "" H 13400 8725 60  0000 C CNN
	1    13400 8725
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR010
U 1 1 564E2C19
P 14075 8725
F 0 "#PWR010" H 14075 8475 50  0001 C CNN
F 1 "GND" H 14075 8575 50  0000 C CNN
F 2 "" H 14075 8725 60  0000 C CNN
F 3 "" H 14075 8725 60  0000 C CNN
	1    14075 8725
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR012
U 1 1 564E32CC
P 10300 6850
F 0 "#PWR012" H 10300 6600 50  0001 C CNN
F 1 "GND" H 10300 6700 50  0000 C CNN
F 2 "" H 10300 6850 60  0000 C CNN
F 3 "" H 10300 6850 60  0000 C CNN
	1    10300 6850
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:D-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue D1
U 1 1 568A5843
P 14375 8125
F 0 "D1" H 14375 7909 50  0000 C CNN
F 1 "1N4148" H 14375 8001 50  0000 C CNN
F 2 "Diodes_SMD:D_MiniMELF" H 14375 8125 50  0001 C CNN
F 3 "" H 14375 8125 50  0000 C CNN
	1    14375 8125
	-1   0    0    1   
$EndComp
Text Label 11510 2360 0    60   ~ 0
BT_RESET
Text Label 11450 1260 0    60   ~ 0
LED1
Text Label 11450 1160 0    60   ~ 0
LED2
$Comp
L atmegax8_with_F-3188-rescue:+BATT-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR014
U 1 1 568BDD77
P 14675 8025
F 0 "#PWR014" H 14675 7875 50  0001 C CNN
F 1 "+BATT" H 14693 8199 50  0000 C CNN
F 2 "" H 14675 8025 50  0000 C CNN
F 3 "" H 14675 8025 50  0000 C CNN
	1    14675 8025
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C5
U 1 1 568BEA89
P 14675 8325
F 0 "C5" H 14790 8371 50  0000 L CNN
F 1 "100n" H 14790 8279 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 14675 8325 60  0001 C CNN
F 3 "" H 14675 8325 60  0001 C CNN
	1    14675 8325
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR016
U 1 1 568BEBF3
P 14675 8725
F 0 "#PWR016" H 14675 8475 50  0001 C CNN
F 1 "GND" H 14675 8575 50  0000 C CNN
F 2 "" H 14675 8725 60  0000 C CNN
F 3 "" H 14675 8725 60  0000 C CNN
	1    14675 8725
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R7
U 1 1 568BF2EB
P 8775 6325
F 0 "R7" V 8983 6325 50  0000 C CNN
F 1 "100" V 8891 6325 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 8775 6325 60  0001 C CNN
F 3 "" H 8775 6325 60  0001 C CNN
	1    8775 6325
	0    -1   -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR017
U 1 1 568BFAAC
P 8100 6975
F 0 "#PWR017" H 8100 6725 50  0001 C CNN
F 1 "GND" H 8100 6825 50  0000 C CNN
F 2 "" H 8100 6975 60  0000 C CNN
F 3 "" H 8100 6975 60  0000 C CNN
	1    8100 6975
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:+BATT-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR018
U 1 1 568BFDFB
P 8100 5725
F 0 "#PWR018" H 8100 5575 50  0001 C CNN
F 1 "+BATT" H 8118 5899 50  0000 C CNN
F 2 "" H 8100 5725 50  0000 C CNN
F 3 "" H 8100 5725 50  0000 C CNN
	1    8100 5725
	1    0    0    -1  
$EndComp
Text Label 3550 7800 2    60   ~ 0
ARESET
Text Label 9425 6325 2    60   ~ 0
BT_RESET
Text Label 11575 6275 2    60   ~ 0
ARESET
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R6
U 1 1 568C1AD8
P 4400 7200
F 0 "R6" V 4300 7200 50  0000 C CNN
F 1 "1M" V 4400 7200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 4400 7200 60  0001 C CNN
F 3 "" H 4400 7200 60  0001 C CNN
	1    4400 7200
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:CONN_02X03-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue P2
U 1 1 568C595C
P 9550 10100
F 0 "P2" H 9550 10508 50  0000 C CNN
F 1 "SPI" H 9550 10416 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03" H 9550 10324 50  0001 C CNN
F 3 "" H 9550 8900 50  0000 C CNN
	1    9550 10100
	1    0    0    -1  
$EndComp
Text Label 8700 10200 0    60   ~ 0
ARESET
$Comp
L atmegax8_with_F-3188-rescue:+5V-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR019
U 1 1 568C5F45
P 10000 9800
F 0 "#PWR019" H 10000 9890 20  0001 C CNN
F 1 "+5V" H 10000 9890 30  0000 C CNN
F 2 "" H 10000 9800 60  0001 C CNN
F 3 "" H 10000 9800 60  0001 C CNN
	1    10000 9800
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR020
U 1 1 568C5F95
P 10000 10350
F 0 "#PWR020" H 10000 10100 50  0001 C CNN
F 1 "GND" H 10000 10200 50  0000 C CNN
F 2 "" H 10000 10350 60  0000 C CNN
F 3 "" H 10000 10350 60  0000 C CNN
	1    10000 10350
	1    0    0    -1  
$EndComp
Text Label 3200 6650 2    60   ~ 0
MOSI
Text Label 3200 6750 2    60   ~ 0
MISO
Text Label 3200 6850 2    60   ~ 0
SCK
Text Label 10250 10100 2    60   ~ 0
MOSI
Text Label 8700 10000 0    60   ~ 0
MISO
Text Label 8700 10100 0    60   ~ 0
SCK
$Comp
L atmegax8_with_F-3188-rescue:INDUCTOR-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue L1
U 1 1 568E3F8D
P 2100 9700
F 0 "L1" V 2317 9700 50  0000 C CNN
F 1 "330u" V 2225 9700 50  0000 C CNN
F 2 "SMD_Packages:SMD-1210_Pol" H 2100 9700 50  0001 C CNN
F 3 "" H 2100 9700 50  0000 C CNN
	1    2100 9700
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R16
U 1 1 568E4746
P 3150 9900
F 0 "R16" H 3080 9854 50  0000 R CNN
F 1 "100k" H 3080 9946 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3150 9900 60  0001 C CNN
F 3 "" H 3150 9900 60  0001 C CNN
	1    3150 9900
	-1   0    0    1   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R17
U 1 1 568E4B22
P 3150 10300
F 0 "R17" H 3080 10254 50  0000 R CNN
F 1 "100k" H 3080 10346 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3150 10300 60  0001 C CNN
F 3 "" H 3150 10300 60  0001 C CNN
	1    3150 10300
	-1   0    0    1   
$EndComp
Text Label 3450 10100 2    60   ~ 0
VGND
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C12
U 1 1 568E5AE8
P 2800 9900
F 0 "C12" H 2915 9946 50  0000 L CNN
F 1 "100n" H 2600 9700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2800 9900 60  0001 C CNN
F 3 "" H 2800 9900 60  0001 C CNN
	1    2800 9900
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:CP-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C11
U 1 1 568E5BCE
P 2500 9900
F 0 "C11" H 2615 9946 50  0000 L CNN
F 1 "100m/16V" H 2100 9650 50  0000 L CNN
F 2 "Capacitors_Tantalum_SMD:CP_Tantalum_Case-D_EIA-7343-31_Hand" H 2500 9900 60  0001 C CNN
F 3 "" H 2500 9900 60  0001 C CNN
	1    2500 9900
	1    0    0    -1  
$EndComp
Text Label 7400 9350 0    60   ~ 0
AOUTL
Text Label 7400 9550 0    60   ~ 0
AOUTR
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR021
U 1 1 568E826A
P 7250 9200
F 0 "#PWR021" H 7250 8950 50  0001 C CNN
F 1 "GNDA" H 7250 9050 50  0000 C CNN
F 2 "" H 7250 9200 50  0000 C CNN
F 3 "" H 7250 9200 50  0000 C CNN
	1    7250 9200
	-1   0    0    1   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:CP-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C13
U 1 1 568E8D5E
P 3000 10300
F 0 "C13" H 2900 10500 50  0000 L CNN
F 1 "2m2/16V" H 2550 9950 50  0000 L CNN
F 2 "Capacitors_Tantalum_SMD:TantalC_SizeA_EIA-3216_HandSoldering" H 3000 10300 60  0001 C CNN
F 3 "" H 3000 10300 60  0001 C CNN
	1    3000 10300
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:CP-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C6
U 1 1 568EA3BA
P 4475 1500
F 0 "C6" H 4125 1550 50  0000 L CNN
F 1 "10m/16V" H 3975 1450 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_5x5.8" H 4475 1500 60  0001 C CNN
F 3 "" H 4475 1500 60  0001 C CNN
	1    4475 1500
	0    1    -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R8
U 1 1 568EB654
P 3875 1500
F 0 "R8" V 3975 1450 50  0000 R CNN
F 1 "22k" V 3975 1650 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3875 1500 60  0001 C CNN
F 3 "" H 3875 1500 60  0001 C CNN
	1    3875 1500
	0    -1   1    0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:+12C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR023
U 1 1 568EC99B
P 2500 9550
F 0 "#PWR023" H 2500 9400 50  0001 C CNN
F 1 "+12C" H 2518 9724 50  0000 C CNN
F 2 "" H 2500 9550 50  0000 C CNN
F 3 "" H 2500 9550 50  0000 C CNN
	1    2500 9550
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR025
U 1 1 568F3D61
P 2500 10300
F 0 "#PWR025" H 2500 10050 50  0001 C CNN
F 1 "GNDA" H 2508 10126 50  0000 C CNN
F 2 "" H 2500 10300 50  0000 C CNN
F 3 "" H 2500 10300 50  0000 C CNN
	1    2500 10300
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR026
U 1 1 568F4180
P 2800 10300
F 0 "#PWR026" H 2800 10050 50  0001 C CNN
F 1 "GNDA" H 2808 10126 50  0000 C CNN
F 2 "" H 2800 10300 50  0000 C CNN
F 3 "" H 2800 10300 50  0000 C CNN
	1    2800 10300
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR027
U 1 1 568F4263
P 3150 10550
F 0 "#PWR027" H 3150 10300 50  0001 C CNN
F 1 "GNDA" H 3158 10376 50  0000 C CNN
F 2 "" H 3150 10550 50  0000 C CNN
F 3 "" H 3150 10550 50  0000 C CNN
	1    3150 10550
	1    0    0    -1  
$EndComp
Text Label 5225 3600 2    60   ~ 0
BTOUTL
Text Label 1075 1400 0    60   ~ 0
AOUTL
Text Label 3800 8250 2    60   ~ 0
CDC_ENA_5V
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R13
U 1 1 568FCD89
P 4200 7950
F 0 "R13" V 4300 7950 50  0000 C CNN
F 1 "330" V 4200 7950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 4200 7950 60  0001 C CNN
F 3 "" H 4200 7950 60  0001 C CNN
	1    4200 7950
	0    -1   -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R14
U 1 1 568FCFF8
P 4200 8050
F 0 "R14" V 4100 8050 50  0000 C CNN
F 1 "330" V 4200 8050 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 4200 8050 60  0001 C CNN
F 3 "" H 4200 8050 60  0001 C CNN
	1    4200 8050
	0    -1   -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R19
U 1 1 568FD093
P 4400 8300
F 0 "R19" H 4470 8346 50  0000 L CNN
F 1 "4k7" H 4470 8254 50  0000 L CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 4400 8300 60  0001 C CNN
F 3 "" H 4400 8300 60  0001 C CNN
	1    4400 8300
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR028
U 1 1 568FE2CC
P 4400 8550
F 0 "#PWR028" H 4400 8300 50  0001 C CNN
F 1 "GND" H 4400 8400 50  0000 C CNN
F 2 "" H 4400 8550 60  0000 C CNN
F 3 "" H 4400 8550 60  0000 C CNN
	1    4400 8550
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:CP-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C21
U 1 1 5691459B
P 4475 1300
F 0 "C21" H 4125 1350 50  0000 L CNN
F 1 "10m/16V" H 3975 1250 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_5x5.8" H 4475 1300 60  0001 C CNN
F 3 "" H 4475 1300 60  0001 C CNN
	1    4475 1300
	0    -1   1    0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R23
U 1 1 56914B95
P 3875 1300
F 0 "R23" V 3975 1250 50  0000 R CNN
F 1 "22k" V 3975 1450 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3875 1300 60  0001 C CNN
F 3 "" H 3875 1300 60  0001 C CNN
	1    3875 1300
	0    1    -1   0   
$EndComp
Text Label 5275 2100 2    60   ~ 0
VGND
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R18
U 1 1 569192E5
P 3025 2100
F 0 "R18" V 3125 2050 50  0000 R CNN
F 1 "47k" V 3125 2250 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3025 2100 60  0001 C CNN
F 3 "" H 3025 2100 60  0001 C CNN
	1    3025 2100
	0    1    -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:CP-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C14
U 1 1 569195E0
P 2025 1400
F 0 "C14" H 1675 1450 50  0000 L CNN
F 1 "47m/25V" H 1525 1350 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x5.7" H 2025 1400 60  0001 C CNN
F 3 "" H 2025 1400 60  0001 C CNN
	1    2025 1400
	0    1    -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C16
U 1 1 5691C62D
P 3025 2350
F 0 "C16" V 3075 2175 50  0000 C CNN
F 1 "33p" V 3100 2525 50  0000 C CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3025 2350 60  0001 C CNN
F 3 "" H 3025 2350 60  0001 C CNN
	1    3025 2350
	0    -1   -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C18
U 1 1 5691C948
P 3875 2350
F 0 "C18" V 3925 2150 50  0000 C CNN
F 1 "33p" V 3925 2550 50  0000 C CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3875 2350 60  0001 C CNN
F 3 "" H 3875 2350 60  0001 C CNN
	1    3875 2350
	0    -1   -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:+12C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR030
U 1 1 5691DAFA
P 6275 1825
F 0 "#PWR030" H 6275 1675 50  0001 C CNN
F 1 "+12C" H 6293 1999 50  0000 C CNN
F 2 "" H 6275 1825 50  0000 C CNN
F 3 "" H 6275 1825 50  0000 C CNN
	1    6275 1825
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR031
U 1 1 5691E5A3
P 6275 2525
F 0 "#PWR031" H 6275 2275 50  0001 C CNN
F 1 "GNDA" H 6283 2351 50  0000 C CNN
F 2 "" H 6275 2525 50  0000 C CNN
F 3 "" H 6275 2525 50  0000 C CNN
	1    6275 2525
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R12
U 1 1 5691ED5C
P 1725 1800
F 0 "R12" V 1825 1750 50  0000 R CNN
F 1 "47k" V 1825 1950 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 1725 1800 60  0001 C CNN
F 3 "" H 1725 1800 60  0001 C CNN
	1    1725 1800
	1    0    0    1   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR032
U 1 1 5691F79E
P 1725 2200
F 0 "#PWR032" H 1725 1950 50  0001 C CNN
F 1 "GNDA" H 1733 2026 50  0000 C CNN
F 2 "" H 1725 2200 50  0000 C CNN
F 3 "" H 1725 2200 50  0000 C CNN
	1    1725 2200
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C8
U 1 1 5691F908
P 1375 2150
F 0 "C8" V 1628 2150 50  0000 C CNN
F 1 "100n" V 1536 2150 50  0000 C CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1375 2150 60  0001 C CNN
F 3 "" H 1375 2150 60  0001 C CNN
	1    1375 2150
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R10
U 1 1 5691FE26
P 1375 1750
F 0 "R10" V 1475 1700 50  0000 R CNN
F 1 "39R" V 1475 1900 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 1375 1750 60  0001 C CNN
F 3 "" H 1375 1750 60  0001 C CNN
	1    1375 1750
	1    0    0    1   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR033
U 1 1 56920349
P 1375 2500
F 0 "#PWR033" H 1375 2250 50  0001 C CNN
F 1 "GNDA" H 1383 2326 50  0000 C CNN
F 2 "" H 1375 2500 50  0000 C CNN
F 3 "" H 1375 2500 50  0000 C CNN
	1    1375 2500
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:CP-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C20
U 1 1 56921143
P 4425 3600
F 0 "C20" H 4075 3650 50  0000 L CNN
F 1 "10m/16V" H 3925 3550 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_5x5.8" H 4425 3600 60  0001 C CNN
F 3 "" H 4425 3600 60  0001 C CNN
	1    4425 3600
	0    1    -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R21
U 1 1 56921149
P 3825 3600
F 0 "R21" V 3925 3550 50  0000 R CNN
F 1 "22k" V 3925 3750 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3825 3600 60  0001 C CNN
F 3 "" H 3825 3600 60  0001 C CNN
	1    3825 3600
	0    -1   1    0   
$EndComp
Text Label 5275 1500 2    60   ~ 0
BTOUTR
$Comp
L atmegax8_with_F-3188-rescue:CP-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C19
U 1 1 56921151
P 4425 3400
F 0 "C19" H 4075 3450 50  0000 L CNN
F 1 "10m/16V" H 3925 3350 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_5x5.8" H 4425 3400 60  0001 C CNN
F 3 "" H 4425 3400 60  0001 C CNN
	1    4425 3400
	0    -1   1    0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R20
U 1 1 56921157
P 3825 3400
F 0 "R20" V 3925 3350 50  0000 R CNN
F 1 "22k" V 3925 3550 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3825 3400 60  0001 C CNN
F 3 "" H 3825 3400 60  0001 C CNN
	1    3825 3400
	0    1    -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R22
U 1 1 5692115D
P 3825 4200
F 0 "R22" V 3925 4150 50  0000 R CNN
F 1 "47k" V 3925 4350 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3825 4200 60  0001 C CNN
F 3 "" H 3825 4200 60  0001 C CNN
	1    3825 4200
	0    1    -1   0   
$EndComp
Text Label 5225 4200 2    60   ~ 0
VGND
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R15
U 1 1 56921165
P 2975 4200
F 0 "R15" V 3075 4150 50  0000 R CNN
F 1 "47k" V 3075 4350 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 2975 4200 60  0001 C CNN
F 3 "" H 2975 4200 60  0001 C CNN
	1    2975 4200
	0    1    -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:CP-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C9
U 1 1 5692116B
P 1975 3500
F 0 "C9" H 1625 3550 50  0000 L CNN
F 1 "47m/25V" H 1475 3450 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x5.7" H 1975 3500 60  0001 C CNN
F 3 "" H 1975 3500 60  0001 C CNN
	1    1975 3500
	0    1    -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C15
U 1 1 56921182
P 2975 4450
F 0 "C15" V 3228 4450 50  0000 C CNN
F 1 "33p" V 3136 4450 50  0000 C CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2975 4450 60  0001 C CNN
F 3 "" H 2975 4450 60  0001 C CNN
	1    2975 4450
	0    -1   -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C17
U 1 1 56921188
P 3825 4450
F 0 "C17" V 4078 4450 50  0000 C CNN
F 1 "33p" V 3986 4450 50  0000 C CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3825 4450 60  0001 C CNN
F 3 "" H 3825 4450 60  0001 C CNN
	1    3825 4450
	0    -1   -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R11
U 1 1 569211A6
P 1675 3900
F 0 "R11" V 1775 3850 50  0000 R CNN
F 1 "47k" V 1775 4050 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 1675 3900 60  0001 C CNN
F 3 "" H 1675 3900 60  0001 C CNN
	1    1675 3900
	1    0    0    1   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR036
U 1 1 569211AE
P 1675 4300
F 0 "#PWR036" H 1675 4050 50  0001 C CNN
F 1 "GNDA" H 1683 4126 50  0000 C CNN
F 2 "" H 1675 4300 50  0000 C CNN
F 3 "" H 1675 4300 50  0000 C CNN
	1    1675 4300
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C7
U 1 1 569211B5
P 1325 4250
F 0 "C7" V 1578 4250 50  0000 C CNN
F 1 "100n" V 1486 4250 50  0000 C CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1325 4250 60  0001 C CNN
F 3 "" H 1325 4250 60  0001 C CNN
	1    1325 4250
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R9
U 1 1 569211BB
P 1325 3850
F 0 "R9" V 1425 3800 50  0000 R CNN
F 1 "39R" V 1425 4000 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 1325 3850 60  0001 C CNN
F 3 "" H 1325 3850 60  0001 C CNN
	1    1325 3850
	1    0    0    1   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR037
U 1 1 569211C4
P 1325 4600
F 0 "#PWR037" H 1325 4350 50  0001 C CNN
F 1 "GNDA" H 1333 4426 50  0000 C CNN
F 2 "" H 1325 4600 50  0000 C CNN
F 3 "" H 1325 4600 50  0000 C CNN
	1    1325 4600
	1    0    0    -1  
$EndComp
Text Label 1025 3500 0    60   ~ 0
AOUTR
$Comp
L Device:CP C22
U 1 1 5692995E
P 8075 1825
F 0 "C22" H 8190 1871 50  0000 L CNN
F 1 "4u7/10V" H 8190 1779 50  0000 L CNN
F 2 "Capacitors_Tantalum_SMD.pretty:TantalC_SizeA_EIA-3216_Wave" H 8075 1825 60  0001 C CNN
F 3 "" H 8075 1825 60  0001 C CNN
	1    8075 1825
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R27
U 1 1 56929A5F
P 9175 1575
F 0 "R27" V 9383 1575 50  0000 C CNN
F 1 "1k2" V 9291 1575 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 9175 1575 60  0001 C CNN
F 3 "" H 9175 1575 60  0001 C CNN
	1    9175 1575
	0    1    -1   0   
$EndComp
$Comp
L Device:C C23
U 1 1 5692A8F6
P 8525 2425
F 0 "C23" V 8778 2425 50  0000 C CNN
F 1 "1u/10V" V 8686 2425 50  0000 C CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 8525 2425 60  0001 C CNN
F 3 "" H 8525 2425 60  0001 C CNN
	1    8525 2425
	0    -1   -1   0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR038
U 1 1 5692D417
P 8075 2025
F 0 "#PWR038" H 8075 1775 50  0001 C CNN
F 1 "GND" H 8075 1875 50  0000 C CNN
F 2 "" H 8075 2025 60  0000 C CNN
F 3 "" H 8075 2025 60  0000 C CNN
	1    8075 2025
	1    0    0    -1  
$EndComp
Text Label 7525 2425 0    60   ~ 0
MICP
Text Label 7525 1575 0    60   ~ 0
MIC_BIAS
Text Label 10275 2825 2    60   ~ 0
MIC_IN1
Text Label 5225 3400 2    60   ~ 0
BTOUTLN
Text Label 5275 1300 2    60   ~ 0
BTOUTRN
$Comp
L Device:C C27
U 1 1 59FC8340
P 8525 2825
F 0 "C27" V 8778 2825 50  0000 C CNN
F 1 "1u/10V" V 8686 2825 50  0000 C CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 8525 2825 60  0001 C CNN
F 3 "" H 8525 2825 60  0001 C CNN
	1    8525 2825
	0    -1   -1   0   
$EndComp
Text Label 7525 2825 0    60   ~ 0
MICN
Text Label 10275 2425 2    60   ~ 0
MIC_IN2
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C26
U 1 1 59FC8E51
P 8025 2625
F 0 "C26" V 8278 2625 50  0000 C CNN
F 1 "10n" V 8186 2625 50  0000 C CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8025 2625 60  0001 C CNN
F 3 "" H 8025 2625 60  0001 C CNN
	1    8025 2625
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C28
U 1 1 59FC9441
P 8675 1825
F 0 "C28" V 8928 1825 50  0000 C CNN
F 1 "100n" V 8836 1825 50  0000 C CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8675 1825 60  0001 C CNN
F 3 "" H 8675 1825 60  0001 C CNN
	1    8675 1825
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR013
U 1 1 59FCA7CB
P 8675 2025
F 0 "#PWR013" H 8675 1775 50  0001 C CNN
F 1 "GND" H 8675 1875 50  0000 C CNN
F 2 "" H 8675 2025 60  0000 C CNN
F 3 "" H 8675 2025 60  0000 C CNN
	1    8675 2025
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R25
U 1 1 59FCBD26
P 9225 3175
F 0 "R25" V 9433 3175 50  0000 C CNN
F 1 "1k2" V 9341 3175 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 9225 3175 60  0001 C CNN
F 3 "" H 9225 3175 60  0001 C CNN
	1    9225 3175
	1    0    0    1   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR029
U 1 1 59FCBF76
P 9225 3425
F 0 "#PWR029" H 9225 3175 50  0001 C CNN
F 1 "GND" H 9225 3275 50  0000 C CNN
F 2 "" H 9225 3425 60  0000 C CNN
F 3 "" H 9225 3425 60  0000 C CNN
	1    9225 3425
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR043
U 1 1 59FCDFAF
P 11970 3210
F 0 "#PWR043" H 11970 2960 50  0001 C CNN
F 1 "GND" H 11970 3060 50  0000 C CNN
F 2 "" H 11970 3210 60  0000 C CNN
F 3 "" H 11970 3210 60  0000 C CNN
	1    11970 3210
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 7500 3500 7500
Wire Wire Line
	14075 8025 14075 8125
Wire Wire Line
	14075 8475 14075 8725
Wire Wire Line
	2950 6850 3250 6850
Wire Wire Line
	2950 6750 3250 6750
Wire Wire Line
	12725 8475 12725 8725
Wire Wire Line
	6800 10050 6800 10100
Wire Wire Line
	2950 8350 3500 8350
Wire Wire Line
	2950 7800 3550 7800
Wire Wire Line
	1000 6150 1000 6350
Wire Wire Line
	1050 6650 1000 6650
Connection ~ 1000 6650
Wire Wire Line
	1050 6450 1000 6450
Connection ~ 1000 6450
Wire Wire Line
	1050 6350 1000 6350
Connection ~ 1000 6350
Wire Wire Line
	1050 8450 1000 8450
Wire Wire Line
	1000 8450 1000 8550
Wire Wire Line
	1050 8550 1000 8550
Connection ~ 1000 8550
Wire Wire Line
	1050 8650 1000 8650
Connection ~ 1000 8650
Wire Wire Line
	1700 9550 1700 9700
Connection ~ 12725 8125
Wire Wire Line
	2950 8050 3850 8050
Wire Wire Line
	1000 6950 1050 6950
Connection ~ 14075 8125
Wire Wire Line
	12050 1260 11450 1260
Wire Wire Line
	12050 1160 11450 1160
Wire Wire Line
	14525 8125 14675 8125
Wire Wire Line
	14675 8025 14675 8125
Connection ~ 14675 8125
Wire Wire Line
	14675 8725 14675 8475
Wire Wire Line
	10300 5775 10300 5825
Wire Wire Line
	8100 5875 8100 5725
Wire Wire Line
	8100 6975 8100 6775
Wire Wire Line
	4650 7450 4650 7350
Wire Wire Line
	4650 6950 4650 7050
Wire Wire Line
	3900 6350 4450 6350
Wire Wire Line
	4400 6950 4400 7050
Connection ~ 4400 6950
Wire Wire Line
	4400 7350 4400 7450
Connection ~ 4400 7450
Wire Wire Line
	2950 7050 3950 7050
Wire Wire Line
	3950 7050 3950 7450
Wire Wire Line
	3950 7450 4000 7450
Wire Wire Line
	4300 7450 4400 7450
Wire Wire Line
	2950 6950 4400 6950
Wire Wire Line
	9300 10000 8700 10000
Wire Wire Line
	8700 10100 9300 10100
Wire Wire Line
	9300 10200 8700 10200
Wire Wire Line
	9800 10000 10000 10000
Wire Wire Line
	10000 10000 10000 9800
Wire Wire Line
	9800 10200 10000 10200
Wire Wire Line
	10000 10200 10000 10350
Wire Wire Line
	9800 10100 10250 10100
Wire Wire Line
	3200 6650 2950 6650
Wire Wire Line
	3150 10050 3150 10100
Wire Wire Line
	3150 10450 3150 10500
Wire Wire Line
	3000 10100 3150 10100
Connection ~ 3150 10100
Wire Wire Line
	2800 10050 2800 10300
Wire Wire Line
	2800 9700 2800 9750
Connection ~ 2800 9700
Wire Wire Line
	2500 10050 2500 10300
Wire Wire Line
	2500 9550 2500 9700
Connection ~ 2500 9700
Wire Wire Line
	7900 9550 7400 9550
Wire Wire Line
	7900 9350 7400 9350
Wire Wire Line
	1700 10050 1700 10300
Connection ~ 1700 9700
Wire Wire Line
	3000 10150 3000 10100
Wire Wire Line
	3000 10450 3000 10500
Wire Wire Line
	3000 10500 3150 10500
Connection ~ 3150 10500
Wire Wire Line
	3150 9700 3150 9750
Wire Wire Line
	4350 7950 4600 7950
Wire Wire Line
	4350 8050 4400 8050
Wire Wire Line
	4400 8150 4400 8050
Connection ~ 4400 8050
Wire Wire Line
	4400 8550 4400 8450
Wire Wire Line
	5275 1500 4625 1500
Wire Wire Line
	4025 1500 4325 1500
Wire Wire Line
	4025 1300 4325 1300
Wire Wire Line
	5275 1300 4625 1300
Wire Wire Line
	3725 2100 3475 2100
Wire Wire Line
	3475 1300 3475 2100
Wire Wire Line
	4025 2100 4225 2100
Wire Wire Line
	3175 2100 3375 2100
Wire Wire Line
	3375 1500 3375 2100
Wire Wire Line
	2875 2100 2525 2100
Wire Wire Line
	2525 1400 2525 2100
Wire Wire Line
	2175 1400 2525 1400
Wire Wire Line
	4025 2350 4225 2350
Wire Wire Line
	4225 2350 4225 2100
Connection ~ 4225 2100
Wire Wire Line
	3475 2350 3725 2350
Connection ~ 3475 2100
Wire Wire Line
	3375 2350 3175 2350
Connection ~ 3375 2100
Wire Wire Line
	2525 2350 2875 2350
Connection ~ 2525 2100
Wire Wire Line
	6275 2525 6275 2475
Wire Wire Line
	1075 1400 1375 1400
Wire Wire Line
	1725 1650 1725 1400
Connection ~ 1725 1400
Wire Wire Line
	1725 2200 1725 1950
Wire Wire Line
	1375 1600 1375 1400
Connection ~ 1375 1400
Wire Wire Line
	1375 1900 1375 2000
Wire Wire Line
	1375 2500 1375 2300
Wire Wire Line
	5225 3600 4575 3600
Wire Wire Line
	3975 3600 4275 3600
Wire Wire Line
	3175 3600 3325 3600
Wire Wire Line
	3175 3400 3425 3400
Wire Wire Line
	3975 3400 4275 3400
Wire Wire Line
	5225 3400 4575 3400
Wire Wire Line
	3675 4200 3425 4200
Wire Wire Line
	3425 3400 3425 4200
Connection ~ 3425 3400
Wire Wire Line
	3975 4200 4175 4200
Wire Wire Line
	3125 4200 3325 4200
Wire Wire Line
	3325 3600 3325 4200
Connection ~ 3325 3600
Wire Wire Line
	2825 4200 2475 4200
Wire Wire Line
	2475 3500 2475 4200
Wire Wire Line
	2125 3500 2475 3500
Connection ~ 2475 3500
Wire Wire Line
	3975 4450 4175 4450
Wire Wire Line
	4175 4450 4175 4200
Connection ~ 4175 4200
Wire Wire Line
	3425 4450 3675 4450
Connection ~ 3425 4200
Wire Wire Line
	3325 4450 3125 4450
Connection ~ 3325 4200
Wire Wire Line
	2475 4450 2825 4450
Connection ~ 2475 4200
Wire Wire Line
	1025 3500 1325 3500
Wire Wire Line
	1675 3750 1675 3500
Connection ~ 1675 3500
Wire Wire Line
	1675 4300 1675 4050
Wire Wire Line
	1325 3700 1325 3500
Connection ~ 1325 3500
Wire Wire Line
	1325 4000 1325 4100
Wire Wire Line
	1325 4600 1325 4400
Wire Wire Line
	8675 2825 9225 2825
Wire Wire Line
	7525 1575 8075 1575
Wire Wire Line
	8075 1675 8075 1575
Connection ~ 8075 1575
Wire Wire Line
	8375 2425 8025 2425
Wire Wire Line
	8675 2425 9725 2425
Wire Wire Line
	8075 2025 8075 1975
Wire Wire Line
	7700 10450 7900 10450
Wire Wire Line
	2950 8250 3800 8250
Wire Wire Line
	7250 9200 7250 9450
Wire Wire Line
	8375 2825 8025 2825
Wire Wire Line
	9325 1575 9725 1575
Wire Wire Line
	9725 1575 9725 2425
Connection ~ 9725 2425
Wire Wire Line
	8025 2475 8025 2425
Connection ~ 8025 2425
Wire Wire Line
	8025 2775 8025 2825
Connection ~ 8025 2825
Wire Wire Line
	8675 1675 8675 1575
Connection ~ 8675 1575
Wire Wire Line
	8675 2025 8675 1975
Wire Wire Line
	9225 3025 9225 2825
Connection ~ 9225 2825
Wire Wire Line
	9225 3425 9225 3325
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR041
U 1 1 59FD5A8D
P 14000 6525
F 0 "#PWR041" H 14000 6275 50  0001 C CNN
F 1 "GND" H 14000 6375 50  0000 C CNN
F 2 "" H 14000 6525 60  0000 C CNN
F 3 "" H 14000 6525 60  0000 C CNN
	1    14000 6525
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:LED-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue D2
U 1 1 59FD6691
P 14000 6175
F 0 "D2" V 14038 6058 50  0000 R CNN
F 1 "LED" V 13947 6058 50  0000 R CNN
F 2 "LEDs:LED_0805_HandSoldering" H 14000 6175 50  0001 C CNN
F 3 "" H 14000 6175 50  0001 C CNN
	1    14000 6175
	0    -1   -1   0   
$EndComp
Wire Wire Line
	14000 6325 14000 6525
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R26
U 1 1 59FD6975
P 14000 5725
F 0 "R26" V 14208 5725 50  0000 C CNN
F 1 "1k" V 14116 5725 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 14000 5725 60  0001 C CNN
F 3 "" H 14000 5725 60  0001 C CNN
	1    14000 5725
	1    0    0    -1  
$EndComp
Wire Wire Line
	14000 5575 14000 5275
Wire Wire Line
	14000 5275 14450 5275
Text Label 14450 5275 2    60   ~ 0
LED1
Wire Wire Line
	14000 5875 14000 6025
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR042
U 1 1 59FD718E
P 14750 6525
F 0 "#PWR042" H 14750 6275 50  0001 C CNN
F 1 "GND" H 14750 6375 50  0000 C CNN
F 2 "" H 14750 6525 60  0000 C CNN
F 3 "" H 14750 6525 60  0000 C CNN
	1    14750 6525
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:LED-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue D3
U 1 1 59FD7194
P 14750 6175
F 0 "D3" V 14788 6058 50  0000 R CNN
F 1 "LED" V 14697 6058 50  0000 R CNN
F 2 "LEDs:LED_0805_HandSoldering" H 14750 6175 50  0001 C CNN
F 3 "" H 14750 6175 50  0001 C CNN
	1    14750 6175
	0    -1   -1   0   
$EndComp
Wire Wire Line
	14750 6325 14750 6525
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R31
U 1 1 59FD719C
P 14750 5725
F 0 "R31" V 14958 5725 50  0000 C CNN
F 1 "1k" V 14866 5725 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 14750 5725 60  0001 C CNN
F 3 "" H 14750 5725 60  0001 C CNN
	1    14750 5725
	1    0    0    -1  
$EndComp
Wire Wire Line
	14750 5575 14750 5275
Wire Wire Line
	14750 5275 15200 5275
Wire Wire Line
	14750 5875 14750 6025
Text Label 15200 5275 2    60   ~ 0
LED2
$Comp
L atmegax8_with_F-3188-rescue:Conn_01x04-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue P3
U 1 1 59FDCC12
P 15100 4000
F 0 "P3" V 15050 4000 50  0000 C CNN
F 1 "CONN_4" V 15150 4000 50  0000 C CNN
F 2 "Pin_Headers.pretty:Pin_Header_Straight_1x04" H 15100 4000 60  0001 C CNN
F 3 "" H 15100 4000 60  0001 C CNN
	1    15100 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	14900 4100 14250 4100
Wire Wire Line
	14900 4000 14250 4000
Wire Wire Line
	14900 3900 14250 3900
Text Label 14250 4100 0    60   ~ 0
INL
Text Label 14250 4000 0    60   ~ 0
INR
Text Label 14250 3900 0    60   ~ 0
IN_DETECT
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR045
U 1 1 59FDDD7C
P 14600 4300
F 0 "#PWR045" H 14600 4050 50  0001 C CNN
F 1 "GND" H 14600 4150 50  0000 C CNN
F 2 "" H 14600 4300 60  0000 C CNN
F 3 "" H 14600 4300 60  0000 C CNN
	1    14600 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	14600 4300 14600 4200
Wire Wire Line
	14600 4200 14900 4200
Wire Wire Line
	1000 6650 1000 6950
Wire Wire Line
	1000 6450 1000 6650
Wire Wire Line
	1000 6350 1000 6450
Wire Wire Line
	1000 8550 1000 8650
Wire Wire Line
	1000 8650 1000 8800
Wire Wire Line
	12725 8125 12725 8175
Wire Wire Line
	14075 8125 14075 8175
Wire Wire Line
	14075 8125 14225 8125
Wire Wire Line
	14675 8125 14675 8175
Wire Wire Line
	4400 6950 4650 6950
Wire Wire Line
	4400 7450 4650 7450
Wire Wire Line
	3150 10100 3150 10150
Wire Wire Line
	3150 10100 3450 10100
Wire Wire Line
	2800 9700 3150 9700
Wire Wire Line
	2500 9700 2500 9750
Wire Wire Line
	2500 9700 2800 9700
Wire Wire Line
	1700 9700 1700 9750
Wire Wire Line
	3150 10500 3150 10550
Wire Wire Line
	4400 8050 4600 8050
Wire Wire Line
	3475 1300 3725 1300
Wire Wire Line
	3375 1500 3725 1500
Wire Wire Line
	4225 2100 5275 2100
Wire Wire Line
	3475 2100 3475 2350
Wire Wire Line
	3375 2100 3375 2350
Wire Wire Line
	2525 2100 2525 2350
Wire Wire Line
	1725 1400 1875 1400
Wire Wire Line
	1375 1400 1725 1400
Wire Wire Line
	3425 3400 3675 3400
Wire Wire Line
	3325 3600 3675 3600
Wire Wire Line
	2475 3500 2575 3500
Wire Wire Line
	4175 4200 5225 4200
Wire Wire Line
	3425 4200 3425 4450
Wire Wire Line
	3325 4200 3325 4450
Wire Wire Line
	2475 4200 2475 4450
Wire Wire Line
	1675 3500 1825 3500
Wire Wire Line
	1325 3500 1675 3500
Wire Wire Line
	8075 1575 8675 1575
Wire Wire Line
	9725 2425 10275 2425
Wire Wire Line
	8025 2425 7525 2425
Wire Wire Line
	8025 2825 7525 2825
Wire Wire Line
	8675 1575 9025 1575
Wire Wire Line
	9225 2825 10275 2825
Wire Wire Line
	5350 7600 5350 7200
Wire Wire Line
	1700 9700 1850 9700
Wire Wire Line
	2350 9700 2500 9700
NoConn ~ 1050 7700
NoConn ~ 1050 7800
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R4
U 1 1 568C17BF
P 3750 6350
F 0 "R4" V 3830 6350 50  0000 C CNN
F 1 "3k3" V 3750 6350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3750 6350 60  0001 C CNN
F 3 "" H 3750 6350 60  0001 C CNN
	1    3750 6350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2950 6350 3600 6350
Wire Wire Line
	800  6400 800  6350
Wire Wire Line
	800  6350 1000 6350
Wire Wire Line
	1000 8550 800  8550
Wire Wire Line
	800  8550 800  6700
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C1
U 1 1 5DA136A4
P 800 6550
F 0 "C1" H 915 6596 50  0000 L CNN
F 1 "100n" H 915 6504 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 800 6550 60  0001 C CNN
F 3 "" H 800 6550 60  0001 C CNN
	1    800  6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 9450 7900 9450
Wire Wire Line
	7700 10150 7900 10150
Wire Wire Line
	7400 9950 7900 9950
Wire Wire Line
	7400 9850 7900 9850
Wire Wire Line
	7050 9750 7050 9700
Wire Wire Line
	7250 10350 7900 10350
Wire Wire Line
	7400 10150 6900 10150
Wire Wire Line
	6900 10250 7900 10250
Wire Wire Line
	7400 10450 6900 10450
Wire Wire Line
	7050 9750 7900 9750
Text Label 7400 9950 0    60   ~ 0
MIC_IN1
Text Label 7400 9850 0    60   ~ 0
MIC_IN2
Text Label 7250 10350 0    60   ~ 0
CDC_ENA_12V
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR011
U 1 1 564E3127
P 6800 10100
F 0 "#PWR011" H 6800 9850 50  0001 C CNN
F 1 "GND" H 6800 9950 50  0000 C CNN
F 2 "" H 6800 10100 60  0000 C CNN
F 3 "" H 6800 10100 60  0000 C CNN
	1    6800 10100
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:+12V-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR05
U 1 1 543549B8
P 7050 9700
F 0 "#PWR05" H 7050 9650 20  0001 C CNN
F 1 "+12V" H 7050 9800 30  0000 C CNN
F 2 "" H 7050 9700 60  0001 C CNN
F 3 "" H 7050 9700 60  0001 C CNN
	1    7050 9700
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R1
U 1 1 530C77F4
P 7550 10150
F 0 "R1" V 7600 10000 50  0000 C CNN
F 1 "3k3" V 7550 10150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 7550 10150 60  0001 C CNN
F 3 "" H 7550 10150 60  0001 C CNN
	1    7550 10150
	0    1    1    0   
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R2
U 1 1 530C78F0
P 7550 10450
F 0 "R2" V 7500 10300 50  0000 C CNN
F 1 "3k3" V 7550 10450 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 7550 10450 60  0001 C CNN
F 3 "" H 7550 10450 60  0001 C CNN
	1    7550 10450
	0    1    1    0   
$EndComp
Text Label 6900 10250 0    60   ~ 0
DataOut
Text Label 6900 10450 0    60   ~ 0
DataIn
Text Label 6900 10150 0    60   ~ 0
Clock
$Comp
L atmegax8_with_F-3188-rescue:CONN_12-gencon-atmegax8_with_bluetooth-rescue P1
U 1 1 530C9435
P 8300 9900
F 0 "P1" V 8250 9900 50  0000 C CNN
F 1 "CONN_10" V 8350 9900 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x12_Pitch2.54mm" H 8300 9900 60  0001 C CNN
F 3 "" H 8300 9900 60  0001 C CNN
	1    8300 9900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male P4
U 1 1 5DA1876B
P 3600 8700
F 0 "P4" V 3550 8700 50  0000 C CNN
F 1 "SERIAL" V 3650 8700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 3600 8700 60  0001 C CNN
F 3 "" H 3600 8700 60  0001 C CNN
	1    3600 8700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 8700 3850 8700
Wire Wire Line
	3850 8700 3850 8050
Connection ~ 3850 8050
Wire Wire Line
	3850 8050 4050 8050
Wire Wire Line
	5350 7200 4850 7200
$Comp
L Device:Crystal_GND2 Y1
U 1 1 5A9682FE
P 4650 7200
F 0 "Y1" V 4850 7025 50  0000 L CNN
F 1 "16MHz" V 5025 7025 50  0000 L CNN
F 2 "modules:MURATA_CSTCE_V53" H 4650 7200 50  0001 C CNN
F 3 "~" H 4650 7200 50  0001 C CNN
	1    4650 7200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2950 7950 3925 7950
$Comp
L MAX809_810:MAX809 U5
U 1 1 5DA5377F
P 10350 6175
F 0 "U5" H 10122 6121 50  0000 R CNN
F 1 "MAX809" H 10122 6030 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD.pretty:SOT-23" H 10350 6175 50  0001 C CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX803-MAX810Z.pdf" H 10350 6175 50  0001 C CNN
	1    10350 6175
	1    0    0    -1  
$EndComp
$Comp
L MAX809_810:MAX809 U4
U 1 1 5DA334FD
P 8150 6225
F 0 "U4" H 7922 6171 50  0000 R CNN
F 1 "MAX809" H 7922 6080 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD.pretty:SOT-23" H 8150 6225 50  0001 C CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX803-MAX810Z.pdf" H 8150 6225 50  0001 C CNN
	1    8150 6225
	1    0    0    -1  
$EndComp
Wire Wire Line
	9425 6325 8925 6325
Wire Wire Line
	8550 6325 8625 6325
Wire Wire Line
	10750 6275 10875 6275
Wire Wire Line
	10300 6725 10300 6850
Wire Wire Line
	11575 6275 11175 6275
Wire Wire Line
	3800 8800 3925 8800
Wire Wire Line
	3925 8800 3925 7950
Connection ~ 3925 7950
Wire Wire Line
	3925 7950 4050 7950
Wire Wire Line
	12725 8125 11925 8125
Text Label 11925 8125 0    60   ~ 0
CDC_ENA_12V
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R5
U 1 1 568C1D2F
P 4150 7450
F 0 "R5" V 4230 7450 50  0000 C CNN
F 1 "20" V 4150 7450 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 4150 7450 60  0001 C CNN
F 3 "" H 4150 7450 60  0001 C CNN
	1    4150 7450
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x03_Male P5
U 1 1 5DA6A611
P 13400 7775
F 0 "P5" V 13350 7775 50  0000 C CNN
F 1 "CONN_4" V 13450 7775 50  0000 C CNN
F 2 "Pin_Headers.pretty:Pin_Header_Straight_1x03" H 13400 7775 60  0001 C CNN
F 3 "" H 13400 7775 60  0001 C CNN
	1    13400 7775
	0    1    1    0   
$EndComp
Wire Wire Line
	13300 7975 13300 8125
Wire Wire Line
	12725 8125 13300 8125
Wire Wire Line
	13400 7975 13400 8725
Wire Wire Line
	13500 7975 13500 8125
Wire Wire Line
	13500 8125 13700 8125
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R24
U 1 1 56916F25
P 3875 2100
F 0 "R24" V 3975 2050 50  0000 R CNN
F 1 "47k" V 3975 2250 50  0000 R CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3875 2100 60  0001 C CNN
F 3 "" H 3875 2100 60  0001 C CNN
	1    3875 2100
	0    1    -1   0   
$EndComp
Wire Wire Line
	6800 10050 7900 10050
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR0101
U 1 1 5DA87F2D
P 1700 10300
F 0 "#PWR0101" H 1700 10050 50  0001 C CNN
F 1 "GNDA" H 1708 10126 50  0000 C CNN
F 2 "" H 1700 10300 50  0000 C CNN
F 3 "" H 1700 10300 50  0000 C CNN
	1    1700 10300
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GNDA-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR0103
U 1 1 5DA88745
P 7225 9825
F 0 "#PWR0103" H 7225 9575 50  0001 C CNN
F 1 "GNDA" H 7233 9651 50  0000 C CNN
F 2 "" H 7225 9825 50  0000 C CNN
F 3 "" H 7225 9825 50  0000 C CNN
	1    7225 9825
	1    0    0    -1  
$EndComp
Wire Wire Line
	7225 9825 7225 9650
Wire Wire Line
	7225 9650 7900 9650
$Comp
L Device:CP C10
U 1 1 568E86D3
P 1700 9900
F 0 "C10" H 1815 9946 50  0000 L CNN
F 1 "100u/16V" H 1815 9854 50  0000 L CNN
F 2 "Capacitors_Tantalum_SMD:CP_Tantalum_Case-D_EIA-7343-31_Hand" H 1700 9900 60  0001 C CNN
F 3 "" H 1700 9900 60  0001 C CNN
	1    1700 9900
	1    0    0    -1  
$EndComp
Wire Wire Line
	13700 8125 13700 8175
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR022
U 1 1 5DA5220E
P 13700 8725
F 0 "#PWR022" H 13700 8475 50  0001 C CNN
F 1 "GND" H 13700 8575 50  0000 C CNN
F 2 "" H 13700 8725 60  0000 C CNN
F 3 "" H 13700 8725 60  0000 C CNN
	1    13700 8725
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C24
U 1 1 5DA5221C
P 13700 8325
F 0 "C24" H 13815 8371 50  0000 L CNN
F 1 "100n" H 13815 8279 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 13700 8325 60  0001 C CNN
F 3 "" H 13700 8325 60  0001 C CNN
	1    13700 8325
	1    0    0    -1  
$EndComp
Wire Wire Line
	13700 8475 13700 8725
Connection ~ 13700 8125
Wire Wire Line
	13700 8125 14075 8125
Connection ~ 2525 1400
Wire Wire Line
	2525 1400 2625 1400
Connection ~ 3375 1500
Wire Wire Line
	3225 1500 3375 1500
Connection ~ 3475 1300
Wire Wire Line
	3225 1300 3475 1300
$Comp
L Amplifier_Operational:TL072 U?
U 1 1 5DAA2104
P 2925 1400
F 0 "U?" H 2925 1767 50  0000 C CNN
F 1 "TL072" H 2925 1676 50  0000 C CNN
F 2 "" H 2925 1400 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tl071.pdf" H 2925 1400 50  0001 C CNN
	1    2925 1400
	-1   0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:TL072 U?
U 2 1 5DAD02FB
P 2875 3500
F 0 "U?" H 2875 3867 50  0000 C CNN
F 1 "TL072" H 2875 3776 50  0000 C CNN
F 2 "" H 2875 3500 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tl071.pdf" H 2875 3500 50  0001 C CNN
	2    2875 3500
	-1   0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:TL072 U?
U 3 1 5DAD918D
P 6375 2175
F 0 "U?" H 6333 2221 50  0000 L CNN
F 1 "TL072" H 6333 2130 50  0000 L CNN
F 2 "" H 6375 2175 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tl071.pdf" H 6375 2175 50  0001 C CNN
	3    6375 2175
	1    0    0    -1  
$EndComp
Wire Wire Line
	6275 1825 6275 1875
$Comp
L F-3188:F-3188_CSR8645 U?
U 1 1 5DAE92D7
P 13250 2710
F 0 "U?" H 13050 4655 60  0000 C CNN
F 1 "F-3188_CSR8645" H 13050 4548 60  0000 C CNN
F 2 "" H 13250 1010 60  0000 C CNN
F 3 "datasheet/F-3188_CSR8645_bluetooth/CSR8645_datasheet.pdf" H 13050 4441 60  0000 C CNN
	1    13250 2710
	1    0    0    -1  
$EndComp
Wire Wire Line
	14050 1460 14700 1460
Wire Wire Line
	14050 1560 14700 1560
Text Label 14670 1960 2    60   ~ 0
MICN
Text Label 14690 1260 2    60   ~ 0
BTOUTL
Text Label 14700 1560 2    60   ~ 0
BTOUTR
Text Label 14690 1360 2    60   ~ 0
BTOUTLN
Text Label 14700 1460 2    60   ~ 0
BTOUTRN
$Comp
L atmegax8_with_F-3188-rescue:+BATT-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR015
U 1 1 568BDDE9
P 11330 2300
F 0 "#PWR015" H 11330 2150 50  0001 C CNN
F 1 "+BATT" H 11348 2474 50  0000 C CNN
F 2 "" H 11330 2300 50  0000 C CNN
F 3 "" H 11330 2300 50  0000 C CNN
	1    11330 2300
	1    0    0    -1  
$EndComp
Text Label 14670 2060 2    60   ~ 0
MIC_BIAS
Wire Wire Line
	12050 2060 11670 2060
Wire Wire Line
	12050 2160 11670 2160
$Comp
L Connector_Generic:Conn_01x01 J5
U 1 1 5DA5A6D8
P 11475 1860
F 0 "J5" H 11275 1830 50  0000 C CNN
F 1 "MOSI" H 11625 1860 50  0000 C CNN
F 2 "modules:1pin_smd" H 11475 1860 50  0001 C CNN
F 3 "~" H 11475 1860 50  0001 C CNN
	1    11475 1860
	-1   0    0    1   
$EndComp
Wire Wire Line
	11675 1660 12050 1660
Wire Wire Line
	12050 1760 11675 1760
Wire Wire Line
	11675 1860 12050 1860
$Comp
L Connector_Generic:Conn_01x01 J4
U 1 1 5DA5A3E2
P 11475 1760
F 0 "J4" H 11275 1730 50  0000 C CNN
F 1 "CLK" H 11625 1760 50  0000 C CNN
F 2 "modules:1pin_smd" H 11475 1760 50  0001 C CNN
F 3 "~" H 11475 1760 50  0001 C CNN
	1    11475 1760
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J3
U 1 1 5DA59E64
P 11475 1660
F 0 "J3" H 11275 1620 50  0000 C CNN
F 1 "CSB" H 11625 1660 50  0000 C CNN
F 2 "modules:1pin_smd" H 11475 1660 50  0001 C CNN
F 3 "~" H 11475 1660 50  0001 C CNN
	1    11475 1660
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 5DA589B4
P 11475 1560
F 0 "J2" H 11275 1520 50  0000 C CNN
F 1 "MISO" H 11625 1560 50  0000 C CNN
F 2 "modules:1pin_smd" H 11475 1560 50  0001 C CNN
F 3 "~" H 11475 1560 50  0001 C CNN
	1    11475 1560
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J1
U 1 1 5DA5B5F7
P 11475 1960
F 0 "J1" H 11275 1930 50  0000 C CNN
F 1 "SPI_PCM" H 11695 1965 50  0000 C CNN
F 2 "modules:1pin_smd" H 11475 1960 50  0001 C CNN
F 3 "~" H 11475 1960 50  0001 C CNN
	1    11475 1960
	-1   0    0    1   
$EndComp
Wire Wire Line
	12050 1560 11675 1560
Wire Wire Line
	12050 1960 11675 1960
Wire Wire Line
	12050 2360 11510 2360
Wire Wire Line
	12050 2760 11970 2760
Wire Wire Line
	11970 2760 11970 3210
Wire Wire Line
	14050 1160 14260 1160
Wire Wire Line
	14260 1160 14260 2940
Wire Wire Line
	14050 2060 14670 2060
Wire Wire Line
	14050 1960 14670 1960
Wire Wire Line
	14050 1860 14670 1860
Text Label 14670 1860 2    60   ~ 0
MICP
Wire Wire Line
	14050 1260 14690 1260
Wire Wire Line
	14050 1360 14690 1360
Wire Wire Line
	12050 2460 11330 2460
Wire Wire Line
	11330 2460 11330 2300
Text Label 4600 7950 2    60   ~ 0
RX
Text Label 4600 8050 2    60   ~ 0
TX
Text Label 11670 2060 0    60   ~ 0
RX
Text Label 11670 2160 0    60   ~ 0
TX
Wire Wire Line
	11295 3140 11295 3390
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C?
U 1 1 5DAA8915
P 11295 2990
F 0 "C?" H 11410 3036 50  0000 L CNN
F 1 "100n" H 11410 2944 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 11295 2990 60  0001 C CNN
F 3 "" H 11295 2990 60  0001 C CNN
	1    11295 2990
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR?
U 1 1 5DAA8926
P 11295 3390
F 0 "#PWR?" H 11295 3140 50  0001 C CNN
F 1 "GND" H 11295 3240 50  0000 C CNN
F 2 "" H 11295 3390 60  0000 C CNN
F 3 "" H 11295 3390 60  0000 C CNN
	1    11295 3390
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:C-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue C?
U 1 1 5DAAD336
P 10940 2990
F 0 "C?" H 11055 3036 50  0000 L CNN
F 1 "100n" H 11055 2944 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 10940 2990 60  0001 C CNN
F 3 "" H 10940 2990 60  0001 C CNN
	1    10940 2990
	1    0    0    -1  
$EndComp
$Comp
L atmegax8_with_F-3188-rescue:GND-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue #PWR?
U 1 1 5DAAD347
P 10940 3390
F 0 "#PWR?" H 10940 3140 50  0001 C CNN
F 1 "GND" H 10940 3240 50  0000 C CNN
F 2 "" H 10940 3390 60  0000 C CNN
F 3 "" H 10940 3390 60  0000 C CNN
	1    10940 3390
	1    0    0    -1  
$EndComp
Wire Wire Line
	10940 3140 10940 3390
Wire Wire Line
	10940 2840 10940 2260
Wire Wire Line
	10940 2260 12050 2260
Wire Wire Line
	11295 2660 12050 2660
Wire Wire Line
	11295 2660 11295 2840
$Comp
L atmegax8_with_F-3188-rescue:R-RESCUE-atmegax8_with_bluetooth-atmegax8_with_bluetooth-rescue R?
U 1 1 5DAAE826
P 11715 2560
F 0 "R?" V 11923 2560 50  0000 C CNN
F 1 "10k" V 11831 2560 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 11715 2560 60  0001 C CNN
F 3 "" H 11715 2560 60  0001 C CNN
	1    11715 2560
	0    -1   1    0   
$EndComp
Wire Wire Line
	11865 2560 12050 2560
Wire Wire Line
	11565 2560 11330 2560
Wire Wire Line
	11330 2560 11330 2460
Connection ~ 11330 2460
Wire Wire Line
	13550 3010 13550 3365
Wire Wire Line
	13450 3010 13450 3365
Wire Wire Line
	13350 3010 13350 3365
Text Label 13550 3365 1    60   ~ 0
MUTE
Text Label 13450 3365 1    60   ~ 0
USB_P
Text Label 13350 3365 1    60   ~ 0
USB_N
$Comp
L tinkerforge:AGND #PWR?
U 1 1 5DAB645D
P 14260 2940
F 0 "#PWR?" H 14260 2690 50  0001 C CNN
F 1 "AGND" H 14265 2767 50  0000 C CNN
F 2 "" H 14260 2940 50  0000 C CNN
F 3 "" H 14260 2940 50  0000 C CNN
	1    14260 2940
	1    0    0    -1  
$EndComp
Wire Wire Line
	14050 2660 14670 2660
Wire Wire Line
	14050 2460 14670 2460
Wire Wire Line
	14050 2160 14670 2160
Wire Wire Line
	14050 2260 14675 2260
Wire Wire Line
	14050 2360 14675 2360
Text Label 14670 2160 2    60   ~ 0
VOLUME_UP
Text Label 14675 2260 2    60   ~ 0
VOLUME_DOWN
Text Label 14675 2360 2    60   ~ 0
NEXT
Text Label 14670 2460 2    60   ~ 0
PREV
Text Label 14670 2660 2    60   ~ 0
PLAY
$EndSCHEMATC
