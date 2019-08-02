#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

/**
 * Возможные состояния приложения
 */
const int STATE_FATAL_ERROR = 0; // Фатальная ошибка
const int STATE_INITIALIZATION = 1; // Чтение конфига
const int STATE_CONFIGURATION_READ_SUCCESS = 2; // Конфигурация прочитана
const int STATE_CONFIGURATION_READ_FAILURE = 3; // Ошибка чтения конфигурации
const int STATE_CONFIGURATION_VALID = 4; // Конфигурация корректна
const int STATE_CONFIGURATION_INVALID = 5; // Конфигурация не корректна
const int STATE_CONFIGURATION_DEFAULT = 6; // Применение конфигурации по умолчанию
const int STATE_START_WIFI_SERVER = 7; // Поднятие Wi-Fi точки доступа
const int STATE_START_WIFI_CLIENT = 8; // Поднятие Wi-Fi в виде клиента
const int STATE_START_WEB_SERVER = 9; // Запуск Web сервера
const int STATE_START_DISCOVERT = 10; // Запуск сервиса обнаружения
const int STATE_WAIT_COMMAND = 11; // Режим ожидания комманд

/**
 * Класс для хранения значения состояния
 */
class State {
  public:
    void set_state(int new_state) {
      Serial.printf("Apply new state from %d to %d\n", state, new_state);
      state = new_state;
    }

    int get_state() {
      return state;
    }
  private:
    int state = 1; // значение по умолчанию
};

/**
 * Класс для работы с web сервером
 */
class WebServer 
{
    public:
      WebServer() : server(80) {
      }
      void start() {
          server.on("/", std::bind(&WebServer::mainPage, this));
          server.begin();
      }
    private:
      ESP8266WebServer server;

      void mainPage(void) {
        server.send_P(200, "text/html", "<h1>TEST</h1>");
      }
};

/**
 * Конфигурация приложения
 */
struct {
  State state; // Используется в работе приложения
  char client_ssid[32]; // Читается из файла
  char client_password[64]; // Читается из файла
  const char* server_sessid = "ServerWiFi-0001"; // Имя Wi-Fi по умолчанию
  const char* server_password = "tempPassNG"; // Пароль от Wi-Fi по умолчанию
} app_data;

void process_fatal_error() {
  // Режим ожидания...
  //Serial.println("Process fatal error...");
}

// Чтение конфигурации
void load_configuration() {
  // Открываем файл с конфигурацией
  File f = SPIFFS.open("/config.txt", "r");
  if (!f) {
    Serial.println("Unable open config.txt");
    app_data.state.set_state(STATE_CONFIGURATION_READ_FAILURE);
    return;
  }

  if (f.size() == 0) {
    // Конфигурационный файл пуст
    Serial.println("Configuration file is empty");
    app_data.state.set_state(STATE_CONFIGURATION_INVALID);
    return;
  }

  // Читаем данные
  char buffer[96];
  f.readBytes(buffer, 96);
  f.close();

  memcpy(app_data.client_ssid, buffer, 32);
  memcpy(app_data.client_password + 64, buffer, 64);

  app_data.state.set_state(STATE_CONFIGURATION_READ_SUCCESS);
}

// Валидация конфигурации
void validate_configuration() {
  if ((sizeof(app_data.client_ssid) == 0) || (sizeof(app_data.client_password) == 0)) {
    app_data.state.set_state(STATE_CONFIGURATION_INVALID);
  } else {
    app_data.state.set_state(STATE_CONFIGURATION_VALID);
  }
}

void load_default_configuration() {
  // nothing...
  app_data.state.set_state(STATE_START_WIFI_SERVER);
}

void start_wifi_server() {
  WiFi.mode(WIFI_AP);
  if (WiFi.softAP(app_data.server_sessid, app_data.server_password)) {
    Serial.println("Wi-Fi access point started!");
    Serial.print("Wi-Fi device IP address: ");
    Serial.println(WiFi.softAPIP());
    app_data.state.set_state(STATE_START_WEB_SERVER);
  } else {
    Serial.println("Failure start wifi AP");
    app_data.state.set_state(STATE_FATAL_ERROR);
  }
}

void start_web_server() {
  WebServer webServer;
  webServer.start();
  app_data.state.set_state(STATE_START_DISCOVERT);
}

void setup(void) {
  // ничего не делаем (((
    Serial.begin(115200);
    while (!Serial) {
      // wait serial
    }
}

void loop() {
  int state = app_data.state.get_state();
  switch (state)
  {
  case STATE_FATAL_ERROR:
    process_fatal_error();
    break;
  case STATE_INITIALIZATION:
    load_configuration();
    break;
  case STATE_CONFIGURATION_READ_SUCCESS:
    validate_configuration();
    break;
  case STATE_CONFIGURATION_READ_FAILURE:
    app_data.state.set_state(STATE_CONFIGURATION_INVALID);
    break;
  case STATE_CONFIGURATION_INVALID:
    app_data.state.set_state(STATE_START_WIFI_SERVER);
    break;
  case STATE_START_WIFI_SERVER:
    start_wifi_server();
    break;
  case STATE_START_WEB_SERVER:
    start_web_server();
    break;
  default:
    Serial.print("Unprocessed state: ");
    Serial.println(state);
    app_data.state.set_state(STATE_FATAL_ERROR);
    break;
  }
}
