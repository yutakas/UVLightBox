#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "EEPROMAnything.h"

LiquidCrystal_I2C lcd(0x27,16,2);

#define SETMODE_HOURS          1
#define SETMODE_MINUTES        2
#define SETMODE_10SEC          3
#define SETMODE_LIGHTSTRENGTH  4

#define LIGHTSTRENGTH_FULL  0
#define LIGHTSTRENGTH_HALF  1


#define EEPROM_SIZE  32

const int relay1Pin = 32;
const int relay2Pin = 33;

const int buttonPinSetTimeUp = 25; 
const int buttonPinSetTimeDown = 26; 
const int buttonPinSetModeChange = 34; 
const int buttonPinStratStop = 35; 

int buttonSetTimeUpState = LOW;     
int buttonSetTimeUpPreState = LOW;

int buttonSetTimeDownState = LOW;
int buttonSetTimeDownPreState = LOW;

int buttonSetModeChangeState = LOW;
int buttonSetModeChangePreState = LOW;

int buttonStartStopState = LOW;
int buttonStartStopPreState = LOW;

int currentSetMode = SETMODE_10SEC;

int32_t timerSetTime = 0;
int32_t timerStartTime;
int32_t timerEndTime;
int32_t timerRemaining = 0;
int32_t lightStrength = LIGHTSTRENGTH_FULL;

bool timerRunning = false;

void lcd_test() {
  lcd.clear();

  // lcd.setCursor(0, 0);
  // uint32_t currentTime = millis();
  // String strTick = String(currentTime);
  // lcd.print(strTick);
 
  if (timerRunning) {
    lcd.setCursor(0, 0);      
    lcd.print("Timer Running");
  } else {
    lcd.setCursor(0, 0);      
    String sSetMode = String("");
    switch (currentSetMode) {
      case SETMODE_10SEC:
        sSetMode = String("Off Mode:10Sec");
        break;
      case SETMODE_MINUTES:
        sSetMode = String("Off Mode:Min");
        break;
      case SETMODE_HOURS:
        sSetMode = String("Off Mode:Hour");
        break;
      case SETMODE_LIGHTSTRENGTH:
        sSetMode = String("Off Mode:Strength");
        break;        
    }
    lcd.print(sSetMode);
  }

  if (lightStrength == LIGHTSTRENGTH_FULL) {
    lcd.setCursor(0, 1);      
    lcd.print("FULL");
  } else if (lightStrength == LIGHTSTRENGTH_HALF) {
    lcd.setCursor(0, 1);      
    lcd.print("HALF");
  }

  char str[128];
  if (timerRemaining > 0) {
    snprintf(str, 128, "%02d:%02d:%02d", timerRemaining / 3600, (timerRemaining %3600) / 60, timerRemaining % 60);
  } else {
    snprintf(str, 128, "%02d:%02d:%02d", timerSetTime / 3600, (timerSetTime % 3600) / 60, timerSetTime % 60);
  }
  lcd.setCursor(5, 1);      
  lcd.print(str);

}

void i2c_scanner() {
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 
  delay(5000);           // wait 5 seconds for next scan
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(buttonPinSetTimeUp, INPUT);
  pinMode(buttonPinSetTimeDown, INPUT);
  pinMode(buttonPinSetModeChange, INPUT);
  pinMode(buttonPinStratStop, INPUT);

  lcd.begin(16,2);
  lcd.init();
  
  // Turn on the backlight.
  lcd.backlight();
  
  EEPROM.begin(EEPROM_SIZE);
  EEPROM_readAnything(0, timerSetTime);
  if (timerSetTime < 0) {
    timerSetTime = 0;
  }
  EEPROM_readAnything(16, lightStrength);
  if (lightStrength < 0) {
    lightStrength = LIGHTSTRENGTH_FULL;
  }
  lcd_test();
}

void updateEeprom() {
  EEPROM_writeAnything(0, timerSetTime);
  EEPROM_writeAnything(16, lightStrength);
  EEPROM.commit();
}

void setRelay() {
  digitalWrite(relay1Pin, HIGH);
  if (lightStrength == LIGHTSTRENGTH_FULL ) {
    digitalWrite(relay2Pin, HIGH);
  }
}

void killRelay() {
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
}

void __loop() {
  lcd_test();
  delay(1000);
}

