#ifndef LCDKeypadH
#define LCDKeypadH


unsigned long debounceDelay = 1000;
unsigned long lastTimeButtonsChanged[5];
typedef enum {LEFT, RIGHT, UP, DOWN, SELECT, NONE} Button;

class LCDKeypad : public LiquidCrystal{
  public:
    LCDKeypad(short a, short b, short c, short d, short e, short f);
    Button getButtonPress();
};

LCDKeypad::LCDKeypad(short a, short b, short c, short d, short e, short f):LiquidCrystal(a, b, c, d, e, f){};


Button LCDKeypad::getButtonPress()
    {
      unsigned short reading = analogRead(0);
      switch (reading)
      {
        case 479:
          {
            if ((millis() - lastTimeButtonsChanged[0]) > debounceDelay)
            {
              lastTimeButtonsChanged[0] = millis();
              return LEFT;
            }
            else
            {
              return NONE;
            }
    
          }
          break;
        case 0:
          {
            if ((millis() - lastTimeButtonsChanged[1]) > debounceDelay)
            {
              lastTimeButtonsChanged[1] = millis();
              return RIGHT;
            }
            else
            {
              return NONE;
            }
          }
          break;
        case 130:
        case 131:
          {
            if ((millis() - lastTimeButtonsChanged[2]) > debounceDelay)
            {
              lastTimeButtonsChanged[2] = millis();
              return UP;
            }
            else
            {
              return NONE;
            }
          }
          break;
        case 307:
          {
            if ((millis() - lastTimeButtonsChanged[3]) > debounceDelay)
            {
              lastTimeButtonsChanged[3] = millis();
              return DOWN;
            }
            else
            {
              return NONE;
            }
          }
          break;
        case 722:
          {
            if ((millis() - lastTimeButtonsChanged[4]) > debounceDelay)
            {
              lastTimeButtonsChanged[4] = millis();
              return SELECT;
            }
            else
            {
              return NONE;
            }
          }
          break;
        case 1023:
          {
            return NONE;
          }
          break;
      }
    }
#endif
