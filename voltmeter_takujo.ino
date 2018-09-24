/*  voltmeter_takujo by K.Sakurai 20180924
 *  あちゃんでいいの3.3V8MHz使用。INA226で測定した電流・電圧・電力をSDに記録するプログラムです。
 *  使用ライブラリは Wire(I2C), SD, INA226
 *  【重要】OLEDやめました。ライブラリ重すぎてATMEGA328のメモリを圧迫しすぎるんです。
 *  かわりにLEDを4灯つけて、電圧によって明るさが変わるようにします。
 */
 
#include <INA226.h>

#include <Wire.h>
#include <SD.h>

//=====【Hardware Configration】ハードウェアに応じて以下を変更すること
const int chipSelect = 10;    //SDピンアサイン CSは10
const int MEASURE_1ST = 3;    //D3ピン@10mV
const int MEASURE_2ND = 5;    //D5ピン@100mV
const int MEASURE_3RD = 6;    //D6ピン@1V
const int CLIPPING = 4;       //D4ピン@測定限界

//=====【Software Configration】測定の仕様を変えたければ以下を変更すること
const int ABSOLUTE_MAX = 1.5;  //定格電流1.5A
const int NUMDET = 100;   //1つのデータにつき測定回数は100回
const int INTERVAL = 30;  //測定インターバルは30ms
const int WAIT_TIME = 5000; //データ取得間隔は5000ms 処理時間は実測200ms程度なので、300ms以下NG

//=====

INA226 ina;

int cnt;

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

  //INA226のセットアップ
  ina.begin();
  ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
  ina.calibrate(0.002, 1.5);  //シャント抵抗は0.002Ω、定格は1.5Aとします

  //データの通し番号初期化
  cnt = 0;
}

void loop()
{
  int i;
  double current;
  double voltage;
  double watt;
  unsigned long time_zero;

  //V,Aリセット＋開始時刻を格納
  voltage = 0;
  current = 0;
  watt = 0;
  time_zero = millis();

  //A0から取得して電圧を求めるのを規定回数繰り返す valの中身は回数分の測定値の総和になる
  for(i = 0; i < NUMDET; i++)
  {
    voltage += ina.readBusVoltage();
    current += ina.readShuntCurrent();
    watt += ina.readBusPower();
    delay(INTERVAL);
  }
  //平均値を導出 valの中身は平均値になる
  voltage = voltage / 100;
  current = current / 100;
  watt = watt / 100;

  //LEDによる表示を行う
  

  //SDに書き込む文字列つくる
  String dataString = "";
  dataString += String(cnt);
  dataString += ",";
  dataString += String(voltage);
  dataString += ",";
  dataString += String(current);
  dataString += ",";
  dataString += String(watt);
  
  
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
  cnt++;

  //WAIT_TIMEまで待機
  while(millis() < time_zero + WAIT_TIME)
  {
    ;
  }
}
