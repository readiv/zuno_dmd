#define pin_led_1 A0 //аналоговый пин для чтения состояния светодиода 1 после нажатия сенсорной кнопки 1
#define pin_sens_btn_1 9  //пин сенсорной кнопки 1
#define pin_led_2 A1 //аналоговый пин для чтения состояния светодиода 1 после нажатия сенсорной кнопки 1
#define pin_sens_btn_2 12  //пин сенсорной кнопки 1

byte led_1 = 10; // Состояние светодиода 1, В loop читаем текущее состояние и пишем его сюда, 10 - состояние хз, При первом проходе loop обновится
byte z_led_1 = 10; // Имеет три состояния 0 - поступила комманда на выключение, 255 - поступила комманда на включение, 10 - нет команды
byte led_2 = 10; // Состояние светодиода 2, В loop читаем текущее состояние и пишем его сюда, 10 - состояние хз, При первом проходе loop обновится
byte z_led_2 = 10; // Имеет три состояния 0 - поступила комманда на выключение, 255 - поступила комманда на включение, 10 - нет команды

long previousMillis = 0;        // храним время последнего переключения светодиода
long interval = 30000;           // интервал между включение/выключением светодиода (1 секунда)

ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(led_1_Getter, led_1_Setter),
  ZUNO_SWITCH_BINARY(led_2_Getter, led_2_Setter)
);

void setup() {
  Serial.begin(9600);
  
  pinMode(pin_led_1, INPUT_PULLUP);
  pinMode(pin_led_2, INPUT_PULLUP);
  
  delay(1000); //На всяуий случай
}

void loop() {
//  Читаем светодиоды digitalWrite(LedPin4, (value > 0) ? LOW : HIGH);
  int sum_led_1 = 0;  // Считаем сумму по результатам 20 измерений
  int sum_led_2 = 0; 
  for (byte i=1; i <= 20; i++){
      sum_led_1 += (analogRead(pin_led_1) >> 2);
      sum_led_2 += (analogRead(pin_led_2) >> 2);
      delay(3);
  };
  byte new_led_1 = (sum_led_1 >= 2550) ? 0 : 255;
  byte new_led_2 = (sum_led_2 >= 2550) ? 0 : 255;
  
  if (new_led_1 != led_1) { //Если новое состояние не равно старому
    led_1 = new_led_1;
    zunoSendReport(1);
  }

  if (new_led_2 != led_2) { //Если новое состояние не равно старому
    led_2 = new_led_2;
    zunoSendReport(2);
  }

  if (z_led_1 != 10) { //Поступила комманда от z-wave на унопку 1
    if (z_led_1 != led_1) { //Разные состояния. Эммулируем нажатия сенсерной кнопки
      pinMode(pin_sens_btn_1,INPUT_PULLUP);
      delay( 70 ); 
      pinMode(pin_sens_btn_1,OUTPUT); 
      delay( 300 );  //Даём время на переключение
      Serial.print("Переключение led1");
    }
    z_led_1 = 10; 
  }

  if (z_led_2 != 10) { //Поступила комманда от z-wave на унопку 2
    if (z_led_2 != led_2) { //Разные состояния. Эммулируем нажатия сенсерной кнопки
      pinMode(pin_sens_btn_2,INPUT_PULLUP);
      delay( 70 ); 
      pinMode(pin_sens_btn_2,OUTPUT); 
      delay( 300 );  //Даём время на переключение
      Serial.print("Переключение led2");     
    }
    z_led_2 = 10; 
  }

  unsigned long currentMillis = millis(); // Отправка состояния каждые 30 секунд
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    for (byte i=1; i <= 4; i++){
      zunoSendReport(i); 
    };
  }
  
}

byte led_1_Getter(void) {
  Serial.print("led_1_Getter = ");
  Serial.println(led_1);
  return led_1;
}

void led_1_Setter(byte value) {
  Serial.print("led_1_Setter = ");
  Serial.println(value);
  z_led_1 = value;
}

byte led_2_Getter(void) {
  Serial.print("led_2_Getter = ");
  Serial.println(led_2);
  return led_2;
}

void led_2_Setter(byte value) {
  Serial.print("led_2_Setter = ");
  Serial.println(value);
  z_led_2 = value;
}
