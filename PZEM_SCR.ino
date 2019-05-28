#include <PZEM004T.h>
#include <LiquidCrystal.h>

int PDown = 0; //Digital pin to decrease power
int PUp = 1; //Digital pin to increase power
//int PBuzzer = A1; //Pin mode to decline buzzer if any.
//int PUserSetup = 2;
//int PAdminSetup = 3;

float MaxCurrentValue = 11.2;
float CurrentMargin = 1.0; //The allowable margin to increase power
int MaxPulse = 103; //The maximum range of pulses allowed.
float DeltaThreashold = 1;
int scrValue = 0;
int freshStart = 1;

float total = 0;
float cor = 2.096;
String version = "1.0-alfa";

PZEM004T pzem(2, 3); // (RX,TX) connect to TX,RX of PZEM
IPAddress ip(192, 168, 0, 1);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  //Serial.begin(9600);
  pzem.setAddress(ip);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.

  pinMode(PDown, OUTPUT);
  digitalWrite(PDown, HIGH);
  pinMode(PUp, OUTPUT);
  digitalWrite(PUp, HIGH);
//  pinMode(PBuzzer, OUTPUT);
  //pinMode(PUserSetup, INPUT_PULLUP);
  //pinMode(PAdminSetup, INPUT_PULLUP);
  // if (digitalRead(PUserSetup) == 0) //pin button pressed
  {
    //Display version

    //Configure user input

  }
  // if (digitalRead(PAdminSetup) == 0) //pin button pressed
  {
    //display version

    //Configure admin setup

  }
}

void loop()
{
  unsigned int i = 0;
  float deltaCur = 0.0;

  digitalWrite(PDown, HIGH);
  digitalWrite(PUp, HIGH);

  /* float v = pzem.voltage(ip);
    if (v < 0.0)
     v = 0.0;

    lcd.setCursor(0,0);
    lcd.print((float)v);
    lcd.print("V ");
  */
  float cur = cor * pzem.current(ip);
  printCur(cur);

  printScr();

  if (freshStart)
  {
    zero (PDown);
    freshStart = 0;
    scrValue = 0;
  }

  if (cur >= 0.0)
  {
    deltaCur = abs(MaxCurrentValue - cur);
    while ((deltaCur >= DeltaThreashold) && ((cur = cor * pzem.current(ip)) > MaxCurrentValue))
    {
      //Zero Out
      zero (PDown);
      deltaCur = abs(cur - MaxCurrentValue);
      printCur(cur);

      lcd.setCursor(14, 0);
      lcd.print("-0");
      scrValue = 0;
      printScr();
      printZero();
    }

    while ((deltaCur < DeltaThreashold) && ((cur = cor * pzem.current(ip)) > MaxCurrentValue) && (scrValue > 0))
    {
      //slow decrease
      digitalWrite(PUp, HIGH);
      printCur(cur);
      printMinus();
      for (int c = 0; c < 5; c++)
      {
        push (PDown);
        scrValue--;
      }
      if (scrValue < 0)
        scrValue = 0;
      printScr();
      deltaCur = abs(MaxCurrentValue - cur);
    }

    printPoint();


    while (((cur = cor * pzem.current(ip)) <= (MaxCurrentValue - CurrentMargin)) && (scrValue < MaxPulse))
    {
      //increase
      lcd.setCursor(0, 0);
      printCur(cur);
      printPlus();
      digitalWrite(PDown, HIGH);
      for (int c = 0; c < 2; c++)
      {
        push(PUp);
        scrValue++;
      }
      if (scrValue > MaxPulse)
        scrValue = MaxPulse;
      printScr();
      digitalWrite(PUp, HIGH);
    }
    printPoint();
    digitalWrite(PUp, HIGH);

    if ( (scrValue == 0) && (cur > MaxCurrentValue) )
    {
      //Beep system
      lcd.setCursor (0, 1);
      lcd.print("!!!");
    }
  }
  /*
    float p = pzem.power(ip);
    if(p >= 0.0)
    {
      lcd.setCursor(0,1);
      lcd.print(p);
      lcd.print("W ");
    }

    float e = pzem.energy(ip);
    if(e >= 0.0)
    {
      lcd.print((float)e);
      lcd.print("Wh ");
    }
  */
}

void push (int pin)
{
  digitalWrite(pin, LOW);
  delay (100);
  digitalWrite(pin, HIGH);
  delay (101);
  //digitalWrite(pin, LOW);
}

void zero (int pin)
{
  digitalWrite(pin, LOW);
  delay (13000);
  digitalWrite(pin, HIGH);
  printZero();
}

void printScr()
{
  lcd.setCursor(0, 1);
  lcd.print("scr:");
  lcd.print(scrValue);
  lcd.print("% ");
}

void printZero()
{
  lcd.setCursor(14, 0);
  lcd.print(" 0");
}

void printPoint()
{
  lcd.setCursor(14, 0);
  lcd.print(" .");
}

void printPlus()
{
  lcd.setCursor(14, 0);
  lcd.print(" +");
}

void printMinus()
{
  lcd.setCursor(15, 0);
  lcd.print("-");
}

void printCur(float cur)
{
  lcd.setCursor(0, 0);
  lcd.print("I= ");
  lcd.print(cur);
  lcd.print("A ");
}

