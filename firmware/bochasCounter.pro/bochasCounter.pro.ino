/* Tablero para conteo de puntos.
    Contador de puntos LOCAL/VISITANTE con máximo de conteo físico de hasta 18 puntos. Por software puede 
    modificarse para contar hasyta 99, con posibilidad de expandirse inclusive.
    Cuenta con multiplexación de salidas a frecuencia de cambio de dígito a 250Hz, debe ser revisada en caso
    que la cantidad de dígitos se incremente.
    Cada segmento de dígito, y el dígito completo, se habilita con un uno lógico. Las entradas se activan con
    flanco ascendente. Posee 5 entradas: aumentar puntos visitantes, aumentar puntos local, decrementar puntos
    visitantes, decrementar puntos local y reset de ambos puntos.
*/

/* 
 *  Se utiliza un timer para generar la multiplexación. Para esto se utiliza una librería extra.
 *  Cada vez que se alcanza el valor del timer 
*/

#include "TimerOne.h" // Includes timer1 library, used for multiplexing frequency

// define outputs
// define pins to code
#define out_a A5
#define out_b A4    
#define out_c A3   
#define out_d A2   
#define out_e A1   
#define out_f A0  
#define out_g 13

#define dig1 PD7   // When digital out=1, enables digit 1
#define dig2 PD6   
#define dig3 PD5   
#define dig4 PD4   

// define inputs
#define pinReset          8    // D8 reset pin
#define pinIncrementHome  9    // D9 increment home points pin, digits 1 and 2
#define pinDecrementHome  10   // D10 decrement home points pin, digits 1 and 2
#define pinIncrementGuest 11   // D11 increment guest points pin, digits 3 and 4
#define pinDecrementGuest 12   // D12 decrement guest points pin, digits 3 and 4

// global variables to detect if some button was pushed
boolean pinResetPushed;
boolean pinIncrementHomePushed;
boolean pinDecrementHomePushed;
boolean pinIncrementGuestPushed;
boolean pinDecrementGuestPushed;

#define pinDelay 50
#define multiplexingPeriod250Hz   4000 // used
#define multiplexingPeriod500Hz   2000 
#define multiplexingPeriod1000Hz  1000 

int digitEnabled = 1; // variable stores digit to show (one, two, three or four)

#define maxPoints 18
#define minPoints 0

int homePoints = 0; // stores home points
int guestPoints = 0; // stores guest points
int digit1 = 0;
int digit2 = 0;
int digit3 = 0;
int digit4 = 0; // each one stores the digits number of each score

// tasks list for state machine
enum tasksList {
  checkButtonPushed, // detect if some button was pushed
  getDigitsFromPoints // get all digits to show
};
enum tasksList task;

void setup() { // put your setup code here, to run once:
  configuration(); // System configuration
  initialization(); // Initialization of variables
  presentation(); // Initial presentation and LEDs, check if all is working well
  Timer1.start(); // Start multiplexing
  task = checkButtonPushed; // initial task
}

void loop() { // put your main code here, to run repeatedly:
  switch(task) {
    case checkButtonPushed:
      if(checkIfSomeButtonIsPushed()==true){
        task = getDigitsFromPoints;
      }
      break;
    case getDigitsFromPoints:
      getDigits(); // change number to show in display
      task = checkButtonPushed; // return to check if some button was pushed
      break;
    default:
      break;
  }
}

// Functions
void configuration(void) {
  // IO config
  pinMode(out_a, OUTPUT);
  pinMode(out_b, OUTPUT);
  pinMode(out_c, OUTPUT);
  pinMode(out_d, OUTPUT);
  pinMode(out_e, OUTPUT);
  pinMode(out_f, OUTPUT);
  pinMode(out_g, OUTPUT);
  pinMode(dig1, OUTPUT);
  pinMode(dig2, OUTPUT);
  pinMode(dig3, OUTPUT);
  pinMode(dig4, OUTPUT);
  pinMode(pinReset, INPUT_PULLUP);
  pinMode(pinIncrementHome, INPUT_PULLUP);
  pinMode(pinDecrementHome, INPUT_PULLUP);
  pinMode(pinIncrementGuest, INPUT_PULLUP);
  pinMode(pinDecrementGuest, INPUT_PULLUP);
  turnOffDigits();
  turnOffSegments();
  Timer1.initialize(multiplexingPeriod250Hz); // initialize timer one, 4000uS -> 250Hz 
  Timer1.attachInterrupt(changeDigit);
  Timer1.stop();
}

