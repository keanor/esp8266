#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include "webserver.h"
#include "favicon.inl"
#include "skeleton.inl"
#include "index.html.inl"

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

        // @TODO список точек доступа по /aplist
};

IWebServer* WebServerFactory::create() {
    return new WebServer();
}