void loop() {

  // i2c_scanner();
  bool updateLcd = false;

  if (!timerRunning) {
    buttonSetTimeUpState = digitalRead(buttonPinSetTimeUp);
    if (buttonSetTimeUpState == HIGH) {
      if (buttonSetTimeUpPreState != buttonSetTimeUpState) {
        Serial.println("buttonSetTimeUpState High");
        switch (currentSetMode) {
          case SETMODE_10SEC:
            timerSetTime += 10;
            break;
          case SETMODE_MINUTES:
            timerSetTime += 60;
            break;
          case SETMODE_HOURS:
            timerSetTime += 3600;
            break;
          case SETMODE_LIGHTSTRENGTH:
            lightStrength = lightStrength == LIGHTSTRENGTH_FULL ? LIGHTSTRENGTH_HALF : LIGHTSTRENGTH_FULL;
            break;            
        }
        if (timerSetTime >= 3600 * 5) {
          timerSetTime = 3600 * 5;
        }
        timerRemaining = timerSetTime;
        updateLcd = true;
      }
    }
    buttonSetTimeUpPreState = buttonSetTimeUpState;

    buttonSetTimeDownState = digitalRead(buttonPinSetTimeDown);
    if (buttonSetTimeDownState == HIGH) {
      if (buttonSetTimeDownPreState != buttonSetTimeDownState) {
        Serial.println("buttonSetTimeDownState High");
        switch (currentSetMode) {
          case SETMODE_10SEC:
            timerSetTime -= 10;
            break;
          case SETMODE_MINUTES:
            timerSetTime -= 60;
            break;
          case SETMODE_HOURS:
            timerSetTime -= 3600;
            break;
          case SETMODE_LIGHTSTRENGTH:
            lightStrength = lightStrength == LIGHTSTRENGTH_FULL ? LIGHTSTRENGTH_HALF : LIGHTSTRENGTH_FULL;
            break;            
        }
        if (timerSetTime < 0) {
          timerSetTime = 0;
        }
        timerRemaining = timerSetTime;
        updateLcd = true;
      }
    }
    buttonSetTimeDownPreState = buttonSetTimeDownState;

    // reset time to zero if bothe up and down button are pushed.
    if (buttonSetTimeDownState == HIGH && buttonSetTimeUpState == HIGH) {
        timerSetTime = 0;
        timerRemaining = timerSetTime;
        updateEeprom();
        updateLcd = true;
    }

    buttonSetModeChangeState = digitalRead(buttonPinSetModeChange);
    if (buttonSetModeChangeState == HIGH) {
      if (buttonSetModeChangePreState != buttonSetModeChangeState) {
        Serial.println("buttonSetTimeDownState High");
        switch (currentSetMode) {
          case SETMODE_10SEC:
            currentSetMode = SETMODE_MINUTES;
            break;
          case SETMODE_MINUTES:
            currentSetMode = SETMODE_HOURS;
            break;
          case SETMODE_HOURS:
            currentSetMode = SETMODE_LIGHTSTRENGTH;
            break;
          case SETMODE_LIGHTSTRENGTH:
            currentSetMode = SETMODE_10SEC;
            break;            
        }
        updateLcd = true;
      }
    }
    buttonSetModeChangePreState = buttonSetModeChangeState;
  }

  buttonStartStopState = digitalRead(buttonPinStratStop);
  if (buttonStartStopState == HIGH) {
    if (buttonStartStopPreState != buttonStartStopState) {
      Serial.println("buttonStartStopState High");
      timerRunning = !timerRunning;
      if (timerRunning) {
        timerStartTime = millis();
        timerEndTime = timerStartTime + timerRemaining * 1000;
        setRelay();
        updateEeprom();
      } else {
        killRelay();
      }
      updateLcd = true;
    }
  }
  buttonStartStopPreState = buttonStartStopState;

  if (timerRunning) {
    uint32_t preTimerRemaining = timerRemaining;
    uint32_t currentTime = millis();
    timerRemaining = (timerEndTime - currentTime) / 1000;
    if (timerRemaining <= 0) {
      timerRunning = false;
      killRelay();
      timerRemaining = timerSetTime;
    }
    if (preTimerRemaining != timerRemaining) {
      updateLcd = true;
    }
  }

  if (updateLcd) {
      lcd_test();
      delay(100);
  }
  // lcd_test();
  delay(10);
}
