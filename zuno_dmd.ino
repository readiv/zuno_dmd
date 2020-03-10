//Переключить светодиод можно двумя способами.
//1. Через z-wave
//2. Нажав сенсорную кнопку
//
//led_1 - в loop опрашивается светодиод и сосояние сохраняется сюда.
//Если led_1 измеолся, то report на zway
//
//Если z_led_1 не равен led_1, иммитируем нажатия сенсорной кнопки.
// 
//В getter просто возврат led_1
//В setter ставим присваиваем переменной z_led_1.

#define pin_led_1 A0 //аналоговый пин для чтения состояния светодиода 1 после нажатия сенсорной кнопки 1
#define pin_sens_btn_1 9  //пин сенсорной кнопки 1
#define pin_led_2 A1 //аналоговый пин для чтения состояния светодиода 1 после нажатия сенсорной кнопки 1
#define pin_sens_btn_2 12  //пин сенсорной кнопки 1

byte led_1 = 10; // Состояние светодиода 1, В loop читаем текущее состояние и пишем его сюда, 10 - состояние хз, При первом проходе loop обновится
byte z_led_1 = 10; // Имеет три состояния 0 - поступила комманда на выключение, 255 - поступила комманда на включение, 10 - нет команды
byte led_2 = 10; // Состояние светодиода 2, В loop читаем текущее состояние и пишем его сюда, 10 - состояние хз, При первом проходе loop обновится
byte z_led_2 = 10; // Имеет три состояния 0 - поступила комманда на выключение, 255 - поступила комманда на включение, 10 - нет команды

int ledState = LOW;             // этой переменной устанавливаем состояние светодиода
long previousMillis = 0;        // храним время последнего переключения светодиода
long interval = 1000;           // интервал между включение/выключением светодиода (1 секунда)


ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(led_1_Getter, led_1_Setter),
  ZUNO_SWITCH_BINARY(led_2_Getter, led_2_Setter)
);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(pin_led_1, INPUT); 
  digitalWrite(pin_led_1, HIGH); // включить подтягивающий резистор
  pinMode(pin_led_2, INPUT); 
  digitalWrite(pin_led_2, HIGH); // включить подтягивающий резистор
  
  delay(1000); //На всяуий случай
}

void loop() {
//  Serial.println();
//  Serial.println(led_1);
//  Serial.println(led_2);

//  Читаем светодиод 1
  int sum_led_1 = 0;  // Считаем сумму по результатам 20 измерений
  byte new_led_1 = 255;
  for (byte i=1; i <= 20; i++){
      sum_led_1 += (byte) (analogRead(pin_led_1) / 4);
      delay(2);
  };
  if (sum_led_1 >= 2550) { //Сумма больше половины максимальной
    new_led_1 = 0; 
  }

//  Читаем светодиод 2
  int sum_led_2 = 0;  // Считаем сумму по результатам 20 измерений
  byte new_led_2 = 255;
  for (byte i=1; i <= 20; i++){
      sum_led_2 += (byte) (analogRead(pin_led_2) / 4);
      delay(2);
  };
  if (sum_led_2 >= 2550) { //Сумма больше половины максимальной
    new_led_2 = 0; 
  }

  if ((new_led_1 != led_1) or (new_led_2 != led_2)) { //Если новое состояние не равно старому
    Serial.println();
    Serial.println(led_1);
    Serial.println(led_2);
    Serial.println(new_led_1);
    Serial.println(new_led_2);
        
    led_1 = new_led_1;
    led_2 = new_led_2;
    
    zunoSendReport(1);
    zunoSendReport(2);
  }

  if (z_led_1 != 10) { //Поступила комманда от z-wave на унопку 1
    if (z_led_1 != led_1) { //Разные состояния. Эммулируем нажатия сенсерной кнопки
      pinMode(pin_sens_btn_1,INPUT_PULLUP);
      delay( 70 ); 
      pinMode(pin_sens_btn_1,OUTPUT); 
      delay( 300 );  //Даём время на переключение
    }
    z_led_1 = 10; 
  }

  if (z_led_2 != 10) { //Поступила комманда от z-wave на унопку 1
    if (z_led_2 != led_2) { //Разные состояния. Эммулируем нажатия сенсерной кнопки
      pinMode(pin_sens_btn_2,INPUT_PULLUP);
      delay( 70 ); 
      pinMode(pin_sens_btn_2,OUTPUT); 
      delay( 300 );  //Даём время на переключение
    }
    z_led_2 = 10; 
  }

  unsigned long currentMillis = millis(); // Мигалка светодиодом
  
  //проверяем не прошел ли нужный интервал, если прошел то
  if(currentMillis - previousMillis > interval) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis; 
 
    // если светодиод не горит, то зажигаем, и наоборот
    if (ledState == LOW) ledState = HIGH; else ledState = LOW;
 
    // устанавливаем состояния выхода, чтобы включить или выключить светодиод
    digitalWrite(LED_BUILTIN, ledState);
  }
}

byte led_1_Getter(void) {
  return led_1;
}

void led_1_Setter(byte value) {
  z_led_1 = value;
}

byte led_2_Getter(void) {
  return led_2;
}

void led_2_Setter(byte value) {
  z_led_2 = value;
}
