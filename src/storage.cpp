#include <EEPROM.h>
#include "storage.h"

/*
 * Данные мы сохраняем на flash чипе
 * 
 * поскольку у нас ессть только побайтовый read write мы заранее
 * понимая какие данные нам нужно хранить, принимаем решение о их
 * порядке и читаем пишем их по определенному оффсету
 * 
 * Текущая карта:
 * =================================
 * = 32 байта Имя домшней AP       =
 * =================================
 * = 64 байта Логин от домашней AP =
 * =================================
 *  */
class FlashStorage : public IStorage {
    public:
        // Читаем название точки доступа (для подключения)
        void readClientWiFiAP(char* dest) {
            read(CLIENT_WIFI_AP_OFFSET, CLIENT_WIFI_AP_LENGTH, dest);
        }

        // Читаем пароль от точки доступа (для подключения)
        void readClientWiFiPassword(char* dest) {
            read(CLIENT_WIFI_PASS_OFFSET, CLIENT_WIFI_PASS_LENGTH, dest);
        }

        // Пишем название точки доступа (для подключения)
        void writeClientWiFiAP(char* data) {
            write(CLIENT_WIFI_AP_OFFSET, CLIENT_WIFI_AP_LENGTH, data);
        }

        // Пишем пароль от точки доступа (для подключения)
        void writeClientWiFiPassword(char* data) {
            write(CLIENT_WIFI_PASS_OFFSET, CLIENT_WIFI_PASS_LENGTH, data);
        }
    private:
        const size_t CLIENT_WIFI_AP_OFFSET = 0;
        const size_t CLIENT_WIFI_AP_LENGTH = 32;
        
        const size_t CLIENT_WIFI_PASS_OFFSET = 32;
        const size_t CLIENT_WIFI_PASS_LENGTH = 64;
        
        void read(size_t start, size_t length, char* buffer) {
            for (size_t i = 0; i < length; i++) {
                buffer[i] = EEPROM.read(i + start);
            }
        }
        
        void write(size_t start, size_t length, char* buffer) {
            for (size_t i = 0; i < length; i++) {
                EEPROM.write(start + i, buffer[i]);
            }
        }
};

IStorage* StorageFactory::create() {
    return new FlashStorage();
}
