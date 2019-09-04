#include <DHT.h>
#include <Ethernet.h>
#include <SPI.h>
#include <UbidotsEthernet.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define TOKEN "YOUR-UBIDOTS-API-TOKEN"
#define VARIABLE_LABEL_1 "TEMPERATURE"
#define VARIABLE_LABEL_2 "HUMIDITY"
#define VARIABLE_LABEL_3 "THERMAL-SENSATION"
#define DEVICE_LABEL "DEVICE-LABEL"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
unsigned long clock_time = 0;

DHT dht(DHTPIN, DHTTYPE);
Ubidots client(TOKEN);
IPAddress ip(192, 168, 1, 40); //static ip

void setup() {
  Serial.begin(9600);
  dht.begin();
  client.setDeviceLabel(DEVICE_LABEL);
  //client.setDebug(true);// uncomment this line to visualize the debug message
  /* start the Ethernet connection */
  Serial.print(F("Starting ethernet..."));
  if (!Ethernet.begin(mac)) {
    Serial.println(F("failed"));
    Serial.println(F("Trying to connect with static ip"));
    Ethernet.begin(mac, ip);
  } else {
    Serial.println(Ethernet.localIP());
  }
  /* Give the Ethernet shield a second to initialize */
  delay(2000);
  Serial.println(F("Ready"));
}

void loop() {
  Ethernet.maintain();
  if ((millis() - clock_time) >= 300000) {
    /* Sensors readings */
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
  
    float st = dht.computeHeatIndex(t, h, false);
    
    client.add(VARIABLE_LABEL_1, t);
    client.add(VARIABLE_LABEL_2, h);
    client.add(VARIABLE_LABEL_3, st);
    client.sendAll();

    Serial.println(F("Data Sent!"));
    clock_time = millis();
  }

  delay(1000);
}
