#include <IRremote.h>
#include <LiquidCrystal.h>

const int RECV_PIN = 10;
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;

const int rsPin = 12;
const int ePin = 11;
const int d4Pin = 5;
const int d5Pin = 4;
const int d6Pin = 3;
const int d7Pin = 2;

LiquidCrystal lcd(rsPin, ePin, d4Pin, d5Pin, d6Pin, d7Pin);


void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);
  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop(){
  if (irrecv.decode(&results)){
 
        if (results.value == 0XFFFFFFFF)
          results.value = key_value;

        lcd.setCursor(0, 0);

        switch(results.value){
          case 0xFFA25D:
          Serial.println("CH-");
          lcd.print("CH- ");
          break;
          case 0xFF629D:
          Serial.println("CH");
          lcd.print("CH  ");
          break;
          case 0xFFE21D:
          Serial.println("CH+");
          lcd.print("CH+ ");
          break;
          case 0xFF22DD:
          Serial.println("|<<");
          lcd.print("|<< ");
          break;
          case 0xFF02FD:
          Serial.println(">>|");
          lcd.print(">>| ");
          break ;  
          case 0xFFC23D:
          Serial.println(">||");
          lcd.print(">|| ");
          break ;               
          case 0xFFE01F:
          Serial.println("-");
          lcd.print("-   ");
          break ;  
          case 0xFFA857:
          Serial.println("+");
          lcd.print("+   ");
          break ;  
          case 0xFF906F:
          Serial.println("EQ");
          lcd.print("EQ  ");
          break ;  
          case 0xFF6897:
          Serial.println("0");
          lcd.print("0   ");
          break ;  
          case 0xFF9867:
          Serial.println("100+");
          lcd.print("100+ ");
          break ;
          case 0xFFB04F:
          Serial.println("200+");
          lcd.print("200+");
          break ;
          case 0xFF30CF:
          Serial.println("1");
          lcd.print("1   ");
          break ;
          case 0xFF18E7:
          Serial.println("2");
          lcd.print("2   ");
          break ;
          case 0xFF7A85:
          Serial.println("3");
          lcd.print("3   ");
          break ;
          case 0xFF10EF:
          Serial.println("4");
          lcd.print("4   ");
          break ;
          case 0xFF38C7:
          Serial.println("5");
          lcd.print("5   ");
          break ;
          case 0xFF5AA5:
          Serial.println("6");
          lcd.print("6   ");
          break ;
          case 0xFF42BD:
          Serial.println("7");
          lcd.print("7   ");
          break ;
          case 0xFF4AB5:
          Serial.println("8");
          lcd.print("8   ");
          break ;
          case 0xFF52AD:
          Serial.println("9");
          lcd.print("9   ");
          break ;      
        }
        key_value = results.value;
        irrecv.resume(); 
  }
}