void reset(void){
  initialization();
}
void showZeroPoints(void){
  initialization();
}
void initialization(void){ // can work like reset too
  Timer1.stop();
  pinResetPushed=false; // reset input variables
  pinIncrementHomePushed=false;
  pinDecrementHomePushed=false;
  pinIncrementGuestPushed=false;
  pinDecrementGuestPushed=false;
  homePoints = 0; // reset points
  guestPoints = 0;
  digitEnabled = 1; // enables first digit
  getDigits();
  Timer1.start();
  task = checkButtonPushed;
}
void presentation(void) { // Presentation function
  guestPoints = 18;
  homePoints = 18;
  getDigits();
  int i;
  for(i=0;i<9;i++){ // loop for presentation
    if((i%2)==0){
      Timer1.start();
      delay(400);
      Timer1.stop();
    }else{
      turnOffSegments();
      turnOffDigits();
      delay(400);
    }
  }
  reset();
}
void getDigits(void) { // function to get each digit from points
  digit1 = (homePoints/10)%10;
  digit2 = homePoints%10;
  digit3 = (guestPoints/10)%10;
  digit4 = guestPoints%10;
}
void changeDigit(){ // timer one interrupt
  switch(digitEnabled) {
    case 1: // In this case, will be "1" or none (both segments turned off)
      if (digit1==0) {
        turnOffDigits();
        turnOffSegments();
      } else {
        turnOnDigit1();
        number1();
      }
      digitEnabled++;
      break;
    case 2:
      turnOnDigit2();
      showNumber(digit2);
      digitEnabled++;
      break;
    case 3: // In this case, will be "1" or none (both segments turned off)
      if (digit3==0){
        turnOffDigits();
        turnOffSegments();
      } else {
        turnOnDigit3();
        number1();
      }
      digitEnabled++;
      break;
    case 4:
      turnOnDigit4();
      showNumber(digit4);
      digitEnabled=1; // restart digit
      break;
    default:
      break;
  }
}
void showNumber(int num) { // show de number in digits
  switch (num) {
    case 0:
      number0();
      break;
    case 1:
      number1();
      break;
    case 2:
      number2();
      break;
    case 3:
      number3();
      break;
    case 4:
      number4();
      break;
    case 5:
      number5();
      break;
    case 6:
      number6();
      break;
    case 7:
      number7();
      break;
    case 8:
      number8();
      break;
    case 9:
      number9();
      break;
    default:
      break;
  }
}
// Input buttons functions
boolean checkIfSomeButtonIsPushed(void) {
  boolean i = false;
  if (pinResetWasPushed()==true) {
    i=true;
  } 
  if (pinIncrementHomeWasPushed()==true) {
    i=true;
  }
  if (pinDecrementHomeWasPushed()==true) {
    i=true;
  }
  if (pinIncrementGuestWasPushed()==true) {
    i=true;
  }
  if (pinDecrementGuestWasPushed()==true) {
    i=true;
  }
  return i;
}
boolean pinResetWasPushed(void) {
  boolean i;
  i=false;
  if ((digitalRead(pinReset)==LOW)&&(pinResetPushed==false)) { // Button was pushed
    delay(pinDelay); // delay
    if(digitalRead(pinReset)==LOW){ // still pushed after delay?
      pinResetPushed = true; // button pushed
    }
  } else if ((digitalRead(pinReset)==HIGH)&&(pinResetPushed==true)) { // Button was pulled
    delay(pinDelay); // delay after pull the button
    if(digitalRead(pinReset)==HIGH){ // still 
      pinResetPushed = false; // button pulled
      i=true; // the button was pushed
      reset(); // reset funcion
    }
  }
  return i;
}
boolean pinIncrementHomeWasPushed(void) {
  boolean i;
  i=false;
  if (digitalRead(pinIncrementHome)==LOW && pinIncrementHomePushed==false) { // Button was pushed
    delay(pinDelay);
    if(digitalRead(pinIncrementHome)==LOW){
      pinIncrementHomePushed = true;
    }
  } else if (digitalRead(pinIncrementHome)==HIGH && pinIncrementHomePushed==true) { // Button was pulled
    delay(pinDelay);
    if(digitalRead(pinIncrementHome)==HIGH){
      pinIncrementHomePushed = false;
      i=true;
      incrementHomePoints(); // increase home points
    }
  }
  return i;
}
boolean pinDecrementHomeWasPushed(void) {
  boolean i;
  i=false;
  if (digitalRead(pinDecrementHome)==LOW && pinDecrementHomePushed==false) { // Button was pushed
    delay(pinDelay);
    if(digitalRead(pinDecrementHome)==LOW){
      pinDecrementHomePushed=true;
    }
  } else if (digitalRead(pinDecrementHome)==HIGH && pinDecrementHomePushed==true) { // Button was pulled
    delay(pinDelay);
    if(digitalRead(pinDecrementHome)==HIGH){
      pinDecrementHomePushed = false;
      i=true;
      decrementHomePoints(); // decrease home points
    }
  }
  return i;
}
boolean pinIncrementGuestWasPushed(void) {
  boolean i;
  i=false;
  if (digitalRead(pinIncrementGuest)==LOW && pinIncrementGuestPushed==false) { // Button was pushed
    delay(pinDelay);
    if(digitalRead(pinIncrementGuest)==LOW){
      pinIncrementGuestPushed=true;  
    }
  } else if (digitalRead(pinIncrementGuest)==HIGH && pinIncrementGuestPushed==true) { // Button was pulled
    delay(pinDelay);
    if(digitalRead(pinIncrementGuest)==HIGH){
      pinIncrementGuestPushed=false;
      i=true;
      incrementGuestPoints(); // increase guest points
    }
  }
  return i;
}
boolean pinDecrementGuestWasPushed(void) {
  boolean i;
  i=false;
  if (digitalRead(pinDecrementGuest)==LOW && pinDecrementGuestPushed==false) { // Button was pushed
    delay(pinDelay);
    if(digitalRead(pinDecrementGuest)==LOW){
      pinDecrementGuestPushed=true;
    }
  } else if (digitalRead(pinDecrementGuest)==HIGH && pinDecrementGuestPushed==true) { // Button was pulled
    delay(pinDelay);
    if(digitalRead(pinDecrementGuest)==HIGH){
      pinDecrementGuestPushed=false;
      i=true;
      decrementGuestPoints(); // decrease guest points
    }
  }
  return i;
}
// Points funcions
void incrementHomePoints(void) {
  if (homePoints < maxPoints){
    ++homePoints;
  }
}

