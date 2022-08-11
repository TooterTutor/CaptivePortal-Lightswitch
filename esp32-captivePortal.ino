// Adding the neccessary libraries to work properly
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>

// SPI Flash File System libraries for uploading html files to ESP32
#include <FS.h>
#include <SPIFFS.h>

// Serial pins needed to transmit the received commands from SPIFFS to Arduino Uno
#define RXp2 16
#define TXp2 17


DNSServer dnsServer;
AsyncWebServer server(80);

// Network credentials used to create the local WiFi network:
// Network's SSID and the captive portal's password.
const char ssid[] = "ESP32-Access-Point";
const char password[] = "Some-Dumb-Old-P4ssword!";



/**
 * @brief List SPIFFS directory in Serial Monitor for storage mounting.
 *
 * @param fs File system handle.
 * @param dirname Directory name.
 * @param levels Maximum number of subdirectories to recurse into.
 */
void listDir(fs::FS &fs, const char* dirname, uint8_t levels)
{
   Serial.printf("Listing directory: %s\r\n", dirname);

   File root = fs.open(dirname);
   if (!root)
   {
      Serial.println("− failed to open directory");
      return;
   }
   if (!root.isDirectory())
   {
      Serial.println(" − not a directory");
      return;
   }

   File file = root.openNextFile();
   while (file)
   {
      if (file.isDirectory())
      {
         Serial.print("  DIR : ");
         Serial.println(file.name());
         if (levels)
         {
            listDir(fs, file.name(), levels - 1);
         }
      }
      else
      {
         Serial.print("  FILE: ");
         Serial.print(file.name());
         Serial.print("\tSIZE: ");
         Serial.println(file.size());
      }
      file = root.openNextFile();
   }
}

/**
 * @brief Captive Request Handler for Captive Portal.
 *
 * Main calls for all files go in this class.
 */
class CaptiveRequestHandler : public AsyncWebHandler
{
public:
   CaptiveRequestHandler();

   virtual ~CaptiveRequestHandler() = default;

   /**
    * @brief Is this class able to handle asynchronous requests?
    *
    * @param request Asynchronous request to check whether we can process it.
    * @return true The client will automatically download the page.
    * @return false The client must manually download the page.
    */
   bool canHandle(AsyncWebServerRequest* request);

   /**
    * @brief When a client connects, this request should send them the main page.
    * @param request Asynchronous request to process.
    */
   void handleRequest(AsyncWebServerRequest* request);
};

CaptiveRequestHandler::CaptiveRequestHandler()
{
   // Defines all of the main HTML pages that will be returned as one of the async responses.

   // Provide a 404 page in case a page can't be found.
   server.onNotFound([] (AsyncWebServerRequest* request)
   {
      // This file is intentionally missing.
      AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/notFound.html", "text/html");
      request->send(response);
   });

   // The .data/ directory is the SPIFFS server root, and contains the following files.

   server.on("/", HTTP_GET, [] (AsyncWebServerRequest* request)
   {
      AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/index.html", "text/html");
      request->send(response);
      Serial.println("Client Connected and Requested Main Page");
   });
   server.on("/lightswitch.html", HTTP_GET, [] (AsyncWebServerRequest* request)
   {
      AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/lightswitch.html", "text/html");
      request->send(response);
      Serial.println("Client Requested Lightswitch Page");
   });
   server.on("/whistle.html", HTTP_GET, [] (AsyncWebServerRequest* request)
   {
      AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/whistle.html", "text/html");
      request->send(response);
      Serial.println("Client Requested Whistle Page");
   });

   server.on("/stereo.html", HTTP_GET, [] (AsyncWebServerRequest* request)
   {
      AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/stereo.html", "text/html");
      request->send(response);
      Serial.println("Client Requested Stereo Page");
   });

   // Defining all of the CSS and PNG files

   server.on("/web.css", HTTP_GET, [] (AsyncWebServerRequest* request)
   {
      AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/web.css", "text/css");
      request->send(response);
   });

   server.on("/layout.css", HTTP_GET, [] (AsyncWebServerRequest* request)
   {
      AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/layout.css", "text/css");
      request->send(response);
   });

   server.on("/header-line.png", HTTP_GET, [] (AsyncWebServerRequest* request)
   {
      AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/header-line.png", "image/png");
      request->send(response);
   });
}

