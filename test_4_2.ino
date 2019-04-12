// локальный контроллер, UART, ModBus
#include <Tiny_ModBusRTU_Slave.h>
#include <TimerOne.h>
#include <DHT11.h>

#define MQ_PIN A0
#define LIGTH_PIN A1
#define FLAME_PIN A2

unsigned int regTable[5]; // таблица регистров
Tiny_ModBusRTU_Slave slave(2, 8, regTable, 5, 2); // создаем объект ModBus, адрес 1, таймаут 4 мс, массив regTable, размер 5, пин управление - 2
int pin = 3;
DHT11 dht11(pin);

float temp, humi, mqValue, mqPercents, svet, flame;  // температура, влажность, газ
int timeCount = 0, err; // счетчик времени

void setup() {
  Timer1.initialize(500);  // инициализация таймера 1, период 500 мкс
  Timer1.attachInterrupt(timerInterrupt, 500);  // задаем обработчик прерываний
  Serial.begin(9600);
}

void loop() {

  if (timeCount < 100) {
    timeCount = 100;
  }
  if (timeCount > 1900) {
    timeCount = 0;
    if ((err = dht11.read(humi, temp)) == 0); // чтение температуры и влажности

    mqValue = analogRead(MQ_PIN);// получаем значение от датчика MQ9
    mqPercents = mqValue / 1024 * 100; // переводим значение в проценты

    svet = analogRead(LIGTH_PIN);
    flame = analogRead(FLAME_PIN);

    // перегрузка данных в таблицу регистров
    noInterrupts();
    regTable[0] = temp;
    regTable[1] = humi;
    regTable[2] = mqPercents;
    regTable[3] = svet;
    regTable[4] = flame;
    interrupts();
  }
}

//--------------------------- обработчик прерывания 500 мкс
void  timerInterrupt() {
  slave.update(); // проверка данных обмена
  timeCount++;
}