void decrementHomePoints(void) {
  if (homePoints > minPoints){
    --homePoints;
  }
}

void incrementGuestPoints(void) {
  if (guestPoints < maxPoints){
    ++guestPoints;
  }
}

void decrementGuestPoints(void) {
  if (guestPoints > minPoints){
    --guestPoints;
  }
}
void turnOffDigits(void) {
  digitalWrite(dig1, LOW);
  digitalWrite(dig2, LOW);
  digitalWrite(dig3, LOW);
  digitalWrite(dig4, LOW);
}
void turnOnDigit1(void) { // if the digit X is turned on, the rest must be TURNED OFF
  digitalWrite(dig1, HIGH);
  digitalWrite(dig2, LOW);
  digitalWrite(dig3, LOW);
  digitalWrite(dig4, LOW);
}
void turnOnDigit2(void) {
  digitalWrite(dig1, LOW);
  digitalWrite(dig2, HIGH);
  digitalWrite(dig3, LOW);
  digitalWrite(dig4, LOW);
}
void turnOnDigit3(void) {
  digitalWrite(dig1, LOW);
  digitalWrite(dig2, LOW);
  digitalWrite(dig3, HIGH);
  digitalWrite(dig4, LOW);
}
void turnOnDigit4(void) {
  digitalWrite(dig1, LOW);
  digitalWrite(dig2, LOW);
  digitalWrite(dig3, LOW);
  digitalWrite(dig4, HIGH);
}
void turnOffSegments(void) {
  turnOff_a();
  turnOff_b();
  turnOff_c();
  turnOff_d();
  turnOff_e();
  turnOff_f();
  turnOff_g();
}
void turnOnAllSegments(void){
  turnOn_a();
  turnOn_b();
  turnOn_c();
  turnOn_d();
  turnOn_e();
  turnOn_f();
  turnOn_g();
}
void number0(void) {
  turnOn_a();
  turnOn_b();
  turnOn_c();
  turnOn_d();
  turnOn_e();
  turnOn_f();
  turnOff_g();
}
void number1(void) {
  turnOff_a();
  turnOn_b();
  turnOn_c();
  turnOff_d();
  turnOff_e();
  turnOff_f();
  turnOff_g();
}
void number2(void) {
  turnOn_a();
  turnOn_b();
  turnOff_c();
  turnOn_d();
  turnOn_e();
  turnOff_f();
  turnOn_g();
}
void number3(void) {
  turnOn_a();
  turnOn_b();
  turnOn_c();
  turnOn_d();
  turnOff_e();
  turnOff_f();
  turnOn_g();
}
void number4(void) {
  turnOff_a();
  turnOn_b();
  turnOn_c();
  turnOff_d();
  turnOff_e();
  turnOn_f();
  turnOn_g();
}
void number5(void) {
  turnOn_a();
  turnOff_b();
  turnOn_c();
  turnOn_d();
  turnOff_e();
  turnOn_f();
  turnOn_g();
}
void number6(void) {
  turnOn_a();
  turnOn_b();
  turnOn_c();
  turnOn_d();
  turnOn_e();
  turnOff_f();
  turnOn_g();
}
void number7(void) {
  turnOn_a();
  turnOn_b();
  turnOn_c();
  turnOff_d();
  turnOff_e();
  turnOff_f();
  turnOff_g();
}
void number8(void) {
  turnOn_a();
  turnOn_b();
  turnOn_c();
  turnOn_d();
  turnOn_e();
  turnOn_f();
  turnOn_g();
}
void number9(void) {
  turnOn_a();
  turnOn_b();
  turnOn_c();
  turnOn_d();
  turnOff_e();
  turnOn_f();
  turnOn_g();
}
void turnOn_a(void) {
  digitalWrite(out_a, HIGH);
}
void turnOff_a(void) {
  digitalWrite(out_a, LOW);
}
void turnOn_b(void) {
  digitalWrite(out_b, HIGH);
}
void turnOff_b(void) {
  digitalWrite(out_b, LOW);
}
void turnOn_c(void) {
  digitalWrite(out_c, HIGH);
}
void turnOff_c(void) {
  digitalWrite(out_c, LOW);
}
void turnOn_d(void) {
  digitalWrite(out_d, HIGH);
}
void turnOff_d(void) {
  digitalWrite(out_d, LOW);
}
void turnOn_e(void) {
  digitalWrite(out_e, HIGH);
}
void turnOff_e(void) {
  digitalWrite(out_e, LOW);
}
void turnOn_f(void) {
  digitalWrite(out_f, HIGH);
}
void turnOff_f(void) {
  digitalWrite(out_f, LOW);
}
void turnOn_g(void) {
  digitalWrite(out_g, HIGH);
}
void turnOff_g(void) {
  digitalWrite(out_g, LOW);
}
