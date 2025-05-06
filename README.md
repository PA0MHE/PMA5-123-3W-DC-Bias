# PMA5-123-3W--DC-Bias

 DC bias PCB for PMA5-123-3W MMIC 3W 10GHz
 - ATtiny412 SW controlled
 - LDO for VDD, typical 7V
 - Switch to switch off or on Vdd
 - Idd Current measurment
 - DAC controlled (negative) Vgg synthersizer
 - Inputs:
	+8 .. 12V DC
	PTT
 - Outputs
	Vdd
	Vgg (-1.% .. -0.75V
	
 
# Key components:

	Attiny412
	MIC29302
	R05C1TF05S-R
	INA168

 # SW
	
	Uses megatinyCore in boardmanager
	Chip: ATtiny412
	clock: 8 MHz internal
	Programmer: serialUPDI 230400 baud

 By pressing SW1 once in RX condition, the Idq is set to ~450mA and stored in EEPROM


# PCB

Gerber files are included, PCB's can be ordered at JCL PCB 


73's Maarten PA0MHE
