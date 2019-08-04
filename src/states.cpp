#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "states.h"

/**
 *  Состояние фатальной ошибки
 */
class FatalErrorState : public IState {
  public:
    IState next(Context *ctx) {
      return *this;
    }
};

/**
 * Состояние запуска Web сервера
 */
class StartWebServerState : public IState {
  public:
    StartWebServerState() : server(80) {
    }
    
    IState next(Context *ctx) {
      start();
      // созвращаем состояние режима обнаружения
      // @TODO безусловно?
      return FatalErrorState();
    }

    void start() {
        server.on("/", std::bind(&StartWebServerState::mainPage, this));
        server.begin();
        Serial.println("Web server started on port 80");
    }
  private:
    ESP8266WebServer server;

    void mainPage(void) {
      server.send_P(200, "text/html", "<h1>TEST</h1>");
    }
};

/**
 * Состояние запуска WiFi сервера
 */
class StartWiFiServerState : public IState {
  public:
    IState next(Context *ctx) {
      WiFi.mode(WIFI_AP);
      if (WiFi.softAP(ctx->server_sessid, ctx->server_password)) {
        Serial.println("Wi-Fi access point started!");
        Serial.print("Wi-Fi device IP address: ");
        Serial.println(WiFi.softAPIP());
      
        return StartWebServerState();
      } else {
        Serial.println("Failure start wifi AP");
        return FatalErrorState();
      }
    }
};

/**
 * Класс состояния загрузки конфигурации
 */
class LoadConfigurationState : public IState {
  public:
    IState next(Context *ctx) {
      boolean success = false;
      if (read_file((char *)"login", ctx->client_ssid, 32)) {
        success = true;
      }
      
      if (success && read_file((char *)"password", ctx->client_password, 64)) {
        // to wifi client state
        return FatalErrorState(); // @TODO IMPLEMENT!!!
      } else {
        return StartWiFiServerState();
      }
    }
  private:
    boolean read_file(char* name, char* buffer, int size) {
      File f = SPIFFS.open("/login", "r");
      if (!f) {
        Serial.print("Unable open saved ");
        Serial.println(name);
        return false;
      }

      if (f.size() == 0) {
        Serial.print("Opened file is empty ");
        Serial.println(name);
        f.close();
        return false;
      }

      f.readBytes(buffer, size);
      f.close();

      Serial.print("Readed ");
      Serial.print(name);
      Serial.print(" with content: ");
      Serial.print(buffer);

      return true;
    }
};

/**
 * Класс стартового состояния приложения
 */
class InitializedState : public IState {
  public:
    IState next(Context *ctx) {
      return LoadConfigurationState();
    }
};

/**
 * Объявление инициализирующего состояния
 */
IState Context::init_state() {
    return InitializedState();
}
