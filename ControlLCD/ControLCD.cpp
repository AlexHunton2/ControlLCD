#include "Arduino.h";
#include "ControlLCD.h";

LowLevelControl::LowLevelControl(uint8_t wPins[3], uint8_t dPins[8]){
  for (int i=0; i<3; i++) {
      writePins[i] = wPins[i];
    }
   for (int i=0; i<8; i++) {
     dataPins[i] = dPins[i];
   } 
   initalize();
}

LowLevelControl::LowLevelControl(uint8_t RS, uint8_t RW, uint8_t E, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7) {
  uint8_t wPins[3] = {RS, RW, E}; 
  uint8_t dPins[8] = {D0, D1, D2, D3, D4, D5, D6, D7};
  for (int i=0; i<3; i++) {
      writePins[i] = wPins[i];
    }
   for (int i=0; i<8; i++) {
     dataPins[i] = dPins[i];
   }
   initalize();
}

void LowLevelControl::initalize() {
  delay(50);
    for (int i=0; i<3; i++) {
      pinMode(writePins[i], OUTPUT);
      digitalWrite(writePins[i], HIGH);
    }
    for (int i=0; i<8; i++) {
      pinMode(dataPins[i], OUTPUT);
      digitalWrite(writePins[i], LOW);
    }
}

void LowLevelControl::writeInstruction(bool RS, bool RW, byte* data) {
    delay(2);
    while (!isBusy());
    this->writeDataPins(data);
    this->sendInstruction(RS, RW);
}

void LowLevelControl::writeEnablePins(bool* setPins) {
  for (int i=0; i<3; i++) {
      digitalWrite(writePins[i], setPins[i]);
  }
}

void LowLevelControl::writeDataPins(byte* data) {
  for (int i=0; i<8; i++) {
      digitalWrite(dataPins[i], data[i]);
  }
}

void LowLevelControl::sendInstruction(bool RS, bool RW) {
  bool pins[3] = {RS, RW, 0};
  this->writeEnablePins(pins);
  pins[2] = 1;
  this->writeEnablePins(pins);
  pins[2] = 0;
  this->writeEnablePins(pins);
}

bool LowLevelControl::isBusy() {
  pinMode(dataPins[8], INPUT);
  bool clearPins[3] = {0, 1, 0};
  this->writeEnablePins(clearPins);
  clearPins[2] = 1;
  this->writeEnablePins(clearPins);
  bool busy = digitalRead(dataPins[8]);
  clearPins[2] = 0;
  this->writeEnablePins(clearPins);
  if (!busy)
    pinMode(dataPins[8], OUTPUT);
  return busy;
}

ScreenControl::ScreenControl(uint8_t wPins[3], uint8_t dPins[8])
: LowLevelControl(wPins, dPins) {}

ScreenControl::ScreenControl(uint8_t RS, uint8_t RW, uint8_t E, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7)
: LowLevelControl(RS, RW, E, D0, D1, D2, D3, D4, D5, D6, D7) {}

void ScreenControl::clearScreen() {
  byte z[8] = {1, 0, 0, 0, 0, 0, 0, 0}; //Clear
  this->writeInstruction(0, 0, z);
  this->charpos = 0;
}

void ScreenControl::returnHome() {
  byte data[8] = {0, 1, 0, 0, 0, 0, 0, 0};
  this->writeInstruction(0, 0, data); 
  this->charpos = 0;
}

void ScreenControl::entryMode(UNARY cursorDirection, SHIFTS shift) {
  byte data[8] = {shift, cursorDirection, 1, 0, 0, 0, 0, 0};
  this->writeInstruction(0, 0, data); 
}

void ScreenControl::displayOnOff(DISPLAY_TOGGLE toggle, CURSOR cursorLCD, BLINK blinkLCD) {
  byte data[8] = {blinkLCD, cursorLCD, toggle, 1, 0, 0, 0, 0};
  this->writeInstruction(0, 0, data); 
}

void ScreenControl::displayShift(SHIFT_DIRECTION shift) {
  byte data[8] = {0, 0, shift, 1, 1, 0, 0, 0};
  this->writeInstruction(0, 0, data); 
}

void ScreenControl::cursorShift(SHIFT_DIRECTION shift) {
  byte data[8] = {0, 0, shift, 0, 1, 0, 0, 0};
  this->writeInstruction(0, 0, data); 
}

void ScreenControl::functionSet(BITS bits, LINES lines, FONT font) {
  byte data[8] = {0, 0, font, lines, bits, 1, 0, 0};
  this->writeInstruction(0, 0, data); 
}

void ScreenControl::writeChar(char c) {
  uint8_t bin[8];
  for (int i = 0; i < 8; i++) {
    bin[i] = (c % 2);
    c = floor(c/2);
  }
  this->charpos++;
  this->writeInstruction(1, 0, bin);
}

void ScreenControl::nextLine() {
  byte cgram[8] = {0, 0, 0, 1, 0, 1, 1, 0}; //Set CGRAM
  this->writeInstruction(0, 0, cgram);  
  byte ddram[8] = {0, 0, 0, 1, 0, 1, 0, 1}; //Set DDRAM
  this->writeInstruction(0, 0, ddram);  
}

void ScreenControl::writeString(String s) {
    for (int i = 0; i < s.length(); i++) {
      if (charpos == 16) { //And if two lines is allowed
        this->nextLine();
      }
      this->writeChar(s.charAt(i));
    }
}

void ScreenControl::start() {
  initalize();
  functionSet(EIGHT_BITS, TWO_LINES, FIVEbyEIGHT);
  displayOnOff(DISPLAY_ON, CURSOR_ON, BLINK_OFF);
  entryMode(INCREMENT, NO_SHIFT);
  clearScreen();  
}

ControlLCD::ControlLCD(uint8_t wPins[3], uint8_t dPins[8])
: ScreenControl(wPins, dPins) {}

ControlLCD::ControlLCD(uint8_t RS, uint8_t RW, uint8_t E, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7)
: ScreenControl(RS, RW, E, D0, D1, D2, D3, D4, D5, D6, D7) {} 