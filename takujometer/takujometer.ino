/*  wattmeter_for_takusabo by K.Sakurai 2018****
 *  卓上サボニウス型風車風力発電機の起電力測定デバイス用プログラムです
 *  INA226モジュールで電圧・電流を測定してSDに書き込んでいく
 *  スイッチでスタート・ストップができるもの
 *  連続測定時間は1時間程度を想定＝バッテリ効率はあんま考えなくてよい
 */

//ライブラリ等導入


//=====【Hardware Configration】ハードウェアに応じて以下を変更すること
const int chipSelect = 10;    //SDピンアサイン CSは10
const int analogPin = 0;    //アナログピンの番号 A0つかいます

//=====【Software Configration】測定の仕様を変えたければ以下を変更すること
const int NUMDET = 100;   //1つのデータにつき測定回数は100回
const int INTERVAL = 30;  //測定間隔は30ms
const int WAIT_TIME = 5000; //データ取得間隔は5000ms 処理時間は実測200ms程度なので、300ms以下NG

//=====【Configration end】


void setup() {

  //LをOUTPUTに
  pinMode(LED_BUILTIN, OUTPUT);

  //SDカードスロットを初期化　エラー時にはLEDを速く点滅
  if (!SD.begin(chipSelect)) {
    while(1){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
    return; //エラー時は処理を中断する
  }

  //OLED初期化
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // aitendoで売ってるOLEDデバイスの I2Cアドレスは0x3C
  display.clearDisplay();

  //データの通し番号初期化
  number = 0;

}

void loop() {

  //開始時刻を格納・valの初期化
  time_zero = millis();
  val = 0;

  //電圧・電流・電力を定義
  float volt, ampere, watt;

  //電圧・電流・電力を取得

  //OLEDに表示

  //SDに出力

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
