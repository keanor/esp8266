#include <ESP8266WiFi.h> // Библиотека для работы с WiFi
#include <EEPROM.h> // Библиотека для работы с FLASH
#include <ESP8266WebServer.h> // Библиотека для работы с веб сервером

// Байтовые представления ресурсов веб сервера
// Генерируются автоматически, см. ./pre_build.sh
#include "favicon.inl"
#include "skeleton.inl"
#include "index.html.inl"

/******************************************************/
/** Инициализация используемых переменных и констант **/
/******************************************************/

const char* INTERNAL_SESSID = "ServerWiFi-0001"; // Имя точки доступа Wi-Fi по умолчанию
const char* INTERNAL_PASSWORD = "tempPassNG"; // Пароль точки доступа от Wi-Fi по умолчанию
const size_t SESSID_LENGTH = 32;
const size_t PASSWORD_LENGTH = 64;
const size_t FLASH_SESSID_OFFSET = 0; // Адрес в flash памяти для имени клиентской сети
const size_t FLASH_PASSWORD_OFFSET = 32; // Адрес в flash памяти для клиентской сети
const int USED_FLASH_SIZE = 96; // Размер используемой под данные FLASH памяти

char sessid[SESSID_LENGTH] = {}; // Имя Wi-Fi клиентской сети
char password[PASSWORD_LENGTH] = {}; // Имя Wi-Fi клиентской сети

char buffer1024[1024] = {}; // Буффер общего назначения

/***********************************************/
/** Вспомогательные функции общего назначения **/
/***********************************************/

void clear_buffer(char* buffer, int length)
{
    for (int i=0; i<length; i++) 
    {
        buffer[i]=0;
    }
}

/***********************************************/
/** Объекты для работы с библиотеками esp8266 **/
/***********************************************/

ESP8266WebServer server(80);

/********************************/
/** Функции для работы с flash **/
/********************************/

void flash_read(const size_t offset, const size_t length, char* buffer) // запись во флэш
{
    EEPROM.begin(USED_FLASH_SIZE);
    for (size_t i = 0; i < length; i++) 
    {
        buffer[i] = EEPROM.read(i + offset);
    }
}

bool flash_write(const size_t offset, const size_t length, const char* buffer) // чтение из флэш
{
    EEPROM.begin(USED_FLASH_SIZE);
    for (size_t i = 0; i < length; i++) 
    {
        EEPROM.write(offset + i, buffer[i]);
    }

    return EEPROM.commit();
}

/*****************************************/
/** Функции для обработки http запросов **/
/*****************************************/

void web_index(void) // Главная страница
{
    server.setContentLength(resources_index_html_len);
    server.send_P(200, "text/html", (char*)resources_index_html, resources_index_html_len);
}

void web_favicon(void) // Инконка web сайта
{
    server.setContentLength(resources_favicon_png_len);
    server.send_P(200, "image/png", (char*)resources_favicon_png, resources_favicon_png_len);
}

void web_stylesheet(void) // Таблица стилей
{
    server.setContentLength(resources_skeleton_min_css_gz_len);
    server.sendHeader("Content-Encoding", "gzip");
    server.send_P(200, "text/css", (char*)resources_skeleton_min_css_gz, resources_skeleton_min_css_gz_len);
}

void web_aplist(void) // Список WiFi сетей
{
    int n = WiFi.scanNetworks();    
    clear_buffer(buffer1024, 1024);

    for (int i = 0; i < n; i++) 
    {
        strcat(buffer1024, WiFi.SSID(i).c_str());
        if (n - i > 1) 
        {
            strcat(buffer1024, ",");
        }
    }

    server.send(200, "text/html", buffer1024);
}

void web_save(void) // Сохранение настроек
{
    if( ! server.hasArg("ssid") || // Валидация
        ! server.hasArg("pass") || 
        server.arg("ssid") == NULL || 
        server.arg("pass") == NULL ||
        server.arg("ssid").length() > SESSID_LENGTH ||
        server.arg("pass").length() > PASSWORD_LENGTH)
    {
        server.send(200, "text/plain", "error");
    }
    else
    {
        // Сохраняем sessid
        if (!flash_write(FLASH_SESSID_OFFSET, 
            server.arg("ssid").length(), 
            server.arg("ssid").c_str())) 
        {
            server.send(200, "text/plain", "error");
            return;
        }

        // Сохраняем pass
        if (!flash_write(FLASH_PASSWORD_OFFSET, 
            server.arg("pass").length(), 
            server.arg("pass").c_str())) 
        {
            server.send(200, "text/plain", "error");
            return;
        }

        server.send(200, "text/plain", "success");
        delay(200); // На всякий случай
        ESP.restart(); // Перезагружаем устройство
    }
}

/******************************/
/** Инициализация устройства **/
/******************************/
void setup(void) 
{
    // Считываем из flash данные о клиентской сети
    flash_read(FLASH_SESSID_OFFSET, SESSID_LENGTH, sessid);
    flash_read(FLASH_PASSWORD_OFFSET, PASSWORD_LENGTH, password);

    // Подключаемся к клиентской вай фай сети
    bool need_start_ap = true; //Нужно ли поднимать точку доступа 
    if (sessid[0] == 0 || password[0] == 0)
    {
        int check_status_count = 0;
        WiFi.begin(sessid, password);
        do // Ждем результатов подключения
        {
            int status = WiFi.status();
            if (status == WL_CONNECTED) // Успешно подключились
            {
                need_start_ap = false;
                break;
            }
            else if (status == WL_CONNECT_FAILED) // Ошибка подключения
            {
                break;
            }
            else 
            {
                // Ждем результатов примерно 10 секунд
                if (check_status_count == 99) 
                {
                    break;
                }
                else
                {
                    check_status_count++;
                    delay(100);
                }
            }
        }
        while (true);
    }

    // Поднимаем собственную Wi-Fi точку доступа
    if (need_start_ap)
    {
        WiFi.softAP(INTERNAL_SESSID, INTERNAL_PASSWORD);
    }

    // Настраиваем и запускаем web сервер
    server.on("/", web_index);
    server.on("/favicon.png", web_favicon);
    server.on("/style.css", web_stylesheet);
    server.on("/aplist", web_aplist);
    server.on("/save", web_save);
    server.begin();
}

void loop() // Цикл работы устройства
{
    server.handleClient();
}
