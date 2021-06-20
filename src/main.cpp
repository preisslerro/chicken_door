//#############################################################################
// Program for open a chicken cage door time based at sunrise/sunset times.
//-----------------------------------------------------------------------------
// ...
//#############################################################################

#include <Arduino.h>
#include "RTClib.h"

#define DEBUG
#define doorOpenSwitch 6
#define relay1 7
#define relay2 8
#define buzzer 9
#define timeset 10
#define timeoffset 1800   // Offset +- Sekunden für Öffnungszeiten

const uint8_t dt[12][31][4] = 
// Januar
{{{8,3,16,25},{8,3,16,26},{8,3,16,27},{8,3,16,28},{8,2,16,30},{8,2,16,31},{8,2,16,32},
{8,2,16,33},{8,1,16,34},{8,1,16,36},{8,0,16,37},{8,0,16,38},{7,59,16,39},{7,59,16,41},
{7,58,16,42},{7,57,16,44},{7,56,16,45},{7,56,16,47},{7,55,16,48},{7,54,16,50},{7,53,16,51},
{7,52,16,53},{7,51,16,54},{7,50,16,56},{7,49,16,57},{7,48,16,59},{7,47,17,0},{7,45,17,2},
{7,44,17,4},{7,43,17,5},{7,41,17,7}},
// Februar
{{7,40,17,9},{7,39,17,10},{7,37,17,12},{7,36,17,13},{7,34,17,15},{7,33,17,17},{7,31,17,18},
{7,30,17,20},{7,28,17,22},{7,27,17,23},{7,25,17,25},{7,23,17,27},{7,22,17,28},{7,20,17,30},
{7,18,17,32},{7,17,17,33},{7,15,17,35},{7,13,17,37},{7,11,17,38},{7,9,17,40},{7,8,17,41},
{7,6,17,43},{7,4,17,45},{7,2,17,46},{7,0,17,48},{6,58,17,50},{6,56,17,51},{6,54,17,53},
{6,52,17,54},{0,0,0,0},{0,0,0,0}},
// März
{{6,50,17,56},{6,48,17,58},{6,46,17,59},{6,44,18,1},{6,42,18,2},{6,40,18,4},{6,38,18,5},
{6,36,18,7},{6,34,18,9},{6,32,18,10},{6,30,18,12},{6,28,18,13},{6,26,18,15},{6,24,18,16},
{6,21,18,18},{6,19,18,19},{6,17,18,21},{6,15,18,22},{6,13,18,24},{6,11,18,25},{6,9,18,27},
{6,7,18,28},{6,5,18,30},{6,2,18,32},{6,0,18,33},{5,58,18,35},{5,56,18,36},{5,54,18,38},
{5,52,18,39},{5,50,18,41},{5,48,18,42}},
// April
{{5,46,18,44},{5,43,18,45},{5,41,18,47},{5,39,18,48},{5,37,18,50},{5,35,18,51},{5,33,18,53},
{5,31,18,54},{5,29,18,56},{5,27,18,57},{5,25,18,59},{5,23,19,0},{5,21,19,2},{5,19,19,3},
{5,17,19,5},{5,15,19,6},{5,13,19,8},{5,11,19,9},{5,9,19,11},{5,7,19,12},{5,5,19,14},
{5,3,19,15},{5,1,19,17},{5,0,19,18},{4,58,19,20},{4,56,19,21},{4,54,19,23},{4,52,19,24},
{4,51,19,26},{4,49,19,27},{0,0,0,0}},
// Mai
{{4,47,19,29},{4,46,19,30},{4,44,19,32},{4,42,19,33},{4,41,19,34},{4,39,19,36},{4,37,19,37},
{4,36,19,39},{4,34,19,40},{4,33,19,42},{4,31,19,43},{4,30,19,44},{4,29,19,46},{4,27,19,47},
{4,26,19,48},{4,25,19,50},{4,23,19,51},{4,22,19,52},{4,21,19,54},{4,20,19,55},{4,19,19,56},
{4,18,19,58},{4,16,19,59},{4,15,20,0},{4,15,20,1},{4,14,20,2},{4,13,20,3},{4,12,20,5},
{4,11,20,6},{4,10,20,7},{4,10,20,8}},
// Juni
{{4,9,20,9},{4,8,20,10},{4,8,20,11},{4,7,20,11},{4,7,20,12},{4,6,20,13},{4,6,20,14},
{4,5,20,15},{4,5,20,15},{4,5,20,16},{4,4,20,17},{4,4,20,17},{4,4,20,18},{4,4,20,18},
{4,4,20,19},{4,4,20,19},{4,4,20,20},{4,4,20,20},{4,4,20,20},{4,4,20,21},{4,5,20,21},
{4,5,20,21},{4,5,20,21},{4,6,20,21},{4,6,20,21},{4,6,20,21},{4,7,20,21},{4,7,20,21},
{4,8,20,21},{4,9,20,21},{0,0,0,0}},
// Juli
{{4,9,20,20},{4,10,20,20},{4,11,20,20},{4,11,20,19},{4,12,20,19},{4,13,20,19},{4,14,20,18},
{4,15,20,17},{4,16,20,17},{4,16,20,16},{4,17,20,15},{4,18,20,15},{4,19,20,14},{4,20,20,13},
{4,22,20,12},{4,23,20,11},{4,24,20,10},{4,25,20,9},{4,26,20,8},{4,27,20,7},{4,28,20,6},
{4,30,20,5},{4,31,20,4},{4,32,20,3},{4,33,20,1},{4,35,20,0},{4,36,19,59},{4,37,19,57},
{4,39,19,56},{4,40,19,55},{4,41,19,53}},
// August
{{4,43,19,52},{4,44,19,50},{4,45,19,49},{4,47,19,47},{4,48,19,45},{4,50,19,44},{4,51,19,42},
{4,52,19,40},{4,54,19,39},{4,55,19,37},{4,57,19,35},{4,58,19,34},{4,59,19,32},{5,1,19,30},
{5,2,19,28},{5,4,19,26},{5,5,19,24},{5,7,19,23},{5,8,19,21},{5,9,19,19},{5,11,19,17},
{5,12,19,15},{5,14,19,13},{5,15,19,11},{5,17,19,9},{5,18,19,7},{5,19,19,5},{5,21,19,3},
{5,22,19,1},{5,24,18,59},{5,25,18,57}},
// September
{{5,27,18,55},{5,28,18,53},{5,29,18,51},{5,31,18,49},{5,32,18,47},{5,34,18,44},{5,35,18,42},
{5,37,18,40},{5,38,18,38},{5,39,18,36},{5,41,18,34},{5,42,18,32},{5,44,18,30},{5,45,18,27},
{5,47,18,25},{5,48,18,23},{5,49,18,21},{5,51,18,19},{5,52,18,17},{5,54,18,15},{5,55,18,12},
{5,57,18,10},{5,58,18,8},{5,59,18,6},{6,1,18,4},{6,2,18,2},{6,4,18,0},{6,5,17,57},
{6,7,17,55},{6,8,17,53},{0,0,0,0}},
// Oktober
{{6,10,17,51},{6,11,17,49},{6,13,17,47},{6,14,17,45},{6,16,17,43},{6,17,17,41},{6,18,17,39},
{6,20,17,37},{6,21,17,34},{6,23,17,32},{6,24,17,30},{6,26,17,28},{6,28,17,26},{6,29,17,24},
{6,31,17,22},{6,32,17,21},{6,34,17,19},{6,35,17,17},{6,37,17,15},{6,38,17,13},{6,40,17,11},
{6,41,17,9},{6,43,17,7},{6,45,17,5},{6,46,17,4},{6,48,17,2},{6,49,17,0},{6,51,16,58},
{6,52,16,57},{6,54,16,55},{6,56,16,53}},
// November
{{6,57,16,52},{6,59,16,50},{7,0,16,48},{7,2,16,47},{7,4,16,45},{7,5,16,44},{7,7,16,42},
{7,8,16,41},{7,10,16,39},{7,11,16,38},{7,13,16,37},{7,15,16,35},{7,16,16,34},{7,18,16,33},
{7,19,16,32},{7,21,16,31},{7,22,16,29},{7,24,16,28},{7,25,16,27},{7,27,16,26},{7,28,16,25},
{7,30,16,24},{7,31,16,23},{7,33,16,23},{7,34,16,22},{7,35,16,21},{7,37,16,20},{7,38,16,20},
{7,40,16,19},{7,41,16,18},{0,0,0,0}},
// Dezember
{{7,42,16,18},{7,43,16,17},{7,45,16,17},{7,46,16,17},{7,47,16,16},{7,48,16,16},{7,49,16,16},
{7,50,16,16},{7,51,16,15},{7,52,16,15},{7,53,16,15},{7,54,16,15},{7,55,16,15},{7,56,16,16},
{7,57,16,16},{7,57,16,16},{7,58,16,16},{7,59,16,17},{7,59,16,17},{8,0,16,17},{8,0,16,18},
{8,1,16,18},{8,1,16,19},{8,2,16,20},{8,2,16,20},{8,2,16,21},{8,2,16,22},{8,3,16,23},
{8,3,16,23},{8,3,16,24},{8,3,16,25}}};

