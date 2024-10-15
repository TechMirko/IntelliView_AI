#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

/* dati della rete */
#define WIFI_SSID "****************"
#define WIFI_PASSWORD "******************"

/* altre configurazioni*/
#define SERVER_NAME "http://indirizzo.ip.server:portaServer/intelliView" // IMPORTANTE: cambiare solo ip address e non toccare porta ed endpoint
#define MQTT_SERVER "broker.emqx.io"
#define TOPIC_AI "tecnicamente/riconoscimento" // topic al quale arriva l'oggetto riconosciuto
#define TOPIC_START "tecnicamente/start" // topic dal quale arriva il segnale di start della procedura
#define MQTT_PORT 1883
#define FLASH 4
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

uint8_t riconosciuto = 0; // flag che va ad 1 quando l'oggetto è stato riconosciuto
uint8_t flag = 1; // variabile di flag per eseguire una volta sola il ciclo: 1 -> fermo, 0 -> inizia

String buffer;
WiFiClient net;
PubSubClient client(net);

void setup() {
  Serial.begin(115000);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // rimuove brownout detector
  
  // configurazione camera
  camera_config_t config;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  // gestione connessioni
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connessione in corso");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connesso, tutto pronto!");
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  reconnect();
  pinMode(FLASH, OUTPUT);
  digitalWrite(FLASH, HIGH);
  delay(150);
  digitalWrite(FLASH, LOW);
}

void loop() {
  if(flag == 0) {
    catturaEdInvia();
  }

  // connessione ed attivazione mqtt
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void catturaEdInvia() {  
  // Scatta una foto
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Invia la foto al server
  HTTPClient http;
  http.begin(SERVER_NAME);
  http.addHeader("Content-Type", "image/jpeg");
  int httpResponseCode = http.POST(fb->buf, fb->len);

  if(httpResponseCode>0){
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  
  // Libera la memoria
  esp_camera_fb_return(fb);
  http.end();
  flag++;
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Provando a connettersi ad MQTT...");
    // Attempt to connect
    String clientId = "ESP32Client";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connesso");
      client.subscribe(TOPIC_AI);
      client.subscribe(TOPIC_START);
    } else {
      Serial.print("fallito, rc=");
      Serial.print(client.state());
      }
  }
}

void callback(char* topic, uint8_t* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  
  // comando di start
  if(msg == "start") {
    flag = 0;
  }

  // oggetto riconosciuto
  if(msg != "start") { 
    Serial.print("Oggetto riconosciuto: "); Serial.println(msg);
  }    
}
