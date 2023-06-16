#include <Arduino.h>
#include <WiFi.h>
#include <Firebase.h>
#include <credentials.h>
#include <DHTesp.h>
#include <Servo.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

FirebaseData stream;
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;



#define DHTPIN 19
#define LampuMaket1 18 // Lampu1
#define LampuMaket2 5  // Lampu2
#define LampuMaket3 17 // Lampu Maket
#define AC 14          // AC
#define LampuMaket4 16 // Lampu Maket
#define LampuShow1 27  // Lampu Showing Panel
#define LampuShow2 22 // Lampu Showing Panel
#define ServoPin2 26   // Door
#define ServoPin3 25   // Door
#define Doorlock 21    // Solenoid

#define DHTTYPE DHTesp::DHT11
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
DHTesp dht;
Servo servo;
Servo Servo2;


String parentPath = "/";
String childPath[9] = {"All Devices/Ruangan 1/Light", "All Devices/Ruangan 1/Door", "All Devices/Ruangan 2/Light", "All Devices/Ruangan 2/Door", "All Devices/Ruangan 3/Light", "All Devices/Ruangan 3/Door", "All Devices/Ruangan 4/Light", "All Devices/Ruangan 4/Door" ,"Air Conditioner/Switch"};


//--------- FIREBASE CALLBACKS ---------//


void streamCallback(MultiPathStreamData stream) {
  size_t numChild = sizeof(childPath) / sizeof(childPath[0]);
  Serial.println("masok");

  for (size_t i = 0; i < numChild; i++) {
    Serial.println("masok loop");
    Serial.println(childPath[i]);
    if (stream.get(childPath[i])) {
      Serial.printf("path: %s, event: %s, type: %s, value: %s%s", stream.dataPath.c_str(), stream.eventType.c_str(), stream.type.c_str(), stream.value.c_str(), i < numChild - 1 ? "\n" : "");
      Serial.println("masok if");

      const char* childPath = stream.dataPath.c_str();
      String value = stream.value.c_str();

      // Perform different actions based on the child path
      if (strcmp(childPath, "/All Devices/Ruangan 1/Light") == 0) {
        Serial.println("masok if 1");
        Serial.println(value);
        if (value != "true") {
            digitalWrite(LampuMaket1, HIGH);
            digitalWrite(LampuShow1,HIGH);
           

        } else {
          digitalWrite(LampuMaket1, LOW);
          digitalWrite(LampuShow1,LOW);

        }
      } else if (strcmp(childPath, "/All Devices/Ruangan 1/Door") == 0) {
        Serial.println("masok if 2");

        if (value == "true") {
          servo.write(90);
        } else {
          servo.write(0);
        }
      }else if(strcmp(childPath, "/All Devices/Ruangan 2/Light") == 0){
        Serial.println("masok if 3");

        if (value != "true") {
          digitalWrite(LampuMaket2, HIGH);
          digitalWrite(LampuShow2, HIGH);
        } else {
          digitalWrite(LampuMaket2, LOW);
          digitalWrite(LampuShow2, LOW);
        }
        }else if(strcmp(childPath, "/All Devices/Ruangan 2/Door") == 0){
        Serial.println("masok if 4");
        if (value == "true"){
            Servo2.write(180);
        }else{
            Servo2.write(0);
        }
        }else if(strcmp(childPath, "/All Devices/Ruangan 3/Light") == 0){
        Serial.println("masok if 5");
        if (value != "true"){
            digitalWrite(LampuMaket3, HIGH);
        }else{
            digitalWrite(LampuMaket3, LOW);
        }
        }else if(strcmp(childPath, "/All Devices/Ruangan 3/Door") == 0){
        Serial.println("masok if 6");
        if (value != "true"){
            digitalWrite(Doorlock, HIGH);
        }else{
            digitalWrite(Doorlock, LOW);
        }
        }else if(strcmp(childPath, "/All Devices/Ruangan 4/Light") == 0){
        Serial.println("masok if 7");
        if (value != "true"){
            digitalWrite(LampuMaket4, HIGH);
        }else{
            digitalWrite(LampuMaket4, LOW);
        }
        }
        else if(strcmp(childPath, "/All Devices/Ruangan 4/Door") == 0){
        Serial.println("masok if 8");
        if (value == "true"){
            Servo2.write(90);
        }else{
            Servo2.write(0);
        }
        }
        else if(strcmp(childPath, "/Air Conditioner/Switch") == 0){
        Serial.println("masok if 9");
        if (value != "true"){
            digitalWrite(AC, HIGH);
        }else{
            digitalWrite(AC, LOW);
        }        
    } else {
      Serial.println("............");
    }
  }

  Serial.println();
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", stream.payloadLength(), stream.maxPayloadLength());
}
}

void streamTimeoutCallback(bool timeout) {
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void setup() {
  Serial.begin(115200);

  dht.setup(DHTPIN, DHTTYPE);
  pinMode(LampuShow1, OUTPUT);
  pinMode(LampuShow2, OUTPUT);
  pinMode(LampuMaket3, OUTPUT);
  pinMode(LampuMaket1, OUTPUT);
  pinMode(LampuMaket2, OUTPUT);
  pinMode(AC, OUTPUT);
  pinMode(LampuMaket4, OUTPUT);
  pinMode(ServoPin2, OUTPUT);
  pinMode(ServoPin3, OUTPUT);
  pinMode(Doorlock, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  servo.attach(ServoPin2);
  Servo2.attach(ServoPin3);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);
  
  if (!Firebase.beginMultiPathStream(stream, parentPath))
    Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setMultiPathStreamCallback(stream, streamCallback, streamTimeoutCallback);

}

void loop() {
     unsigned long sendDataCurrentMillis = millis();
  if (sendDataCurrentMillis - sendDataPrevMillis > 20000) {
    sendDataPrevMillis = sendDataCurrentMillis;
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    Firebase.RTDB.setFloat(&fbdo, "/humidity", humidity);
    Firebase.RTDB.setFloat(&fbdo, "/temperature", temperature);
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C ");
  
  if (Firebase.ready()) {

  }
  // Serial.println("masuk case 1");
}
}