String serCommand = ""; 

RTC_DS3231 rtc;
DateTime dtakt;
uint8_t tag;
uint8_t monat;
uint8_t stunde;
uint8_t minute;
unsigned long previousMillis = 0;
const unsigned long interval = 60000;    // Alle "interval" Millisekunden Schaltpukte prüfen
enum class DState {closed, open, undefined};
DState doorState = DState::undefined;

String getTimeStamp() {
  DateTime now = rtc.now();           // Get actual timestamp from RTC
  String timeStamp = "";              // Variable for assembled timestamp
  unsigned int tsvar = 0;             // Auxiliary variable

  timeStamp += now.year();            // Year
  
  timeStamp += '-';
  
  tsvar = now.month();                // Month
  if (tsvar < 10)
    timeStamp += '0';
  timeStamp += tsvar;
  
  timeStamp += '-';
  
  tsvar = now.day();                  // Day
  if (tsvar < 10) 
    timeStamp += '0';
  timeStamp += tsvar;  
  
  timeStamp += ' ';
  
  tsvar = now.hour();                 // Hour
  if (tsvar < 10)
    timeStamp += '0';
  timeStamp += tsvar;
  
  timeStamp += ':';
  
  tsvar = now.minute();               // Minute
  if (tsvar < 10)
    timeStamp += '0';
  timeStamp += tsvar;
  
  timeStamp += (':');
  
  tsvar = now.second();               // Second
  if (tsvar < 10)
    timeStamp += '0';
  timeStamp += tsvar;

  return timeStamp;
}

