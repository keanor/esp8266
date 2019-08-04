#ifndef STORAGE_H
#define STORAGE_H

/**
 * Класс реализующий хранение данных
 */
class IStorage {
    public:
        // Читаем название точки доступа (для подключения)
        virtual void readClientWiFiAP(char* dest) = 0;
        // Читаем пароль от точки доступа (для подключения)
        virtual void readClientWiFiPassword(char* dest) = 0;
        // Пишем название точки доступа (для подключения)
        virtual void writeClientWiFiAP(char* data) = 0;
        // Пишем пароль от точки доступа (для подключения)
        virtual void writeClientWiFiPassword(char* data) = 0;
};

class StorageFactory {
    public:
        IStorage* create();
};
#endif
