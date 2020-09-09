//=== Wireless Calculator by Hari Wiguna, Sep 2020 ==

#include "IRremote.h"
#include "IR.h"

IRrecv irrecv(RECEIVER);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

//===================================
// LCD
/*
  The circuit:
   LCD RS pin to digital pin 7
   LCD Enable pin to digital pin 8
   LCD D4 pin to digital pin 9
   LCD D5 pin to digital pin 10
   LCD D6 pin to digital pin 11
   LCD D7 pin to digital pin 12
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)
*/
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 6, 12);

//=== Calculator ===
String operandA = "0";
char opCode = ' ';
String operandB;
byte state = 1;
#define STATE1_ENTERING_A 1
#define STATE3_ENTERING_B 2
//#define divChar char(0b11111101)
#define divChar '/'

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decode");
  irrecv.enableIRIn();
  refreshLCD();
}

void refreshLCD_debug()
{
  lcd.setCursor(0, 0); lcd.print("                ");
  lcd.setCursor(0, 1); lcd.print("                ");
  lcd.setCursor(0, 0); lcd.print("A:" + operandA + " " + opCode);
  lcd.setCursor(0, 1); lcd.print("B:" + operandB);
}

void refreshLCD()
{
  lcd.setCursor(0, 0); lcd.print("~~ Calculator " + String(char(0b01111111)) + String(char(0b01111111)));
  lcd.setCursor(0, 1); lcd.print("                ");
  lcd.setCursor(0, 1); lcd.print(operandA);
  if (opCode != ' ' && opCode != '=') lcd.print(" " + String(opCode) + " " + operandB);
}

void showOnLCD(String txt)
{
  // lcd.setCursor(0, 0); lcd.print("~~ Calculator " + String(char(0b01111111))+ String(char(0b01111111)));
  lcd.setCursor(0, 1); lcd.print("                ");
  lcd.setCursor(0, 1); lcd.print(txt);
}

void pressNumber(String txt) {
  switch (state) {
    case STATE1_ENTERING_A:
      if (opCode != ' ') {
        operandA = "";
        opCode = ' ';
      }
      if (operandA == "0") operandA = ""; // kill leading zero.
      if (txt == "." ) { // Insert decimal point only if there wasn't one there yet.
        if (operandA.indexOf('.') < 0) operandA.concat(txt);
      }
      else
        operandA.concat(txt); // Insert entered digit into A
      refreshLCD();
      break;

    case STATE3_ENTERING_B:
      if (txt == "." ) { // Insert decimal point only if there wasn't one there yet.
          if (operandB.indexOf('.') < 0) operandB.concat(txt);
        }
        else
          operandB.concat(txt); // Insert entered digit into B
        refreshLCD();
        break;
      }
  }

  void pressOp(char op) {
    switch (op)
    {
      case 'c': // Clear
        opCode = ' ';
        operandA = "0";
        operandB = "";
        state = STATE1_ENTERING_A;
        break;

      case '=': // Equal
        doMath();
        opCode = op;
        operandB = "";
        state = STATE1_ENTERING_A;
        break;

      default: // Add, Subtract, Multiple, and Divide
        if (state == STATE3_ENTERING_B) { // if they press an operator while they're supposed to be entering B
          doMath();
        }
        // Save the op and start taking B
        opCode = op;
        operandB = "";
        state = STATE3_ENTERING_B;
    }
    refreshLCD();
  }


  void doMath() {
    if (opCode == '+') operandA = String( operandA.toDouble() + operandB.toDouble() );
    if (opCode == '-') operandA = String( operandA.toDouble() - operandB.toDouble() );
    if (opCode == '*') operandA = String( operandA.toDouble() * operandB.toDouble() );
    if (opCode == divChar) operandA = String( operandA.toDouble() / operandB.toDouble() );
  }

  void changeSign() {
    switch (state) {
      case STATE1_ENTERING_A:
        operandA = String( - operandA.toDouble() );
        break;
      case STATE3_ENTERING_B:
        operandB = String( - operandB.toDouble() );
        break;
    }
    refreshLCD();
  }

  void loop()
  {
    int tmpValue;
    if (irrecv.decode(&results)) // have we received an IR signal?
    {
      switch (results.value)
      {
        case KEY_POWER: pressOp('c'); break;
        case KEY_FUNC_STOP: changeSign(); break;
        case KEY_VOL_ADD: pressOp('+'); break;
        case KEY_FAST_BACK: pressOp(divChar); break;
        case KEY_PAUSE: showOnLCD(" by Hari Wiguna");    break;
        case KEY_FAST_FORWARD: pressOp('*'); break;
        // case KEY_DOWN: showOnLCD("DOWN");    break;
        case KEY_VOL_DE: pressOp('-');  break;
        // case KEY_UP: showOnLCD("UP");    break;
        case KEY_EQ: pressOp('=');       break;
        case KEY_ST_REPT:  pressNumber("."); break;
        case KEY_0: pressNumber("0");    break;
        case KEY_1: pressNumber("1");    break;
        case KEY_2: pressNumber("2");    break;
        case KEY_3: pressNumber("3");    break;
        case KEY_4: pressNumber("4");    break;
        case KEY_5: pressNumber("5");    break;
        case KEY_6: pressNumber("6");    break;
        case KEY_7: pressNumber("7");    break;
        case KEY_8: pressNumber("8");    break;
        case KEY_9: pressNumber("9");    break;
        // case REPEAT: showOnLCD(" REPEAT"); break;
        // default:
        //    showOnLCD(" other button   "); break;
      }// End Switch
      irrecv.resume(); // receive the next value
    }
  }
