#include "storage.h"
#ifndef WEBSERVER_H
#define WEBSERVER_H

/**
 * Класс реализующий веб сервер
 */
class IWebServer {
    public:
        // Читаем название точки доступа (для подключения)
        virtual void start() = 0;
        virtual void handle() = 0;
};

class WebServerFactory {
    public:
        IWebServer* create(IStorage* storage);
};
#endif
