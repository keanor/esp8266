#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include "webserver.h"
#include "storage.h"
#include "favicon.inl"
#include "skeleton.inl"
#include "index.html.inl"

class WebServer : public IWebServer {
    public:
        WebServer(IStorage* storage) {
            server = new ESP8266WebServer(80);
            this->storage = storage;
        }

        void start() {
            server->on("/", std::bind(&WebServer::main, this));
            server->on("/favicon.png", std::bind(&WebServer::favicon, this));
            server->on("/style.css", std::bind(&WebServer::stylesheet, this));
            server->on("/aplist", std::bind(&WebServer::aplist, this));
            server->on("/save", std::bind(&WebServer::save, this));

            server->begin();
            Serial.println("Web server started on port 80");
        }

        void handle() {
            server->handleClient();
        }
    private:
        ESP8266WebServer* server;
        IStorage* storage;

        void main(void) {
            server->setContentLength(resources_index_html_len);
            server->send_P(
                200, 
                "text/html", 
                (char*)resources_index_html, 
                resources_index_html_len
            );
        }

        void favicon(void) {
            server->setContentLength(resources_favicon_png_len);
            server->send_P(
                200, 
                "image/png", 
                (char*)resources_favicon_png,
                resources_favicon_png_len
            );
        }

        void stylesheet(void) {
            server->setContentLength(resources_skeleton_min_css_tar_gz_len);
            server->sendHeader("Content-Encoding", "gzip");
            server->send_P(
                200, 
                "text/css", 
                (char*)resources_skeleton_min_css_tar_gz, 
                resources_skeleton_min_css_tar_gz_len
            );
        }

        void aplist(void) {
            int n = WiFi.scanNetworks();
            String content = "";
            if (n > 0) {
                for (int i = 0; i < n; i++) {
                    content += WiFi.SSID(i);
                    if (n - i > 1) {
                        content += ",";
                    }
                }
            }
            server->send(200, "text/html", content);
        }

        void save(void) {
            // c3Ya6PN796
            if( ! server->hasArg("ssid") || 
                ! server->hasArg("pass") || 
                server->arg("ssid") == NULL || 
                server->arg("pass") == NULL) 
            {
                String message = "POST\nHeaders:\n";
                for (uint8_t i = 0; i < server->headers(); i++) {
                    message += server->headerName(i) + ": " + server->header(i) + "\n";
                }
                message += "\nRequest params:\n";
                for (uint8_t i = 0; i < server->args(); i++) {
                    message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
                }
                server->send(200, "text/plain", message);
            } else {
                String ssid_str = server->arg("ssid");
                char ssid_arr[ssid_str.length()];
                memcpy(ssid_arr, ssid_str.c_str(), ssid_str.length());
                storage->writeClientWiFiAP(ssid_arr);

                String pass_str = server->arg("pass");
                char pass_arr[pass_str.length()];
                memcpy(pass_arr, pass_str.c_str(), pass_str.length());
                storage->writeClientWiFiAP(pass_arr);

                server->send(200, "text/plain", "success");

                Serial.println("Configuration saved, restart hardware");
                delay(200);
                ESP.restart();
            }
        }
};

IWebServer* WebServerFactory::create(IStorage* storage) {
    return new WebServer(storage);
}
