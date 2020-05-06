#ifndef ControLCD_h
#include "Arduino.h";

namespace ENUMS {
    enum UNARY {DECREMENT, INCREMENT};
    enum SHIFTS {NO_SHIFT, SHIFT};
    enum CURSOR {CURSOR_OFF, CURSOR_ON};
    enum BLINK {BLINK_OFF, BLINK_ON};
    enum SHIFT_DIRECTION {LEFT, RIGHT};
    enum DISPLAY_TOGGLE {DISPLAY_OFF, DISPLAY_ON};
    enum BITS {FOUR_BITS, EIGHT_BITS};
    enum LINES {ONE_LINE, TWO_LINES};
    enum FONT {FIVEbyEIGHT, FIVEbyTEN};
};

class LowLevelControl {
  private:
    uint8_t* writePins;
    uint8_t* dataPins;
  protected:
    LowLevelControl(uint8_t wPins[3], uint8_t dPins[8]);
    LowLevelControl(uint8_t RS, uint8_t RW, uint8_t E, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7);
    LowLevelControl();
    void writeInstruction(bool RS, bool RW, byte* data);
    void initalize();
  private:
    void writeDataPins(byte* data);
    void writeEnablePins(bool* setPins);
    bool isBusy();
    void sendInstruction(bool RS, bool RW);
    
};

using namespace ENUMS;
class ScreenControl : private LowLevelControl {  
    private:
      int charpos;      
    public:
      ScreenControl(uint8_t wPins[3], uint8_t dPins[8]);
      ScreenControl(uint8_t RS, uint8_t RW, uint8_t E, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7);
      void clearScreen();
      void returnHome();
      void entryMode(UNARY cursorDirection, SHIFTS shift);
      void displayOnOff(DISPLAY_TOGGLE toggle, CURSOR cursorLCD, BLINK blinkLCD);
      void displayShift(SHIFT_DIRECTION shift);
      void cursorShift(SHIFT_DIRECTION shift);
      void functionSet(BITS bits, LINES lines, FONT font); // bits : 0 = 4, 1 = 8. lines: 1 = 2 lines, 0 = 1 line. font : 1 = 5 x 10, 0 = 5 x 8. 
      void writeChar(char c);
      void nextLine();
      void writeString(String s);
      void start();
};

class ControlLCD : public ScreenControl  {
  public:
    ControlLCD(uint8_t wPins[3], uint8_t dPins[8]);
    ControlLCD(uint8_t RS, uint8_t RW, uint8_t E, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7);
};

#endif