#ifndef STATES_H
#define STATES_H

class IState; // Форвардное объявление

/**
 * Класс общего для всех состояний контекста
 */
class Context {
  public:
    const char* server_sessid = "ServerWiFi-0001"; // Имя Wi-Fi по умолчанию
    const char* server_password = "tempPassNG"; // Пароль от Wi-Fi по умолчанию
    char client_ssid[32];
    char client_password[64];
    IState* init_state();
};

/**
 * Интерфейс для имплементации состояния
 */
class IState {
  public:
    virtual IState* next(Context *ctx) = 0;
};
#endif