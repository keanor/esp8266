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
                    "<meta charset=\"UTF-8\">"
                    "<link rel=\"stylesheet\" href=\"/style.css\" />"
                    "<link rel=\"shortcut icon\" href=\"/favicon.png\" type=\"image/png\">"
                    "<title>Панель управления умным домом</title>"
                "</head>"
                "<body>"
                    "<h1>Web server работает</h1>"
                    "<p><a href=\"/favicon.png\">Иконка</a></p>"
                    "<p><a href=\"/style.css\">Стили</a></p>"
                "</body>"
            "</html>";
            server->send(200, "text/html", html);
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
};

IWebServer* WebServerFactory::create() {
    return new WebServer();
}
