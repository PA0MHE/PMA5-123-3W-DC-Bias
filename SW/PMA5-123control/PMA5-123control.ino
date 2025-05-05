/* DC control Minicircuit PMA5-123-3W+ 3W 10GHz GaAs MMIC
 *  
 * Maarten PA0MHE
 * 
 * 31-03-2025
 * 
 * Uses megatinyCore in boardmanager
 * Chip: ATtiny412
 * clock: 8 MHz internal
 * Programmer: serialUPDI 230400 baud
 * 
 */

#include <EEPROM.h>

#define pttPin     PIN_PA3 // package pin 7 on AtTiny 412
#define vddEnable  PIN_PA2 // package pin 5
#define currADC    PIN_PA7 // package pin 3
#define switchPin  PIN_PA1 // package pin 4
#define VgateDAC   PIN_PA6 // DAC It must be package pin 2

#define VggMin 90    // DAC value for lowest Vgate useally -1.5V
#define VggMax 255   // DAC value for highest Vgate -0.4V
#define Idq 160      // 51K 10x, 500 =~ 1.2A 400=~1A 100=~ 250mA
#define IdMax  800   // ~2A
#define delaySteps 5

#define VggEeprom 1
#define IdEeprom 2

#define EepromProgrammed 0x55

bool TX = false;
bool error = false;

int stabVgate =VggMin;
byte vggTarget = VggMin;


void setup()
{
  digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF 
  pinMode(vddEnable, OUTPUT);

  DACReference(INTERNAL4V3);
  analogReference(INTERNAL2V5);  

  pinMode(pttPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);
  
  analogWrite(VgateDAC, VggMin);

  if (EEPROM.read(IdEeprom) != EepromProgrammed) EEPROM.write(VggEeprom, VggMin);

  delay(500);
  analogRead(currADC);
}

  
void loop() {
// NormalControl();

//FixedVggControl(135); //  ~ 440mA in my case
//FixedVggControl(vggTarget); 

FixedVggControl(EEPROM.read(VggEeprom));
VggProgram();
 
 } // End loop


void NormalControl(void) {
  if (analogRead(currADC) > IdMax) {
   error = true;
   SwitchOffPA();
  }
  if (!error) {
     if (digitalRead(switchPin)) { // RX mode
        if (TX) SwitchOffPA();
      }
      else {    // TX mode
        if (!TX) {
          if (SwitchOnPA()) TX=true; 
          }
       }  
    }
}


bool SwitchOnPA(void) {
  analogWrite(VgateDAC, VggMin);
  delay(delaySteps); 
  digitalWrite(vddEnable, HIGH);   // 7V VDD of PMA5-123 is ON 
  for (int i=VggMin; i < VggMax; i++) {
    if (i == VggMax -1) {
      error = true;
      return false;
    }
    analogWrite(VgateDAC, i);
    delay(delaySteps);   
    if (analogRead(currADC) > Idq) {
      return true;
    }
  }
}


void SwitchOffPA(void) {
  digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF 
  analogWrite(VgateDAC, VggMin);
  TX = false;
}


void FixedVggControl(byte Vgg) {
  analogWrite(VgateDAC, Vgg);
  if (analogRead(currADC) > IdMax) {
   error = true;
   digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF 
  }
  if (!error) {
     if (!digitalRead(pttPin)) { // RX mode
     //if (digitalRead(switchPin)) { // RX mode      
        if (TX) {
          digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF
          TX=false; 
        }
      }
      else {    // TX mode
        if (!TX) {
          digitalWrite(vddEnable, HIGH);   // 7V VDD of PMA5-123 is ON
          TX=true; 
          }
       }  
    }  
}


bool VggProgram(void) {
  if (!TX) { // Only in RX Vgg can be programmed
    if (!digitalRead(switchPin)) { // Program mode
      analogWrite(VgateDAC, VggMin);
      delay(delaySteps); 
      digitalWrite(vddEnable, HIGH);   // 7V VDD of PMA5-123 is ON
      if (analogRead(currADC) > IdMax) {
         error = true;
         digitalWrite(vddEnable, LOW); // 7V VDD of PMA5-123 is OFF
         return false; 
         } 
      for (byte i=VggMin; i < VggMax; i++) {
        if (i == VggMax -1){
          error = true;
          return false;
        }
        else {
           analogWrite(VgateDAC, i);
           delay(delaySteps);   
           if (analogRead(currADC) > Idq) { 
              vggTarget = i;
              EEPROM.write(VggEeprom, i);
              EEPROM.write(IdEeprom, EepromProgrammed);
              digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is Off 
              delay(500); 
              return true;
              }
           }
        }
      }
   }
   return true;  
}
