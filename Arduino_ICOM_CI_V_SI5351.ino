// full description of CI-V command? read this - http://www.plicht.de/ekki/civ/civ-p4.html

// адрес контроллера "по умолчанию"
#define C_ADDR 0xE0
// адрес трансивера "по умолчанию", как у IC-706
#define TRX_ADDR 0x48

#include <SPI.h>
#include <Wire.h>
#include "si5351.h"
Si5351 si5351;

// массив под получаемые команды
byte buffer[32];
// счетчик полученных байт
byte rcv_byte = 0;
// текущая частота работы
unsigned long current_freq = 7000000;
unsigned long old_freq = 0;

// временные переменные
String St;
byte r, z;
unsigned long fr, mn;


void setup() {
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  
  bool i2c_found;
  i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  si5351.set_freq(current_freq, SI5351_CLK2);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);    
}

void loop() {
  if (current_freq != old_freq) {
    si5351.set_freq(current_freq, SI5351_CLK0);
    old_freq = current_freq;
  }

  if (Serial.available() > 0) {
    delay(10);
    rcv_byte = 0;
    St = "";
    while (Serial.available() > 0) {
      buffer[rcv_byte] = Serial.read();
      // дублируем запрос в линию передачи данных (без этого OmniRig не работает)
//      Serial.write(buffer[rcv_byte]);
      if (rcv_byte > 3) {
        St = St + String(buffer[rcv_byte],HEX) + " ";
      }
      if (buffer[rcv_byte] == 0xFD) {
        break;
      }
      rcv_byte += 1;
    }
    if ( (buffer[0] == 0xFE) && (buffer[rcv_byte] == 0xFD) ) {
      for (int k=0; k<=rcv_byte; k++) {
        Serial.write(buffer[k]);
      }
// запрос на установку текущей частоты 
// Write operating freq. data to VFO.
      if (buffer[4] == 0x00) {
        // -----------------------------------
        fr = 0;
        mn = 1000000000;
        for (int i=0; i<5; i++) {
          fr = fr + buffer[9-i]/16 * mn;
          mn = mn / 10;
          fr = fr + buffer[9-i]%16 * mn;
          mn = mn / 10;
        }    
        current_freq = fr;
        Serial.write(0xFE);
        Serial.write(0xFE);
        Serial.write(C_ADDR);
        Serial.write(TRX_ADDR);
        Serial.write(0x06);
        Serial.write(0x01);
        Serial.write(0xFD);
      }

// запрос на получение текущей частоты
// Read operating freq. data
      if (buffer[4] == 0x03) {
        // -----------------------------------
        Serial.write(0xFE);
        Serial.write(0xFE);
        Serial.write(C_ADDR);
        Serial.write(TRX_ADDR);
        Serial.write(0x03);
        fr = current_freq;
        for (int i=0; i<5; i++) {
          z = (fr % 10) ;
          fr = fr / 10;
          z = z + (fr % 10)*16 ;
          fr = fr / 10;
          Serial.write(z);
        }  
        Serial.write(0xFD);
      }

// запрос текущего режима работы
// Read operating mode data
      if (buffer[4] == 0x04) {
        // -----------------------------------
        Serial.write(0xFE);
        Serial.write(0xFE);
        Serial.write(C_ADDR);
        Serial.write(TRX_ADDR);
        Serial.write(0x04);
        Serial.write(0x01);
        Serial.write(0x01);
        Serial.write(0xFD);
      }

// запрос на установку текущей частоты 
// Write operating freq. data to VFO.
      if (buffer[4] == 0x05) {
        // -----------------------------------
        fr = 0;
        mn = 1000000000;
        for (int i=0; i<5; i++) {
          fr = fr + buffer[9-i]/16 * mn;
          mn = mn / 10;
          fr = fr + buffer[9-i]%16 * mn;
          mn = mn / 10;
        }    
        current_freq = fr;
        Serial.write(0xFE);
        Serial.write(0xFE);
        Serial.write(C_ADDR);
        Serial.write(TRX_ADDR);
        Serial.write(0x06);
        Serial.write(0x01);
        Serial.write(0xFD);
      }

// запрос на установку текущего режима работы
// Write operating mode data to VFO.
      if (buffer[4] == 0x06) {
        // -----------------------------------
        Serial.write(0xFE);
        Serial.write(0xFE);
        Serial.write(C_ADDR);
        Serial.write(TRX_ADDR);
        Serial.write(0x06);
        Serial.write(0x01);
        Serial.write(0xFD);
      }

// запрос 
// Set AF, RF, Sql gain
      if (buffer[4] == 0x14) {
        // -----------------------------------
        Serial.write(0xFE);
        Serial.write(0xFE);
        Serial.write(C_ADDR);
        Serial.write(TRX_ADDR);
        Serial.write(0xFA);
        Serial.write(0xFD);
      }
    }
    delay(200);
  }
  
}
