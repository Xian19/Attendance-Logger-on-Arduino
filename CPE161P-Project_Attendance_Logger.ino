#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <virtuabotixRTC.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

virtuabotixRTC myRTC(1, 7, 8);

const byte ROWS = 4; 
const byte COLS = 4; 
const int trigPin = 6;
const int echoPin = 5;
const int SS_PIN = 10;
const int RST_PIN = 9;
const int ledPin = 0;

int countdownRFID;
int currentMonth;
int checkMonth;
int currentDay;
int checkDay;

const String password_1 = "123456"; // change your password here
const String password_2 = "654321";  // change your password here
const String password_3 = "456123";   // change your password here
String input_password;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC1[4] = {109, 96, 252, 49};
byte nuidPICC2[4] = {83, 221, 251, 52};
byte nuidPICC3[4] = {226, 98, 188, 137};


char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3}; 
byte colPins[COLS] = {4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

long duration;
int distance;
bool logOne = false;
bool logTwo = false;
bool logThree = false;
bool menuLog;

int userOneOnTime = 0;
int userOneLate = 0;
int userOneAbsent = 0;

int userTwoOnTime = 0;
int userTwoLate = 0;
int userTwoAbsent = 0;

int userThreeOnTime = 0;
int userThreeLate = 0;
int userThreeAbsent = 0;




void setup(){
  //Serial.begin(9600);
  input_password.reserve(5); // maximum input characters is 33, change if needed
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(ledPin, OUTPUT);
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  //myRTC.setDS1302Time(00, 34, 20, 2, 23, 10, 2023); //Here you write your actual time/date as shown above 
  currentDay = 0;
  checkDay = myRTC.dayofmonth;
  currentMonth = 0;
  checkMonth = myRTC.month;

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

}
  
void loop(){
  countdownRFID = 10;
  checkDay = myRTC.dayofmonth;
  checkMonth = myRTC.month;
  currentTime();
  myRTC.updateTime();
  lcd.setCursor(10,0);
  lcd.print(myRTC.hours);
  lcd.print(":");
  lcd.print(myRTC.minutes);
  lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  lcd.print("1.Log in");
  lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
  lcd.print("2.Show Log");
  lcd.setCursor(11,1);
  lcd.print(myRTC.month);
  lcd.print("/");
  lcd.print(myRTC.dayofmonth);

  char menuKey = customKeypad.getKey();
  
  if (menuKey){
    Serial.println(menuKey);
    if (menuKey == '1') {
      ultrasonicSense();
    }
    else if (menuKey == '2') {
      lcd.clear();
      menuLog = true;
      userPoints();
    }
    else {
      lcd.clear();
      lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
      lcd.print("Incorrect key!");
      delay(2000);
      lcd.clear();
    }
  }
}

void currentTime() {
  if (currentDay != checkDay) {
    logOne = false;
    logTwo = false;
    logThree = false;
    currentDay = checkDay;
  }
  if (currentMonth != checkMonth) {
    userOneOnTime = 0;
    userOneLate = 0;
    userOneAbsent = 0;
    userTwoOnTime = 0;
    userTwoLate = 0;
    userTwoAbsent = 0;
    userThreeOnTime = 0;
    userThreeAbsent = 0;
    currentMonth = checkMonth;
  }
}

void userPoints() {
  if (menuLog == true) {
    lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
    lcd.print("Which User?");
    lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
    lcd.print("1, 2, or 3?");
    menuLog = false;
  }
  char userKey = customKeypad.getKey();

  if (userKey) {
    if (userKey == '1') {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("USER ONE STATS");
      lcd.setCursor(0,1);
      lcd.print(String("O= ") + String(userOneOnTime) + String(" L= ") + String(userOneLate) + String(" A= ") + String(userOneAbsent));
      delay(5000);
      lcd.clear();
      return;
    }
    else if (userKey == '2'){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("USER TWO STATS");
      lcd.setCursor(0,1);
      lcd.print(String("O= ") + String(userTwoOnTime) + String(" L= ") + String(userTwoLate) + String(" A= ") + String(userTwoAbsent));
      delay(5000);
      lcd.clear();
      return;
    }
    else if (userKey == '3'){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("USER THREE STATS");
      lcd.setCursor(0,1);
      lcd.print(String("O= ") + String(userThreeOnTime) + String(" L= ") + String(userThreeLate) + String(" A= ") + String(userThreeAbsent));
      delay(5000);
      lcd.clear();
      return;
    }
    else {
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("INVALID KEY!");
      delay(2000);
      userPoints();
    }
  }
  else {
    delay(200);
    userPoints();
  }
}
void keyPadInput() {
    lcd.setCursor(0,0);
    lcd.print(input_password);
    char key = customKeypad.getKey();
    if(key) {
      if(key == '*') {
        input_password = ""; // reset the input password
        keyPadLogin();
      } 
      else if(key == '#') {
        if(input_password == password_1) {
          if (!logOne) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WELCOME USER ONE");
            delay(3000);
            if (myRTC.hours == 8 && myRTC.minutes <= 15) {
              digitalWrite(ledPin,HIGH);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER ONE");
              lcd.setCursor(0,1);
              lcd.print("ON TIME!");
              userOneOnTime++;
              logOne = true;
              delay(5000);
              digitalWrite(ledPin,LOW);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 15 && myRTC.minutes <= 30) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER ONE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE LATE!");
              userOneLate++;
              logOne = true;
              delay(5000);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 30 || myRTC.hours > 8 && myRTC.hours <= 24 ) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER ONE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE ABSENT!");
              userOneAbsent++;
              logOne = true;
              delay(5000);
            }
            else {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("PLEASE COME BACK");
              lcd.setCursor(0,1);
              lcd.print("AT WORK TIME!");
              delay(5000);
            }
            lcd.clear();
            return;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("YOU HAVE ALREADY");
            lcd.setCursor(0,1);
            lcd.print("LOGGED IN TODAY!");
            delay(5000);
            return;
          }
        } 
        else if (input_password == password_2) {
          if (!logTwo) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WELCOME USER TWO");
            delay(3000);
            if (myRTC.hours == 8 && myRTC.minutes <= 15) {
              digitalWrite(ledPin,HIGH);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER TWO");
              lcd.setCursor(0,1);
              lcd.print("ON TIME!");
              userTwoOnTime++;
              logTwo = true;
              delay(5000);
              digitalWrite(ledPin,LOW);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 15 && myRTC.minutes <= 30) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER TWO");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE LATE!");
              userTwoLate++;
              logTwo = true;
              delay(5000);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 30 || myRTC.hours > 8 && myRTC.hours <= 24 ) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER TWO");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE ABSENT!");
              userTwoAbsent++;
              logTwo = true;
              delay(5000);
            }
            else {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("PLEASE COME BACK");
              lcd.setCursor(0,1);
              lcd.print("AT WORK TIME!");
              delay(5000);
            }
            lcd.clear();
            return;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("YOU HAVE ALREADY");
            lcd.setCursor(0,1);
            lcd.print("LOGGED IN TODAY!");
            delay(5000);
            return;
          }
        }
        else if (input_password == password_3) {
          if (!logThree) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WELCOME USER");
            lcd.setCursor(1,1);
            lcd.print("THREE");
            delay(3000);
            if (myRTC.hours == 8 && myRTC.minutes <= 15) {
              digitalWrite(ledPin,HIGH);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER THREE");
              lcd.setCursor(0,1);
              lcd.print("ON TIME!");
              userThreeOnTime++;
              logThree = true;
              delay(5000);
              digitalWrite(ledPin,LOW);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 15 && myRTC.minutes <= 30) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER THREE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE LATE!");
              userThreeLate++;
              logThree = true;
              delay(5000);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 30 || myRTC.hours > 8 && myRTC.hours <= 24 ) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER THREE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE ABSENT!");
              userThreeAbsent++;
              logThree = true;
              delay(5000);
            }
            else {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("PLEASE COME BACK");
              lcd.setCursor(0,1);
              lcd.print("AT WORK TIME!");
              delay(5000);
            }
            lcd.clear();
            return;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("YOU HAVE ALREADY");
            lcd.setCursor(0,1);
            lcd.print("LOGGED IN TODAY!");
            delay(5000);
            return;
          }
        }
        else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("INVALID PASSCODE!");
          delay(5000);
          input_password = "";
          lcd.clear();
          return;
        }
      }
      else {
      input_password += key; // append new character to input password string
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(input_password);
      keyPadInput();
      }
    }
    else{
      keyPadInput();
    }
}
void keyPadLogin() {
  lcd.setCursor(0,0);
  lcd.print("Enter Password");
  char key = customKeypad.getKey();
  if(key){
    if(key == '*') {
      input_password = ""; // reset the input password
      keyPadLogin();
    } 
    else if(key == '#') {
      if(input_password == password_1) {
          if (!logOne) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WELCOME USER ONE");
            delay(3000);
            if (myRTC.hours == 8 && myRTC.minutes <= 15) {
              digitalWrite(ledPin,HIGH);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER ONE");
              lcd.setCursor(0,1);
              lcd.print("ON TIME!");
              userOneOnTime++;
              logOne = true;
              delay(5000);
              digitalWrite(ledPin,LOW);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 15 && myRTC.minutes <= 30) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER ONE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE LATE!");
              userOneLate++;
              logOne = true;
              delay(5000);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 30 || myRTC.hours > 8 && myRTC.hours <= 24 ) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER ONE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE ABSENT!");
              userOneAbsent++;
              logOne = true;
              delay(5000);
            }
            else {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("PLEASE COME BACK");
              lcd.setCursor(0,1);
              lcd.print("AT WORK TIME!");
              delay(5000);
            }
            lcd.clear();
            return;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("YOU HAVE ALREADY");
            lcd.setCursor(0,1);
            lcd.print("LOGGED IN TODAY!");
            delay(5000);
            return;
          }
        } 
        else if (input_password == password_2) {
          if (!logTwo) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WELCOME USER TWO");
            delay(3000);
            if (myRTC.hours == 8 && myRTC.minutes <= 15) {
              digitalWrite(ledPin,HIGH);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER TWO");
              lcd.setCursor(0,1);
              lcd.print("ON TIME!");
              userTwoOnTime++;
              logTwo = true;
              delay(5000);
              digitalWrite(ledPin,LOW);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 15 && myRTC.minutes <= 30) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER TWO");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE LATE!");
              userTwoLate++;
              logTwo = true;
              delay(5000);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 30 || myRTC.hours > 8 && myRTC.hours <= 24 ) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER TWO");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE ABSENT!");
              userTwoAbsent++;
              logTwo = true;
              delay(5000);
            }
            else {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("PLEASE COME BACK");
              lcd.setCursor(0,1);
              lcd.print("AT WORK TIME!");
              delay(5000);
            }
            lcd.clear();
            return;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("YOU HAVE ALREADY");
            lcd.setCursor(0,1);
            lcd.print("LOGGED IN TODAY!");
            delay(5000);
            return;
          }
        }
        else if (input_password == password_3) {
          if (!logThree) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WELCOME USER");
            lcd.setCursor(1,1);
            lcd.print("THREE");
            delay(3000);
            if (myRTC.hours == 8 && myRTC.minutes <= 15) {
              digitalWrite(ledPin,HIGH);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER THREE");
              lcd.setCursor(0,1);
              lcd.print("ON TIME!");
              userThreeOnTime++;
              logThree = true;
              delay(5000);
              digitalWrite(ledPin,LOW);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 15 && myRTC.minutes <= 30) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER THREE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE LATE!");
              userThreeLate++;
              logThree = true;
              delay(5000);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 30 || myRTC.hours > 8 && myRTC.hours <= 24 ) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER THREE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE ABSENT!");
              userThreeAbsent++;
              logThree = true;
              delay(5000);
            }
            else {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("PLEASE COME BACK");
              lcd.setCursor(0,1);
              lcd.print("AT WORK TIME!");
              delay(5000);
            }
            lcd.clear();
            return;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("YOU HAVE ALREADY");
            lcd.setCursor(0,1);
            lcd.print("LOGGED IN TODAY!");
            delay(5000);
            return;
          }
        }
        else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("INVALID PASSCODE!");
          delay(5000);
          input_password = "";
          lcd.clear();
          return;
        }
    } 
    else {
      input_password += key; // append new character to input password string
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(input_password);
      keyPadInput();
    }
  }
  else {
    keyPadLogin();
  }
}

