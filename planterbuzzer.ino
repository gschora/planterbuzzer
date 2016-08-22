#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 17
Adafruit_SSD1306 display(OLED_RESET);

#define btn_a                   2
#define btn_b                   3
#define btn_c                   4
#define BTN_HOLD_TIME           200
#define BTN_CONF_TIME           3000

#define DISTANCE_STEP_SIZE      5
#define DISTANCE_MIN            50
#define DISTANCE_MAX            200

#define SERIAL_BAUD 115200

int btn_a_time = 0;
int btn_b_time = 0;
int btn_c_time = 0;
int btn_c_switch_time = 0;

int distance_a = DISTANCE_MIN;
int distance_b = DISTANCE_MIN;

// modes:
// 1 - only distance vom a is used - only one beep
// 2 - same distance for both  - two beeps, different tone, in turn
// 3 - different distance for both - two beeps, different tone
//
uint8_t mode = 0;
bool is_config_mode = false;

void setup()
{
        Serial.begin(SERIAL_BAUD);
        while (!Serial && (millis () <= 8000));

        pinMode(btn_a, INPUT_PULLDOWN);
        pinMode(btn_b, INPUT_PULLDOWN);
        pinMode(btn_c, INPUT_PULLDOWN);

        display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
        display.display();
        delay(2000);
        display.clearDisplay();
}

void loop()
{
        checkbtn();
        display_mode();
}

void display_mode(){
        switch (mode) {
        case 0:
                display_mode_0();
                break;
        case 1:
                display_mode_1();
                break;
        case 2:
                display_mode_2();
                break;
        case 3:
                display_mode_3();
                break;
        }
}
void display_mode_0(){
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(20,0);
        display.println("planterbuzzer");

        display.setCursor(0,20);
        display.print("A: ");
        display.println(distance_a);
        display.setCursor(64,20);
        display.print("B: ");
        display.println(distance_b);

        display.display();
}
void display_mode_1(){
        display.clearDisplay();
        display.setCursor(10,0);
        display.println("Konfig Abstand");
        display.setCursor(0,20);
        display.print("A: ");
        display.println(distance_a);
        display.setCursor(64,20);
        display.print("B: ");
        display.println(distance_b);
        display.display();
}
void display_mode_2(){
        display.clearDisplay();
        display.setCursor(10,0);
        display.println("Konfig Tone A");
        display.setCursor(0,20);
        display.print("A: ");
        display.println(distance_a);
        display.setCursor(64,20);
        display.print("B: ");
        display.println(distance_b);
        display.display();
}
void display_mode_3(){
        display.clearDisplay();
        display.setCursor(10,0);
        display.println("Konfig Tone B");
        display.setCursor(0,20);
        display.print("A: ");
        display.println(distance_a);
        display.setCursor(64,20);
        display.print("B: ");
        display.println(distance_b);
        display.display();
}
void checkbtn(){
        int t = millis();

        chk_config(t);

        if(is_config_mode) {
                // Serial.print("mode: ");
                // Serial.println(mode);
                switch (mode) {
                case 1:
                        config_distance(t);
                        break;
                case 2:
                        config_tone_a(t);
                        break;
                case 3:
                        config_tone_b(t);
                        break;
                }
        }

        // config_distance(t);
        // config_tone_a(t);
        // config_tone_b(t);

}

void chk_config(int t){
        if (digitalRead(btn_c)) {
                if(btn_c_time <= 0) {
                        btn_c_time = millis();
                }
                if((t-btn_c_time) > BTN_CONF_TIME) {
                        btn_c_time = 0;
                        if (!is_config_mode) {
                                Serial.println("config mode");
                                is_config_mode = true;
                        } else if (is_config_mode) {
                                Serial.println("config mode off, saving data");
                                is_config_mode = false;
                                display.clearDisplay();
                                display.setCursor(10,25);
                                display.println("konfig gespeichert");
                                display.display();
                                delay(2000);
                                mode = 0;

                        }
                } else if(is_config_mode && (t-btn_c_switch_time) > BTN_HOLD_TIME) {
                        mode++;

                        if (mode >3) {
                                mode = 1;
                        }
                        btn_c_switch_time = millis();
                        is_config_mode = true;
                        Serial.print("switching config mode: ");
                        Serial.println(mode);
                }
        } else {
                btn_c_time = 0;
        }
}

void config_distance(int t){
        if (digitalRead(btn_a)) {
                if(btn_a_time <= 0) {
                        btn_a_time = millis();
                }
                if((t-btn_a_time) > BTN_HOLD_TIME) {
                        btn_a_time = millis();
                        distance_a = distance_a + DISTANCE_STEP_SIZE;
                        if(distance_a > DISTANCE_MAX) {
                                distance_a = DISTANCE_MIN;
                        }
                        Serial.print("set dist a: ");
                        Serial.println(distance_a);
                }
        }

        if (digitalRead(btn_b)) {
                if(btn_b_time <= 0) {
                        btn_b_time = millis();
                }
                if((t-btn_b_time) > BTN_HOLD_TIME) {
                        btn_b_time = millis();
                        distance_b = distance_b + DISTANCE_STEP_SIZE;
                        if(distance_b > DISTANCE_MAX) {
                                distance_b = DISTANCE_MIN;
                        }
                        Serial.print("set dist b: ");
                        Serial.println(distance_b);
                }
        }


}

void config_tone_a(int t){
        if (digitalRead(btn_a)) {
                if(btn_a_time <= 0) {
                        btn_a_time = millis();
                }
                if((t-btn_a_time) > BTN_HOLD_TIME) {
                        btn_a_time = millis();
                        Serial.println("config_tone_a_up");
                }
        }

        if (digitalRead(btn_b)) {
                if(btn_b_time <= 0) {
                        btn_b_time = millis();
                }
                if((t-btn_b_time) > BTN_HOLD_TIME) {
                        btn_b_time = millis();
                        Serial.println("config_tone_a_down");
                }
        }
}
void config_tone_b(int t){
        if (digitalRead(btn_a)) {
                if(btn_a_time <= 0) {
                        btn_a_time = millis();
                }
                if((t-btn_a_time) > BTN_HOLD_TIME) {
                        btn_a_time = millis();
                        Serial.println("config_tone_b_up");
                }
        }

        if (digitalRead(btn_b)) {
                if(btn_b_time <= 0) {
                        btn_b_time = millis();
                }
                if((t-btn_b_time) > BTN_HOLD_TIME) {
                        btn_b_time = millis();
                        Serial.println("config_tone_b_down");
                }
        }
}
