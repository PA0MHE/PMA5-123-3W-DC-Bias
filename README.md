# PMA5-123-3W--DC-Bias

 DC bias PCB for PMA5-123-3W MMIC 3W 10GHz
 - ATtiny412 SW controlled
 - LDO for VDD, typical 7V
 - Switch to switch off or on Vdd
 - Idd Current measurment
 - DAC controlled (negative) Vgg powersupply
 - Inputs:
	+8 .. 12V DC
	PTT
 - Outputs
	Vdd
	Vgg (-1.5 .. -0.75V
	
 
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

 By pressing SW1 during Power-Up, Idq is set to ~400mA and stored in EEPROM
 	
 HEX file is included.


# PCB

Gerber files are included, PCB's can be ordered at JCL PCB 

# Software

A V1.0 of the Control software is included.


# Disclaimer  

"This software and hardware design is provided "as is" and without  
any warranties, express or implied, including but not limited to the   
implied warranties of merchantability and fitness for a particular purpose." 


73's

Maarten PA0MHE, Rens PA3AXA
