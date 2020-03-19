#define pin_led_1 A0 //аналоговый пин для чтения состояния светодиода 1
#define pin_sens_btn_1 9  //пин сенсорной кнопки 1
#define pin_led_2 A1 //аналоговый пин для чтения состояния светодиода 2
#define pin_sens_btn_2 12  //пин сенсорной кнопки 2

#define pin_sw_1 19 //пин для чтения состояния переключателя 1 
#define pin_rellay_1 20  //пин реле 1 для включения нагрузки
#define pin_sw_2 21 //пин для чтения состояния переключателя 1 
#define pin_rellay_2 13  //пин реле 1 для включения нагрузки

byte led_1 = 10; // Состояние светодиода 1, В loop читаем текущее состояние и пишем его сюда, 10 - состояние хз, При первом проходе loop обновится
byte z_led_1 = 10; // Имеет три состояния 0 - поступила комманда на выключение, 255 - поступила комманда на включение, 10 - нет команды
byte led_2 = 10; // Состояние светодиода 2, В loop читаем текущее состояние и пишем его сюда, 10 - состояние хз, При первом проходе loop обновится
byte z_led_2 = 10; // Имеет три состояния 0 - поступила комманда на выключение, 255 - поступила комманда на включение, 10 - нет команды

byte sw_1 = 10; // Выключатель // 0 - одно состояние // 255 - другое // 10 - неизвестно
byte sw_2 = 10; // Выключатель // 0 - одно состояние // 255 - другое // 10 - неизвестно

long previousMillis = 0;        // храним время последней отправки zuno
long interval = 30000;           // интервал между отправками данных в сеть z-wabe
bool flag_send_zreport = false;

ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(led_1_Getter, led_1_Setter),
  ZUNO_SWITCH_BINARY(led_2_Getter, led_2_Setter),
  ZUNO_SWITCH_BINARY(rellay_1_Getter, rellay_1_Setter),
  ZUNO_SWITCH_BINARY(rellay_2_Getter, rellay_2_Setter)
);

void setup() {
  Serial.begin(9600);
  
  pinMode(pin_led_1, INPUT_PULLUP);
  pinMode(pin_led_2, INPUT_PULLUP);
  pinMode(pin_sens_btn_1,OUTPUT);
  pinMode(pin_sens_btn_2,OUTPUT);
  
  pinMode(pin_sw_1, INPUT_PULLUP); 
  pinMode(pin_rellay_1, OUTPUT); 
  digitalWrite(pin_rellay_1, HIGH); // включаем нагрузку

  pinMode(pin_sw_2, INPUT_PULLUP);
  pinMode(pin_rellay_2, OUTPUT); 
  digitalWrite(pin_rellay_2, HIGH); // включаем нагрузку

  previousMillis = millis() - interval + 5000; // отправить состояние через 5 секунд
  
  delay(1000); //На всякий случай // свет моргнет при инициализации на это время
}

void set_flag_send_zreport() {
    if (not flag_send_zreport) {
      previousMillis = millis() - interval + 300; // Отправить zunoSendReport по всем каналам через 300 милисекунд
      flag_send_zreport = true;
    }    
}

void loop() {
//  Читаем светодиоды и выключатели
  int sum_led_1 = 0;  // Считаем сумму по результатам 20 измерений
  int sum_led_2 = 0; 
  int sum_sw_1 = 0;  // Считаем сумму по результатам 20 измерений
  int sum_sw_2 = 0;  // Считаем сумму по результатам 20 измерений
  for (byte i=1; i <= 20; i++){
      sum_led_1 += (analogRead(pin_led_1) >> 2);
      sum_led_2 += (analogRead(pin_led_2) >> 2);
	    sum_sw_1 += digitalRead(pin_sw_1);
      sum_sw_2 += digitalRead(pin_sw_2);
      delay(3);
  };
  byte new_led_1 = (sum_led_1 >= 2550) ? 0 : 255;
  byte new_led_2 = (sum_led_2 >= 2550) ? 0 : 255;
  byte new_sw_1 = (sum_sw_1 < 10) ? 0 : 255;
  byte new_sw_2 = (sum_sw_2 < 10) ? 0 : 255;
  
  if (new_led_1 != led_1) { //Если новое состояние не равно старому
    led_1 = new_led_1;
    set_flag_send_zreport();
  }

  if (new_led_2 != led_2) { //Если новое состояние не равно старому
    led_2 = new_led_2;
    set_flag_send_zreport();
  }

  if (z_led_1 != 10) { //Поступила комманда от z-wave на кнопку 1
    if (z_led_1 != led_1) { //Разные состояния. Эммулируем нажатия сенсерной кнопки
      pinMode(pin_sens_btn_1,INPUT_PULLUP);
      delay( 100 ); 
      pinMode(pin_sens_btn_1,OUTPUT); 
      set_flag_send_zreport(); 
      Serial.print("1");
    }
    z_led_1 = 10; 
  }

  if (z_led_2 != 10) { //Поступила комманда от z-wave на кнопку 2
    if (z_led_2 != led_2) { //Разные состояния. Эммулируем нажатия сенсерной кнопки
      pinMode(pin_sens_btn_2,INPUT_PULLUP);
      delay( 100 ); 
      pinMode(pin_sens_btn_2,OUTPUT); 
      set_flag_send_zreport(); 
      Serial.print("2");   
    }
    z_led_2 = 10; 
  }
  
  if (new_sw_1 != sw_1) { // Нажали выключатель 1 
    digitalWrite(pin_rellay_1, (digitalRead(pin_rellay_1) == 0) ? HIGH : LOW);
    sw_1 = new_sw_1;
    set_flag_send_zreport(); 
  }

  if (new_sw_2 != sw_2) { // Нажали выключатель 2
    digitalWrite(pin_rellay_2, (digitalRead(pin_rellay_2) == 0) ? HIGH : LOW);
    sw_2 = new_sw_2;
    set_flag_send_zreport();
  }

  if(millis() > previousMillis + interval) { // Отправка состояния каждые interval секунд
    previousMillis = millis();
    flag_send_zreport = false; //флаг сбрасываем в любом случае
    for (byte i=1; i <= 4; i++){
      zunoSendReport(i); 
    };
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

byte rellay_1_Getter(void) {
  byte tmp = digitalRead(pin_rellay_1);
  return (tmp == 0) ? 0 : 255;
}

void rellay_1_Setter(byte value) {
  digitalWrite(pin_rellay_1, (value == 0) ? LOW : HIGH);
}

byte rellay_2_Getter(void) {
  byte tmp = digitalRead(pin_rellay_2);
  return (tmp == 0) ? 0 : 255;
}

void rellay_2_Setter(byte value) {
  digitalWrite(pin_rellay_2, (value == 0) ? LOW : HIGH);
}
