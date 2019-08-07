#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "states.h"
#include "storage.h"
#include "webserver.h"

/**
 *  Состояние фатальной ошибки
 */
class FatalErrorState : public IState {
  public:
    IState* next(Context *ctx) {
      return this;
    }
};

/**
 * Корректное рабочее состояние
 */
class WorkerState : public IState {
  IState *next(Context *ctx) {
    ctx->webserver->handle();
    return this;
  }
};

/**
 * Состояние запуска механизма обнаружения
 */
class StartDiscoveryState : public IState {
  public:
    IState *next(Context *ctx) {
      return new WorkerState();
    }
};

/**
 * Состояние запуска Web сервера
 */
class StartWebServerState : public IState {
  public:
    IState* next(Context *ctx) {
      IWebServer *webserver = WebServerFactory().create();
      webserver->start();
      ctx->webserver = webserver;
      // созвращаем состояние режима обнаружения
      // @TODO безусловно?
      return new FatalErrorState();
    }
};

/**
 * Состояние запуска WiFi сервера
 */
class StartWiFiServerState : public IState {
  public:
    IState* next(Context *ctx) {
      WiFi.mode(WIFI_AP);
      if (WiFi.softAP(ctx->server_sessid, ctx->server_password)) {
        Serial.println("Wi-Fi access point started!");
        Serial.print("Wi-Fi device IP address: ");
        Serial.println(WiFi.softAPIP());
      
        return new StartWebServerState();
      } else {
        Serial.println("Failure start wifi AP");
        return new FatalErrorState();
      }
    }
};

/**
 * Класс состояния загрузки конфигурации
 */
class LoadConfigurationState : public IState {
  public:
    IState* next(Context *ctx) {
      IStorage* storage = StorageFactory().create();
      storage->readClientWiFiAP(ctx->client_ssid);
      storage->readClientWiFiPassword(ctx->client_password);

      if ((ctx->client_password[0] > 0) && (ctx->client_ssid[0] > 0)) {
        // to wifi client state
        return new FatalErrorState(); // @TODO IMPLEMENT!!!
      } else {
        return new StartWiFiServerState();
      }
    }
};

/**
 * Класс стартового состояния приложения
 */
class InitializedState : public IState {
  public:
    IState* next(Context *ctx) {
      return new LoadConfigurationState();
    }
};

/**
 * Объявление инициализирующего состояния
 */
IState* Context::init_state() {
    return new InitializedState();
}
