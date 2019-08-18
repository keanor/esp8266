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
 * = sizeof(int) кол-во логов      =
 * =================================
 * = Записи логов по 128 байт      =
 * =================================
 * 
 * Логов может быть максимум 100 записей ()
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
        bool writeClientWiFiAP(char* data) {
            return write(CLIENT_WIFI_AP_OFFSET, CLIENT_WIFI_AP_LENGTH, data);
        }

        // Пишем пароль от точки доступа (для подключения)
        bool writeClientWiFiPassword(char* data) {
            return write(CLIENT_WIFI_PASS_OFFSET, CLIENT_WIFI_PASS_LENGTH, data);
        }
    private:
        const size_t CLIENT_WIFI_AP_OFFSET = 0;
        const size_t CLIENT_WIFI_AP_LENGTH = 32;
        
        const size_t CLIENT_WIFI_PASS_OFFSET = CLIENT_WIFI_AP_OFFSET + CLIENT_WIFI_AP_LENGTH;
        const size_t CLIENT_WIFI_PASS_LENGTH = 64;

        const size_t LOG_COUNT_OFFSET = CLIENT_WIFI_PASS_OFFSET + CLIENT_WIFI_PASS_LENGTH;
        const size_t LOG_COUNT_LENGTH = sizeof(int);
        
        void read(size_t start, size_t length, char* buffer) {
            EEPROM.begin(512);
            for (size_t i = 0; i < length; i++) {
                buffer[i] = EEPROM.read(i + start);
            }
        }
        
        bool write(size_t start, size_t length, char* buffer) {
            EEPROM.begin(512);
            for (size_t i = 0; i < length; i++) {
                EEPROM.write(start + i, buffer[i]);
            }
            return EEPROM.commit();
        }
};

IStorage* StorageFactory::create() {
    return new FlashStorage();
}