bool CaptiveRequestHandler::canHandle(AsyncWebServerRequest* request)
{
   return true;
}

void CaptiveRequestHandler::handleRequest(AsyncWebServerRequest* request)
{
   // When someone connects (client), which page should they be given (index.html), and should it be downloaded (false)
   request->send(SPIFFS, "/index.html", String(), false);
}

/**
 * @brief Defines the actions that the buttons on the various pages perform when clicked.
 *
 * When a button is pressed, send a command through Serial2 to the listening Arduino Uno.
 * Example: Flip the lightswitch on or off, or create a shrill whistling sound.
 */
void setupServer()
{
   // Handle when one of the Lightswitch buttons is pressed.

   server.on("/on",      HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("ON"); });
   server.on("/off",     HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("OFF"); });
   server.on("/lock",    HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("LOCK"); });
   server.on("/unlock",  HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("UNLOCK"); });
   server.on("/rlock",   HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("LOCK REMOTE"); });
   server.on("/runlock", HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("UNLOCK REMOTE"); });
   server.on("/hlock",   HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("HIDDEN LOCK"); });
   server.on("/hunlock", HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("HIDDEN UNLOCK"); });
   server.on("/alock",   HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("LOCK ALL"); });
   server.on("/aunlock", HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("UNLOCK ALL"); });

   // Handle when one of the Whistle buttons is pressed.

   server.on("/TooterTutor",  HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("TooterTutor"); });
   server.on("/AndOrButt",    HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("AndOrButt"); });
   server.on("/DieSpiderDie", HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("DieSpiderDie"); });
   server.on("/StompGoblin",  HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("StompGoblin"); });

   // Handle when one of the Stereo buttons is pressed.
   // Transmits a series of IR information to an IR-controlled Stereo System.
   // Still a work in progress!

   server.on("/stereo_power",   HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_power"); });
   server.on("/stereo_cd",      HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_cd"); });
   server.on("/stereo_mute",    HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_mute"); });
   server.on("/stereo_aux",     HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_aux"); });
   server.on("/stereo_bt",      HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_bt"); });
   server.on("/stereo_fm",      HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_fm"); });
   server.on("/stereo_rr",      HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_rr"); });
   server.on("/stereo_play",    HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_play"); });
   server.on("/stereo_ff",      HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_ff"); });
   server.on("/stereo_rpt",     HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_rpt"); });
   server.on("/stereo_stop",    HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_stop"); });
   server.on("/stereo_prog",    HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_prog"); });
   server.on("/stereo_voldown", HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_voldown"); });
   server.on("/stereo_bass",    HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_bass"); });
   server.on("/stereo_volup",   HTTP_GET, [] (AsyncWebServerRequest* request) { Serial2.write("stereo_volup"); });
}

/**
 * @brief Set up the Serial and Serial2 loggers, as well as SPIFFS and so forth.
 */
void setup()
{
   // Start Both Serial (Serial Monitor) and Serial2 (Communication to Arduino Uno)
   Serial.begin(115200);
   Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

   // Start with IR_SEND_PIN as send pin and if NO_LED_FEEDBACK_CODE is NOT defined, enable feedback LED at default feedback LED pin
   //IrSender.begin();

   // If SPIFFS has not mounted, throw an error in the Serial Monitor.
   if (!SPIFFS.begin(true))
   {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
   }

   // List contents of SPIFFS in the Serial Monitor.
   listDir(SPIFFS, "/", 0);

   // Start Access Point Mode with ssid and password defined earlier and print info
   Serial.println();
   Serial.println("Setting up AP Mode");
   WiFi.mode(WIFI_AP);
   WiFi.softAP(ssid, password);

   auto wifi_ip_address = WiFi.softAPIP();

   // Print Access Point IP address and then set up Async Webserver
   Serial.print("AP IP address: ");
   Serial.println(wifi_ip_address);

   Serial.println("Setting up Async WebServer");
   setupServer();

   // Start DNS server and add the CaptiveRequestHandler that was defined earlier
   Serial.println("Starting DNS Server");
   dnsServer.start(53, "*", wifi_ip_address);

   // Only when requested from Access Point
   server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);

   // Start the server
   server.begin();
   Serial.println("All Done!");
}

void loop()
{
   dnsServer.processNextRequest();
}
