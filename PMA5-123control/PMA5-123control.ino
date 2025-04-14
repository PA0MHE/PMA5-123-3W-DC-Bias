/* DC control Minicircuit PMA5-123-3W+ 3W 10GHz GaAs MMIC
 *  
 * Maarten PA0MHE
 * 
 * 31-03-2025
 * 
 */


#define pttPin     PIN_PA3 // package pin 7 on AtTiny 412
#define vddEnable  PIN_PA2 // package pin 5
#define currADC    PIN_PA7 // package pin 3
#define switchPin  PIN_PA1 // package pin 4
#define VgateDAC   PIN_PA6 // DAC It must be package pin 2

#define VgateMin 0 // DAC value for lowest Vgate useally -1.5V
#define VgateTarget 100 //
#define VgateMax 255 // DAC value for highest Vgate -0.4V
#define targetCurrent 400 // 51K 10x, 500 =~ 1.2A 400=~1A
#define currMax  1000  // 2.4A
#define delaySteps 10

bool TX = false;
bool error = false;

int stabVgate =VgateMin;

void setup()
{
  digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF 
  pinMode(vddEnable, OUTPUT);

  DACReference(INTERNAL4V3);
  analogReference(INTERNAL2V5);  

  pinMode(pttPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);
  
  analogWrite(VgateDAC, VgateMin);
  
  delay(4000);
  analogRead(currADC);
}

  

void loop() {
//VGateTest(0,255);

// NormalControl();

// StabControl();

 FixedVggControl(VgateTarget);

 } // End loop


void StabControl(void) { 
  if (analogRead(currADC) > currMax) {
   error = true;
   SwitchOffPA();
  }
  if (!error) StabCurrent(targetCurrent);
}


void NormalControl(void) {
  if (analogRead(currADC) > currMax) {
   error = true;
   SwitchOffPA();
  }
  if (!error) {
     if (digitalRead(pttPin)) { // RX mode
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
  analogWrite(VgateDAC, VgateMin);
  delay(delaySteps); 
  digitalWrite(vddEnable, HIGH);   // 7V VDD of PMA5-123 is ON 
  for (int i=VgateMin; i < VgateMax; i++) {
    if (i == VgateMax -1) {
      error = true;
      return false;
    }
    analogWrite(VgateDAC, i);
    delay(delaySteps);   
    if (analogRead(currADC) > targetCurrent) {
      return true;
    }
  }
}


void SwitchOffPA(void) {
  digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF 
  analogWrite(VgateDAC, VgateMin);
  TX = false;
}


void FixedVggControl(int Vgg) {
  analogWrite(VgateDAC, Vgg);
  if (analogRead(currADC) > currMax) {
   error = true;
   digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF 
  }
  if (!error) {
     if (digitalRead(pttPin)) { // RX mode
        if (TX) digitalWrite(vddEnable, LOW);   // 7V VDD of PMA5-123 is OFF 
      }
      else {    // TX mode
        if (!TX) {
          digitalWrite(vddEnable, HIGH);   // 7V VDD of PMA5-123 is ON
          TX=true; 
          }
       }  
    }  
}

void StabCurrent(int cur) {
  digitalWrite(vddEnable, HIGH);   // 7V VDD of PMA5-123 is ON 
  if (analogRead(currADC) < cur -2) {
    stabVgate = stabVgate+1;
  }
    if (analogRead(currADC) > cur +2) {
    stabVgate =stabVgate-1;
  }
      analogWrite(VgateDAC,stabVgate );
}


void VGateTest(int minV, int maxV) {
  digitalWrite(vddEnable, LOW);   // PMA5-123 is OFF 
  analogWrite(VgateDAC, maxV);
  delay(4000);
  analogWrite(VgateDAC, minV);
  delay(4000);
}
