/* DC control Minicircuit PMA5-123-3W+ 
 *  ------------------------------------------
 *         3W 10GHz GaAs MMIC
 *         
 * Disclaimer
 * ------------------------------------------
 *  "This software is provided "as is" and without
 *  any warranties, express or implied, including but
 *  not limited to the implied warranties of merchantability
 *  and fitness for a particular purpose." 
 *
 *  
 * Rens PA3AXA / Maarten PA0MHE
 * 
 * Software V1.0 - Initial release 01-06-2025 for
 * Hardware V1.0 - Initial release 01-06-2025
 * 
 * Uses megatinyCore in boardmanager
 * Chip: ATtiny412
 * Clock: 8 MHz internal
 * Safe EEprom setting to retain set Bias
 * Programmer: serialUPDI 230400 baud
 * 
 * 
 * Current sensor gain R9 is set for 51k ohm
 * this nearly 1A/V.
 * 
 *    1.02V = (1) (0,1) (200 μA/V) (51k)
 
 
 ----------------------------------------------------------------
    
     ###  ### #   #  ###
    #   #  #  ##  # #
    ####   #  # # #  ###
    #      #  #  ##     #
    #     ### #   #  ###

 ----------------------------------------------------------------
*/

#include <EEPROM.h>
 
#define pttPin     PIN_PA3 // package pin 7 on AtTiny 412
#define vddEnable  PIN_PA2 // package pin 5
#define IdADC      PIN_PA7 // package pin 3
#define switchPin  PIN_PA1 // package pin 4
#define VggDAC     PIN_PA6 // DAC It must be package pin 2

#define VggMin  90         // DAC value for lowest Vgate useally -1.5V
#define VggMax 255         // DAC value for highest Vgate -0.73V

/* Vgg 0 = -1,9V, Vgg 90 = -1.5 Vgg 255 = -0,73,  Vgg 128 = -1.37V */ 

#define Idq    163         //  400mA ADC  0,4/2,5 * 1023 = 163
#define IdMax  511         // 1250mA ADC 1,25/2,5 * 1023 = 511
#define IdMin  0           //    0mA ADC  0,2/2.5 * 1023 = 0

/* The IdMin feature is not fully tested current default 0 mA */


#define VggEeprom 1
#define IdEeprom 2

#define EepromProgrammed 0x55

bool TX = false;
bool error = false;
bool LedState = false;

int VggIdq = 150;         // DAC value before calibration set to a safe value


void setup(){
  
  digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF 
  pinMode(vddEnable, OUTPUT);

  DACReference(INTERNAL4V3);     // DAC reference, max 8bit level is 4,3V 
  analogReference(INTERNAL2V5);  // 10bit ADC, Input > Ref is clipped to 0x3FF = 1023

  pinMode(pttPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);

  analogWrite(VggDAC, VggMin);   // Write DAC cut Vgg Drain Cut-off level

  delay(500);
 
 /*
  Press Switch during Power-Up to Start Idq callibration
  Final VggIdq value is set to meet the Idq of 400mA
 */
 
  if (!digitalRead(switchPin)){
  SetIdq();
  }

  
 /*Check if we need to run calibration, (needed after every software update) */
  
  if (EEPROM.read(IdEeprom) != EepromProgrammed) {
        EEPROM.write(VggEeprom, VggMin);
        
        // Switch to output mode for error flash
        pinMode(switchPin, OUTPUT);
        
        for (int c=0; c < 6; c++) {
        delay(100);
        LedState = !LedState;
        digitalWrite(switchPin, LedState); // Flash 5x to sign EEProm is not programmmed   
        }
        pinMode(switchPin, INPUT_PULLUP); 
    }
  
  /* Load VggIdq setting for the idle current */
  
  VggIdq = EEPROM.read(VggEeprom);     

}



void loop(){
    
    if (TX) { // Check if RX mode requested when in TX
      if (!digitalRead(pttPin)) { // RX mode requested when pptPin == Low
      SwitchOffPA();
      TX = false;
      }
    }
    
    if (!TX) { // Check if TX mode requested when in RX
       if (digitalRead(pttPin)) { // TX mode requested when pptPin == High
        SwitchOnPA();
        TX = true;
      }
    }

   // System Error detect check if currents are between normal operation parms
   
   if (TX) {
      if ((analogRead(IdADC) > IdMax) || (analogRead(IdADC) < IdMin )) {
      SwitchOffPA();
      TX = false;
      SysError(1000); // Flash LED every second
      }
   }

}// End Loop


void SysError(int t){

  // Flash LED when we are in error needs powerdown to reset
  pinMode(switchPin, OUTPUT);  
  
  while (true){
  delay(t);
  LedState = !LedState;
  digitalWrite(switchPin, LedState);
 }
} 

void SwitchOnPA(){
  
  analogWrite(VggDAC, VggMin);
  delay(100); 
  digitalWrite(vddEnable, HIGH);   // 7V VDD of PMA5-123 is ON 
  delay(100); 
  analogWrite(VggDAC, VggIdq);
  delay(100);
  if ((analogRead(IdADC) > IdMax) || (analogRead(IdADC) < IdMin )) {
  SwitchOffPA();
  SysError(100);
  }

} // END SwitchOnPA


void SwitchOffPA() {
  
  analogWrite(VggDAC, VggMin);  // First Vgg to MIN  to cut drain current
  delay(100); 
  digitalWrite(vddEnable, LOW);    // 7V VDD of PMA5-123 is OFF
  delay(100); 
  
} // END SwitchOffPA


/*
 * Button start current setting only during Power-Up 
 */

void SetIdq(){

    analogWrite(VggDAC, VggMin);
    delay(100);
    digitalWrite(vddEnable, HIGH);   // 7V VDD of PMA5-123 is ON 
    delay(100);
    
    for (int i=VggMin; i < VggMax; i++)
    {
    analogWrite(VggDAC, i);
    delay(100);
      if (analogRead(IdADC) > Idq) // Idq setting reached, stop loop
        {
          delay(100);
          analogWrite(VggDAC, VggMin);
          delay(100);
          digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF
          delay(100);
          
          // Program Vgg in EEprom
          EEPROM.write(VggEeprom, i);
          EEPROM.write(IdEeprom, EepromProgrammed);
          break;
        }
    }
     
    // Turn off Calibrate Idq mode when not writing to EEprom.
    // This happens when Idq setting not satisfied.
    // Leave at current settings
      
    delay(100);
    analogWrite(VggDAC, VggMin);
    delay(100);
    digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF
    delay(100);
}
