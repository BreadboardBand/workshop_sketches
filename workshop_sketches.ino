//4ステップ シーケンサ 
//可変抵抗の状態で各ステップの周波数をコントロール 
//可変抵抗の状態でテンポのコントロール

int outpin = 9;   // スピーカにつなぐピン 
int seqpin0 = 0; //可変抵抗1ステップ目をつなぐピン 
int seqpin1 = 1; //可変抵抗2ステップ目をつなぐピン 
int seqpin2 = 2; //可変抵抗3ステップ目をつなぐピン 
int seqpin3 = 3; //可変抵抗4ステップ目をつなぐピン 
int sensorpin = 5;
int tempopin = 4; //テンポを変更 

// saita modified
static const int NUMBER_OF_SWITCHES = 2;
static const int switchPorts[NUMBER_OF_SWITCHES] = {2, 3};
int lastModeSwitchValues[NUMBER_OF_SWITCHES] = { -1, -1 };

int val = 0;  //アナログ入力値
int stepcnt =0;  //現在のステップ 
int i; 
float noteval;  //周波数 

enum Mode
{
  OSCILLATOR = 0,
  SEQUENCER,
  NUMBER_OF_MODES,
};

int mode = 0;

void detectChange()
{
  for (int i = 0; i < NUMBER_OF_SWITCHES; ++i) {
    const int newValue = digitalRead(switchPorts[i]);
    
    if (lastModeSwitchValues[i] != newValue) {
      lastModeSwitchValues[i] = newValue;
    }
  }
  
  int newMode = (lastModeSwitchValues[0] << 1) + lastModeSwitchValues[1];

  if (newMode >= NUMBER_OF_MODES) {
    newMode = NUMBER_OF_MODES - 1;
  }
  
  if (newMode != mode) {
    mode = newMode;
    Serial.print("mode = ");
    Serial.println(mode);
  }
}

void setup() 
{ 
  Serial.begin(9600);
  
  for(int i = 0; i < NUMBER_OF_SWITCHES; ++i) { 
    // make them input
    pinMode(switchPorts[i], INPUT);
    // make them pullup
    digitalWrite(switchPorts[i], HIGH);
  }
}

void loop()
{
  detectChange(); // モードの変化があるかを調べる
  
  switch (mode) {
    case OSCILLATOR:
    osc();
    break;
    case SEQUENCER:
    seq();
    break;
    default:
    break;
  }
}

void osc(){
  int val = (analogRead(tempopin) >> 2) << 2;     //可変抵抗からの入力値を読む 
  int sensor = (analogRead(sensorpin) >> 2) << 2;     //可変抵抗からの入力値を読む 
  
  //入力値の範囲を出したい音の周波数の範囲に変更
  //0〜1023の範囲の入力値を0〜400の範囲に変換 
  int thisPitch = map(val, 0, 1023, 0, 4000);
  int sensorPitch = map(sensor, 0, 1023, 0, 4000);

  //指定した周波数の矩形波を生成します（音がでます）
  //tone(トーンを出力するピン, 周波数(Hz), 出力する時間をミリ秒で指定できます(オプション))
  tone(outpin, thisPitch+sensorPitch, 10);

}


void seq(){
  int tempo = (analogRead(tempopin) >> 2) << 2;  //テンポ用にアナログ入力を読む 
  tempo = constrain(tempo, 1, 1023);      //入力値の0を切り捨てる 
  int sensor = (analogRead(sensorpin) >> 2) << 2;     //可変抵抗からの入力値を読む 
  int sensorPitch = map(sensor, 0, 1023, 0, 4000);
  
  if(stepcnt>3){    //もし現在のステップが3より大きかったら0にリセット 
    stepcnt = 0; 
  } 
  if(stepcnt == 0){                      //もし現在のステップが0 
      val = (analogRead(seqpin0) >> 2) << 2;  //0ステップ目の入力値 
  } 
  else if(stepcnt == 1){               //もし現在のステップが1 
      val = (analogRead(seqpin1) >> 2) << 2;  //1ステップ目の入力値 
  } 
  else if(stepcnt == 2){               //もし現在のステップが2 
      val = (analogRead(seqpin2) >> 2) << 2;  //2ステップ目の入力値 
  } 
  else if(stepcnt == 3){               //もし現在のステップが3 
      val = (analogRead(seqpin3) >> 2) << 2;  //3ステップ目の入力値 
  } 

  stepcnt++;         // ステップ数を1プラス 
  val = constrain(val, 1, 1023);      //入力値の0を切り捨てる 

  //入力値の範囲を出したい音の周波数の範囲に変更
  //0〜1023の範囲の入力値を0〜400の範囲に変換 
  int thisPitch = map(val, 1, 1023, 1, 4000);
  tone(outpin, thisPitch + sensorPitch, tempo);

  //音を区切るために、音を出す間隔（時間）をとります
  //音の出力する時間 + 30%がうまく動作します
  int pauseBetweenNotes = tempo * 1.30;
  delay(pauseBetweenNotes);

}