void parseCommand(String com)
{
  String data = "";                                 // Variable for parsed input
  unsigned short year;
  unsigned char month, day, hour, minute, second;
  unsigned char index, lastindex;                   // Auxiliary variable for indices

  if (com.equalsIgnoreCase("rtc"))                  // If command is "rtc"
  {
    Serial.println(F("Datum und Uhrzeit - in Normalzeit (Winterzeit) - eingeben (Muster: 2021-12-31 23:59:59): "));
    while (true)
    {
      if (Serial.available())
      {
        char c = Serial.read();

        if (c == '\n')
        {
          if (data == "")                     // If inputed comman was just the return-key ...
          {
            Serial.println(getTimeStamp());   // ... print actual date / time ...
            Serial.print('\n');
            break;                            // ... and leave parse-loop
          }

          index = data.indexOf('-');                          // Get index until delimeter
          year = data.substring(0, index).toInt();            // Extract day
          lastindex = index + 1;                              // Save last index without delimeter
          index = data.indexOf('-', lastindex);               // Get index until delimeter from last indexIndex on
          month = data.substring(lastindex, index).toInt();   // Extract month
          lastindex = index + 1;                              // Save last index without delimeter
          index = data.indexOf(' ', lastindex);               // Get index until delimeter from last indexIndex on
          day = data.substring(lastindex, index).toInt();     // Extract year
          lastindex = index + 1;                              // Save last index without delimeter
          index = data.indexOf(':', lastindex);               // Get index until delimeter from last indexIndex on
          hour = data.substring(lastindex, index).toInt();    // Extract hour
          lastindex = index + 1;                              // Save last index without delimeter
          index = data.indexOf(':', lastindex);               // Get index until delimeter from last indexIndex on
          minute = data.substring(lastindex, index).toInt();  // Extract minute
          lastindex = index + 1;                              // Save last index without delimeter
          index = data.indexOf('\n', lastindex);              // Get index until delimeter from last indexIndex on
          second = data.substring(lastindex, index).toInt();  // Extract second

          rtc.adjust(DateTime(year, month, day, hour, minute, second));   // Adjust RTC
          Serial.print("\"");
          Serial.print(getTimeStamp());
          Serial.println(F("\" wurde in RTC-Uhr gepeichert!"));
          Serial.print('\n');
  
          data = "";
          break;
        }
        else
        {
          data += c;
        }
      }
    }
  }
  else if (com.equalsIgnoreCase("info")) {
    Serial.print(F("Datum / Zeit: "));
    Serial.println(getTimeStamp());
    Serial.println(F("Kommando eingeben (info / rtc):\n"));
  }
  else {
    Serial.println(F("Wrong command!"));
    Serial.println();
    Serial.println(F("Kommando eingeben (info / rtc):\n"));
  }
}

