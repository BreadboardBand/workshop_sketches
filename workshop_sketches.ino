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

static const int NUMBER_OF_SWITCHES = 2;
static const int switchPorts[NUMBER_OF_SWITCHES] = {2, 3};
int lastModeSwitchValues[NUMBER_OF_SWITCHES] = { -1, -1 };

int val = 0;  //アナログ入力値
int stepcnt =0;  //現在のステップ 
int i; 
float noteval;  //周波数 

enum Mode
{
  SEQUENCER = 0,
  OSCILLATOR,
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
    // DIPスイッチの設定（入力）
    pinMode(switchPorts[i], INPUT);
    // DIPスイッチの設定(何もつながれていない時にはHIGHにする)
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

/*
Copyright (c) 2011 - 2012 , Breadboard Band
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
- Neither the name of the Breadboard Band nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
Copyright (c) 2011 - 2012, Breadboard Band
All rights reserved.

ソースコード形式かバイナリ形式か、変更するかしないかを問わず、以下の条件を満たす場合に限り、再頒布および使用が許可されます。

- ソースコードを再頒布する場合、上記の著作権表示、本条件一覧、および下記免責条項を含めること。
- バイナリ形式で再頒布する場合、頒布物に付属のドキュメント等の資料に、上記の著作権表示、本条件一覧、および下記免責条項を含めること。
- 書面による特別の許可なしに、本ソフトウェアから派生した製品の宣伝または販売促進に、Breadboard Bandの名前またはコントリビューターの名前を使用してはならない。

本ソフトウェアは、著作権者およびコントリビューターによって「現状のまま」提供されており、明示黙示を問わず、商業的な使用可能性、および特定の目的に対する適合性に関する暗黙の保証も含め、またそれに限定されない、いかなる保証もありません。著作権者もコントリビューターも、事由のいかんを問わず、 損害発生の原因いかんを問わず、かつ責任の根拠が契約であるか厳格責任であるか（過失その他の）不法行為であるかを問わず、仮にそのような損害が発生する可能性を知らされていたとしても、本ソフトウェアの使用によって発生した（代替品または代用サービスの調達、使用の喪失、データの喪失、利益の喪失、業務の中断も含め、またそれに限定されない）直接損害、間接損害、偶発的な損害、特別損害、懲罰的損害、または結果損害について、一切責任を負わないものとします。
*/