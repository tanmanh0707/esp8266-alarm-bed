#include "common.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D6;
const int LOADCELL_SCK_PIN = D7;

HX711 scale;
Servo myservo;

static const int SCALE_CALIB_FACTOR = -21361;

bool LocalCheckWeight();

void setup()
{
  Serial.begin(115200);
  delay(2000);
  Serial.println("");

  /* Database */
  DB_Init();

  /* WiFi */
  WIFI_Init();

  /* Time */
  TimeClient_Init();

  /* Buzzer */
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  /* Servo */
  myservo.attach(SERVO_PIN, 500, 2400);
  delay(500);
  myservo.write(0);

  Serial.println("Đang khởi tạo module HX711...");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  delay(1000);

  while (! scale.is_ready()) {
    Serial.println("Không tìm thấy module HX711!");
    delay(1000);
    break;
  }

  if (scale.is_ready()) {
    scale.set_scale(SCALE_CALIB_FACTOR);
    scale.tare();
    Serial.println("Module HX711 đã sẵn sàng!");
  }
}

void loop()
{
  static byte buzzer_state = LOW;
  TimeClient_Task();

  /* Check */
  uint8_t hour = 0, min = 0, crt_hrs = 0, crt_min = 0;
  DB_GetTimerOn(hour, min);

  // Serial.print("DB: "); Serial.print(hour);Serial.print(":");Serial.print(min);
  TimeClient_GetTime(crt_hrs, crt_min);
  // Serial.print(" - Current: "); Serial.print(crt_hrs);Serial.print(":");Serial.println(crt_min);
  if (crt_hrs != 0 && crt_min != 0 && crt_hrs == hour && crt_min == min && LocalCheckWeight()) {
    if (buzzer_state == LOW) {
      Serial.println("Buzzer ON");
      digitalWrite(BUZZER_PIN, HIGH);
      myservo.write(180);
      buzzer_state = HIGH;
    }
  } else {
    if (buzzer_state == HIGH) {
      Serial.println("Buzzer OFF");
      digitalWrite(BUZZER_PIN, LOW);
      myservo.write(0);
      buzzer_state = LOW;
    }
  }

  delay(5000);
}

bool LocalCheckWeight()
{
  bool ret = false;

  if (scale.is_ready())
  {
#if 1
    float weight = scale.get_units(10);
    ret = weight > 10;
#else
    scale.set_scale();    
    Serial.println("Bỏ hết vật ra khỏi cân...");
    delay(10000);
    Serial.println("Đang cân bì...");
    scale.tare();
    Serial.println("Cân bì xong!");
    Serial.println("Đặt một vật đã biết khối lượng lên cân...");
    delay(10000);
    Serial.print("Đang cân... ");
    long reading = scale.get_units(10);
    Serial.print("Kết quả: ");
    Serial.println(reading);
#endif
  }

  return ret;
}