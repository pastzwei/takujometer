/*  wattmeter_SD_withINA226 by K.Sakurai 20180922 ver.1.0
 *  あちゃんでいいの使用デバイスでINA226から電流・電圧をとってSDカードに記録するプログラムです。
 *  VCC=3.6V, f=8mHz
 *  使用ライブラリは Wire(I2C), SD, Adafruit_GFX, Adafruit_SSD1306 ina226
 */
 
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <Adafruit_SSD1306.h>

#include <INA226.h>

#include <Wire.h>

#include <SD.h>

//=====【Hardware Configration】ハードウェアに応じて以下を変更すること
const int chipSelect = 10;    //SDピンアサイン CSは10
const int analogPin = 0;    //アナログピンの番号 A0つかいます

//=====【Software Configration】測定の仕様を変えたければ以下を変更すること
const int NUMDET = 100;   //1つのデータにつき測定回数は100回
const int INTERVAL = 30;  //測定間隔は30ms
const int WAIT_TIME = 5000; //データ取得間隔は5000ms 処理時間は実測200ms程度なので、300ms以下NG

//=====

INA226 ina;

Adafruit_SSD1306 display(-1);

int number;

void setup()
{                
  pinMode(LED_BUILTIN, OUTPUT);

  //SDカードスロット初期化
  if (!SD.begin(chipSelect)) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(5000);
    // don't do anything more:
    return;
  }

  // ina226のsetup Default INA226 address is 0x40
  ina.begin();

  // Configure INA226
  ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);

  // Calibrate INA226. Rshunt = 0.01 ohm, Max excepted current = 4A
  ina.calibrate(0.01, 4);

  //OLED初期化
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // aitendoで売ってるOLEDデバイスの I2Cアドレスは0x3C
  display.clearDisplay();

  //データの通し番号初期化
  number = 0;
}

void loop()
{
  int i;
  double current;
  double voltage;
  unsigned long time_zero;

  //開始時刻を格納

  time_zero = millis();

  //ina226から測定値を受け取る
  voltage = 0;
  current = 0;

  for(i = 0; i < NUMDET; i++) {
    voltage += abs(ina.readBusVoltage());
    current += abs(ina.readShuntCurrent());
    delay(1);
  }

  voltage = voltage / NUMDET;
  current = current / NUMDET;
  
  //測定値をOLEDに出力
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  display.print(voltage, 5);
  display.println(" V");
  display.print(current, 5);
  display.println(" A");
  display.display();

  //SDに書き込む文字列つくる
  String dataString = "";
  dataString += String(number);
  dataString += ",";
  dataString += String(voltage);
  dataString += ",";
  dataString += String(current);
  
  //SDカードに書き込む。ダメならLEDが点灯する。
  File dataFile = SD.open("measure.csv", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else { 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  } 

  //全部終わったら1回チカっとして通し番号をインクリメント
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  number++;

  //WAIT_TIMEまで待機
  while(millis() < time_zero + WAIT_TIME)
  {
    ;
  }
}