void signal() {
  #ifdef DEBUG
    Serial.println("Signal ausgelöst.");
  #endif

  uint8_t count = 5;
  while (count > 0) {
    count--;
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer,LOW);
    delay(500);
  }  
}

void doorOpen() {
  if (doorState == DState::undefined || doorState == DState::closed) {
    #ifdef DEBUG
      Serial.println("Türe öffnen ...");
    #endif
    signal();
    delay(5000);
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
    delay(90000);
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    doorState = DState::open;
    #ifdef DEBUG
      Serial.println("Türe geöffnet.");
    #endif
  }
}

void doorClose() {
  if (doorState == DState::undefined || doorState == DState::open) {
    #ifdef DEBUG
      Serial.println("Türe schliessen...");
    #endif
    signal();
    delay(5000);
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, LOW);
    delay(90000);
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    doorState = DState::closed;
    #ifdef DEBUG
      Serial.println("Türe geschlossen.");
    #endif
  }
}

void checkStatus() {
  dtakt = (RTC_DS3231::now() + TimeSpan(-timeoffset));
  #ifdef DEBUG
  Serial.print(dtakt.hour());
  Serial.print(":");
  Serial.println(dtakt.minute());
  #endif
  monat = dtakt.month() - 1;  // Minus 1 wegen Array
  tag = dtakt.day() - 1;      // Minus 1 wegen Array
  stunde = dtakt.hour();
  minute = dtakt.minute();

  uint8_t sr_h = dt[monat][tag][0];   // Sonnenaufgangs Stunde
  uint8_t sr_m = dt[monat][tag][1];   // Sonnenaufgangs Minute
  uint8_t ss_h = dt[monat][tag][2];   // Sonnenuntergangs Stunde
  uint8_t ss_m = dt[monat][tag][3];   // Sonnenuntergangs Minute

  if (sr_h > stunde) {
    doorClose();
  } else if (sr_h == stunde) {
    if(sr_m > minute) {
      doorClose();
    } else if (sr_m <= minute) {
      doorOpen();
    }
  } else {
    if (ss_h > stunde) {
      doorOpen();
    } else if (ss_h == stunde) {
      if (ss_m > minute) {
        doorOpen();
      } else if (ss_m <= minute) {
        doorClose();
      }
    } else {
      doorClose();
    }
  }

}

void setup() {
  Serial.begin(115200);
  pinMode(doorOpenSwitch, INPUT_PULLUP);
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  pinMode(timeset, INPUT_PULLUP);
  
  if (! rtc.begin()) {
    Serial.println(F("Kann RTC Modul nicht finden!"));
    Serial.flush();
    abort();
  }

  if (rtc.lostPower() || digitalRead(timeset) == LOW) {
    Serial.println(F("Kommando eingeben (info / rtc):\n"));
    while (true) {
      if (Serial.available()) {
        serCommand = Serial.readStringUntil('\n');
        parseCommand(serCommand);
        Serial.readStringUntil('\n');
        delay(500);
      }
    }
  }

  #ifdef DEBUG
    Serial.println("Aktiviert: Überprüfe Status ...");
  #endif
  checkStatus();
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (digitalRead(doorOpenSwitch) == LOW) {
    delay(50);
    while (digitalRead(doorOpenSwitch) == LOW) {
      doorOpen();
    }
  }
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    checkStatus();
  }
}