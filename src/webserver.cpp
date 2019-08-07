#include <ESP8266WebServer.h>
#include "webserver.h"
#include "favicon.inl"
#include "skeleton.inl"

class WebServer : public IWebServer {
    public:
        WebServer() {
            server = new ESP8266WebServer(80);
        }

        void start() {
            server->on("/", std::bind(&WebServer::main, this));
            server->on("/favicon.png", std::bind(&WebServer::favicon, this));
            server->on("/style.css", std::bind(&WebServer::stylesheet, this));

            server->begin();
            Serial.println("Web server started on port 80");
        }

        void handle() {
            server->handleClient();
        }
    private:
        ESP8266WebServer* server;

        void main(void) {
            const char* html =
            "<html>"
                "<head>"
                    "<link rel=\"stylesheet\" href=\"/style.css\" />"
                    "<link rel=\"shortcut icon\" href=\"/favicon.png\" type=\"image/png\">"
                    "<title>Панель управления умным домом</title>"
                "</head>"
                "<body>"
                    
                "</body>"
            "</html>";
            server->send_P(200, "text/html", "<h1>TEST</h1>");
        }

        void favicon(void) {
            server->send(200, "images/png", String((char*)&resources_favicon_png));
        }

        void stylesheet(void) {
            server->sendHeader("Content-Encoding", "gzip");
            server->send(200, "text/css", String((char*)&resources_skeleton_min_css_tar_gz));
        }
};

IWebServer* WebServerFactory::create() {
    return new WebServer();
}
