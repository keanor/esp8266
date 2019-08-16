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
        void writeClientWiFiAP(char* data) {
            write(CLIENT_WIFI_AP_OFFSET, CLIENT_WIFI_AP_LENGTH, data);
        }

        // Пишем пароль от точки доступа (для подключения)
        void writeClientWiFiPassword(char* data) {
            write(CLIENT_WIFI_PASS_OFFSET, CLIENT_WIFI_PASS_LENGTH, data);
        }

        // Читаем кол-во логов на устройстве
        int readLogCount() {
            char buffer[sizeof(int)];
            read(LOG_COUNT_OFFSET, LOG_COUNT_LENGTH, buffer);
            int count = reinterpret_cast<int>(buffer);
            return count;
        }

        void writeLog(char* message, const size_t length) {
            int logCount = readLogCount();
            if (logCount == 100) {
                logCount = 0;
            }
            size_t start = LOG_DATA_LENGTH * logCount + LOG_DATA_FIRST_OFFSET;
            if (length > LOG_DATA_LENGTH) {
                write(start, LOG_DATA_LENGTH, message);
            } else {
                write(start, length, message);
            }

            logCount++;
            char buf[sizeof(int)] = {};
            memcpy(buf, &logCount, sizeof(int));
            write(LOG_COUNT_OFFSET, LOG_COUNT_LENGTH, buf);
        }

        void readLog(int index, char* dest) {
            size_t start = LOG_DATA_LENGTH * index + LOG_DATA_FIRST_OFFSET;
            read(start, LOG_DATA_LENGTH, dest);
        }
    private:
        const size_t CLIENT_WIFI_AP_OFFSET = 0;
        const size_t CLIENT_WIFI_AP_LENGTH = 32;
        
        const size_t CLIENT_WIFI_PASS_OFFSET = CLIENT_WIFI_AP_OFFSET + CLIENT_WIFI_AP_LENGTH;
        const size_t CLIENT_WIFI_PASS_LENGTH = 64;

        const size_t LOG_COUNT_OFFSET = CLIENT_WIFI_PASS_OFFSET + CLIENT_WIFI_PASS_LENGTH;
        const size_t LOG_COUNT_LENGTH = sizeof(int);

        const size_t LOG_DATA_FIRST_OFFSET = LOG_COUNT_OFFSET + LOG_COUNT_LENGTH;
        const size_t LOG_DATA_LENGTH = 128;
        
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
