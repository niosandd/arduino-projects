<div align="center">
  <img src="../images/sceme.jpg" alt="sceme">
</div>

## Подключение компонентов
### Arduino Uno
<img src="../images/arduino-uno.jpg" alt="Arduino Uno" width="320">

| Компонент              | Пин на модуле | Пин на Arduino |
|------------------------|---------------|----------------|
| **LCD + I2C-адаптер**  | VCC           | 5V             |
|                        | GND           | GND            |
|                        | SDA           | A4             |
|                        | SCL           | A5             |
| **RTC DS1302**         | VCC           | 5V             |
|                        | GND           | GND            |
|                        | RST           | D4             |
|                        | DAT           | D3             |
|                        | CLK           | D13            |
| **Клавиатура 4×4**     | R1            | D12            |
|                        | R2            | D11            |
|                        | R3            | D10            |
|                        | R4            | D9             |
|                        | C1            | D8             |
|                        | C2            | D7             |
|                        | C3            | D6             |
|                        | C4            | D5             |
| **Датчик звука**       | VCC / +       | 5V             |
|                        | GND           | GND            |
|                        | AO            | A0             |
|                        | DO            | D2 (опционально) |

---

### LCD 1602 + I2C-адаптер
<img src="../images/I2C-adapter.jpg" alt="I2C Adapter" width="320">
<img src="../images/LCD-display.jpg" alt="LCD Display" width="320">

### RTC DS1302 (MH-Real-Time Clock Modules-2)
<img src="../images/MH-Real-Time-Clock-Modules-2.jpg" alt="RTC DS1302" width="320">

### Матричная клавиатура 4×4
<img src="../images/4x4.jpg" alt="4x4 Keypad" width="320">

### Датчик звука
<img src="../images/sound-sensor-module.jpg" alt="Sound Sensor" width="320">

---

### Общая схема подключения
<img src="../images/main-scheme-arduino-sound-logger.jpg" alt="Общая схема" width="320">
