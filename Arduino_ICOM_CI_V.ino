// full description of CI-V command? read this - http://www.plicht.de/ekki/civ/civ-p4.html

// адрес контроллера "по умолчанию"
#define C_ADDR 0xE0
// адрес трансивера "по умолчанию", как у IC-706
#define TRX_ADDR 0x48

// Описываем подключение модуля AD9850
#define W_CLK 5
#define FQ_UD 4
#define DATA  3 
#define RESET 2
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }


// массив под получаемые команды
byte buffer[32];
// счетчик полученных байт
byte rcv_byte = 0;
// текущая частота работы
unsigned long current_freq = 12345678;
unsigned long old_freq = 0;

// временные переменные
String St;
byte r, z;
unsigned long fr, mn;


void setup() {
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  
  sendFrequency(current_freq);

}

void loop() {

  if (Serial.available() > 0) {
    delay(20);
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
        sendFrequency(current_freq);
        
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
        sendFrequency(current_freq);
        
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


// Передаем байт побитно, начиная с младшего бита, в AD9850, по последовательному интерфейсу 
// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}

// расчитываем коэффициент деления для формирования AD9850 нужной нам частоты
// передаем нужные коэффициенты на AD9850
// frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {
  if (current_freq != old_freq) {
    int32_t freq = frequency * 4294967295/125000000;  // note 125 MHz clock on 9850
    for (int b=0; b<4; b++, freq>>=8) {
      tfr_byte(freq & 0xFF);
    }
    tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
    pulseHigh(FQ_UD);  // Done!  Should see output
    old_freq = current_freq;
  }
}
