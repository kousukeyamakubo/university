const int buttonPin = 8;  // the pin that the pushbutton is attached to
const int segA = 7;
const int segB = 6;
const int segC = 5;
const int segD = 4;
const int INa = 12;
const int INb = 11;
const int INc = 10;
const int INd = 9;
int buttonState = 0;
int previousbuttonState = 0;
int change = 0;
int aState = 0;
int bState = 0;
int cState = 0;
int dState = 0;
int memory[7];          //入力された数値を記憶
int answer_flag = 0;    //答えを表示するときに立つフラグ
int input_counter = 0;  //入力された回数を数える
int answer = 0;         //計算結果
int answer_memory[6];   //回答の各位の数を記憶
int count = 0;          //回答を表示した回数を記憶
int digit_count = 0;    //答えが何桁かを記憶
int error_flag = 0;     //エラーが起きたときに立つフラグ
int now = 0;            //エラーが起きたときの時刻を記憶する

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(INa, INPUT);
  pinMode(INb, INPUT);
  pinMode(INc, INPUT);
  pinMode(INd, INPUT);
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  digitalWrite(segA, 1);
  digitalWrite(segB, 1);
  digitalWrite(segC, 1);
  digitalWrite(segD, 1);
  Serial.begin(9600);
}

void loop() {
  if (error_flag = 1) {
    error();
  } else {
    aState = digitalRead(INa);
    bState = digitalRead(INb);
    cState = digitalRead(INc);
    dState = digitalRead(INd);
    //ボタンが離れたときにchange = 1
    offbutton();
    if (change == 1) {
      if (answer_flag == 1) {
        //解答を表示
        output();
      } else {
        int sum = dState * 8 + cState * 4 + bState * 2 + aState;
        if (sum == 15) {
          //複数桁の数値を配列の一つの要素に入れる
          combine();
          //上の処理がうまくいけば数字　演算子　数字という形にmemoryの中が変わっているはず.以下では実際の計算と出力用の配列に格納する処理
          if (memory[1] >= 10 && memory[1] <= 13 && memory[3] == -1) {
            calculate();
          } else {
            //エラー処理
            now = millis();
            error_flag = 1;
          }
        } else if (sum < 14) {
          //memoryに入力
          if (input_counter == 8) {
            //エラー処理
            now = millis();
            error_flag = 1;
          } else {
            memory[++input_counter] = sum;
          }
        }
      }
    }
  }
}

void offbutton() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    if (previousbuttonState == LOW) {
      previousbuttonState = HIGH;
    }
  } else if (buttonState == LOW) {
    if (previousbuttonState == HIGH) {
      previousbuttonState = LOW;
      change = 1;
    }
  }
}

void error() {
  for (int i = 0; i < input_counter; i++) {
    memory[i] = 0;
  }
  if (millis() - now <= 1000) {
    digitalWrite(segA, 0);
    digitalWrite(segB, 1);
    digitalWrite(segC, 1);
    digitalWrite(segD, 1);
  } else {
    digitalWrite(segA, 1);
    digitalWrite(segB, 1);
    digitalWrite(segC, 1);
    digitalWrite(segD, 1);
    error_flag = 0;
  }
}

void combine() {
  for (int i = 0; i < input_counter; i++) {
    if (memory[i] < 10) {
      if (memory[i + 1] < 10) {
        if (memory[i + 2] < 10) {
          memory[i] = memory[i] * 100 + memory[i + 1] * 10 + memory[i + 2];
          for (int j = i + 1; j < input_counter; j++) {
            if (j <= input_counter - 2) {
              memory[j] = memory[j + 2];
            } else {
              memory[j] = -1;
            }
            input_counter = input_counter - 2;
          }
          i = i + 2;
        }
        memory[i] = memory[i] * 10 + memory[i + 1];
        for (int j = i + 1; j < input_counter; j++) {
          if (j <= input_counter - 1) {
            memory[j] = memory[j + 1];
          } else {
            memory[j] = -1;
          }
          input_counter = input_counter - 1;
        }
        i = i + 1;
      }
    }
  }
}

void calculate() {
  if (memory[1] == 10) {
    answer = memory[0] + memory[2];
  } else if (memory[1] == 11) {
    answer = memory[0] - memory[2];
  } else if (memory[1] == 12) {
    answer = memory[0] * memory[2];
  } else if (memory[1] == 13) {
    if (memory[2] == 0) {
      now = millis();
      error_flag = 1;
    }
    answer = memory[0] / memory[2];
  }
  int temp = answer;
  while (temp != 0) {
    temp /= 10;
    digit_count++;
  }
  for (int i = digit_count - 1; i >= 0; i--) {
    answer_memory[i] = answer % 10;
    answer /= 10;
  }
  answer_flag = 1;
}

void output() {
  if (count <= digit_count) {
    change = 0;
    aState = answer_memory[count] % 2;
    bState = ((answer_memory[count] % 4) - aState) / 2;
    cState = ((answer_memory[count] % 2) - bState * 2 - aState) / 4;
    dState = ((answer_memory[count] % 2) - cState * 4 - bState * 2 - aState) / 8;
    digitalWrite(segA, aState);
    digitalWrite(segB, bState);
    digitalWrite(segC, cState);
    digitalWrite(segD, dState);
    count++;
  } else {
    answer_flag = 0;
    digitalWrite(segA, 1);
    digitalWrite(segB, 1);
    digitalWrite(segC, 1);
    digitalWrite(segD, 1);
  }
}
