/*  voltmeter_takujo by K.Sakurai 20180924
 *  あちゃんでいいの3.3V8MHz使用。INA226で測定した電流・電圧・電力をSDに記録するプログラムです。
 *  使用ライブラリは Wire(I2C), SD, INA226
 */

#include <Wire.h>

#include <SD.h>

//=====【Hardware Configration】ハードウェアに応じて以下を変更すること
const int chipSelect = 10;    //SDピンアサイン CSは10
const int MEASURE_1ST = 0;    //A0ピン@10mA
const int MEASURE_2ND = 1;    //A1ピン@100mA
const int MEASURE_3RD = 2;    //A2ピン@1A
const int MEASURE_4th = 3;    //A3ピン@10A
const int CLIPPING = 2;       //D2ピン@測定限界

//=====【Software Configration】測定の仕様を変えたければ以下を変更すること
const int NUMDET = 100;   //1つのデータにつき測定回数は100回
const int INTERVAL = 30;  //測定インターバルは30ms
const int WAIT_TIME = 5000; //データ取得間隔は5000ms 処理時間は実測200ms程度なので、300ms以下NG

//=====

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

  //データの通し番号初期化
  number = 0;
}

void loop()
{
  int i;
  double val;
  double voltage;
  unsigned long time_zero;

  //開始時刻を格納・valの初期化

  time_zero = millis();
  val = 0;

  //A0から取得して電圧を求めるのを規定回数繰り返す valの中身は回数分の測定値の総和になる
  for(i = 0; i < NUMDET; i++)
  {
    voltage = (double) analogRead(analogPin) * VCC / 1024 * (MULTI_HIGH + MULTI_LOW) / MULTI_LOW * 2;
    val += voltage;
    delay(INTERVAL);
  }

  //平均値を導出 valの中身は平均値になる
  val = val / NUMDET;  
  
  //SDに書き込む文字列つくる
  String dataString = "";
  dataString += String(number);
  dataString += ",";
  dataString += String(val);
  
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