void passWordVerify() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Please tap RFID");
  lcd.setCursor(0,1);
  lcd.print(countdownRFID);
  if (rfid.PICC_IsNewCardPresent()){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Scanning RFID...");
      delay(1000);
    if (rfid.PICC_ReadCardSerial()){
      if (rfid.uid.uidByte[0] == nuidPICC1[0] && rfid.uid.uidByte[1] == nuidPICC1[1] && rfid.uid.uidByte[2] == nuidPICC1[2] && rfid.uid.uidByte[3] == nuidPICC1[3] ) {
        if (!logOne) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WELCOME USER ONE");
            delay(3000);
            if (myRTC.hours == 8 && myRTC.minutes <= 15) {
              digitalWrite(ledPin,HIGH);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER ONE");
              lcd.setCursor(0,1);
              lcd.print("ON TIME!");
              userOneOnTime++;
              logOne = true;
              delay(5000);
              digitalWrite(ledPin,LOW);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 15 && myRTC.minutes <= 30) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER ONE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE LATE!");
              userOneLate++;
              logOne = true;
              delay(5000);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 30 || myRTC.hours > 8 && myRTC.hours <= 24 ) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER ONE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE ABSENT!");
              userOneAbsent++;
              logOne = true;
              delay(5000);
            }
            else {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("PLEASE COME BACK");
              lcd.setCursor(0,1);
              lcd.print("AT WORK TIME!");
              delay(5000);
            }
            lcd.clear();
            return;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("YOU HAVE ALREADY");
            lcd.setCursor(0,1);
            lcd.print("LOGGED IN TODAY!");
            delay(5000);
            return;
          }
      }
      else if (rfid.uid.uidByte[0] == nuidPICC2[0] && rfid.uid.uidByte[1] == nuidPICC2[1] && rfid.uid.uidByte[2] == nuidPICC2[2] && rfid.uid.uidByte[3] == nuidPICC2[3] ) {
        if (!logTwo) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WELCOME USER TWO");
            delay(3000);
            if (myRTC.hours == 8 && myRTC.minutes <= 15) {
              digitalWrite(ledPin,HIGH);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER TWO");
              lcd.setCursor(0,1);
              lcd.print("ON TIME!");
              userTwoOnTime++;
              logTwo = true;
              delay(5000);
              digitalWrite(ledPin,LOW);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 15 && myRTC.minutes <= 30) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER TWO");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE LATE!");
              userTwoLate++;
              logTwo = true;
              delay(5000);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 30 || myRTC.hours > 8 && myRTC.hours <= 24 ) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER TWO");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE ABSENT!");
              userTwoAbsent++;
              logTwo = true;
              delay(5000);
            }
            else {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("PLEASE COME BACK");
              lcd.setCursor(0,1);
              lcd.print("AT WORK TIME!");
              delay(5000);
            }
            lcd.clear();
            return;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("YOU HAVE ALREADY");
            lcd.setCursor(0,1);
            lcd.print("LOGGED IN TODAY!");
            delay(5000);
            return;
          }
      }
      else if (rfid.uid.uidByte[0] == nuidPICC3[0] && rfid.uid.uidByte[1] == nuidPICC3[1] && rfid.uid.uidByte[2] == nuidPICC3[2] && rfid.uid.uidByte[3] == nuidPICC3[3] ){
        if (!logThree) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WELCOME USER");
            lcd.setCursor(1,1);
            lcd.print("THREE");
            delay(3000);
            if (myRTC.hours == 8 && myRTC.minutes <= 15) {
              digitalWrite(ledPin,HIGH);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER THREE");
              lcd.setCursor(0,1);
              lcd.print("ON TIME!");
              userThreeOnTime++;
              logThree = true;
              delay(5000);
              digitalWrite(ledPin,LOW);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 15 && myRTC.minutes <= 30) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER THREE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE LATE!");
              userThreeLate++;
              logThree = true;
              delay(5000);
            }
            else if (myRTC.hours == 8 && myRTC.minutes > 30 || myRTC.hours > 8 && myRTC.hours <= 24 ) {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("USER THREE");
              lcd.setCursor(0,1);
              lcd.print("YOU ARE ABSENT!");
              userThreeAbsent++;
              logThree = true;
              delay(5000);
            }
            else {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("PLEASE COME BACK");
              lcd.setCursor(0,1);
              lcd.print("AT WORK TIME!");
              delay(5000);
            }
            lcd.clear();
            return;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("YOU HAVE ALREADY");
            lcd.setCursor(0,1);
            lcd.print("LOGGED IN TODAY!");
            delay(5000);
            return;
          }
      }
      else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("INVALID CARD!");
        Serial.println("INVALID CARD");
        delay(5000);
      }
    }
  }
  else if (countdownRFID == 0) {
    countdownRFID = 10;
    lcd.clear();
    keyPadLogin();
  }
  else {
    delay(1000);
    countdownRFID--;
    passWordVerify();
  }
   // Halt PICC
   rfid.PICC_HaltA();

   // Stop encryption on PCD
   rfid.PCD_StopCrypto1();  
   lcd.clear();
}

void ultrasonicSense() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  if (distance > 100) {
    lcd.clear();
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("Move closer");
    lcd.setCursor(0,1);   //Set cursor to character 2 on line 0
    lcd.print("To the sensor");
    delay(2000);
    lcd.clear();
    return;
  }
  else {
    lcd.clear();
    passWordVerify();
  }
}