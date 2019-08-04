#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "states.h"

/**
 * Класс стейт машины
 */
class Application {
  public:
    Application() {
      context = Context();
      state = context.init_state();
    }
    void run() {
      state = state->next(&context);
    }
  private:
    IState* state; // значение по умолчанию
    Context context;
};

Application app;

void setup(void) {
  // ничего не делаем (((
    Serial.begin(115200);
    while (!Serial) {
      // wait serial
    }
}

void loop() {
  app.run();
}
