#include <Arduino.h>
#include <WiFi.h>
#include <Firebase.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include "DHTesp.h"
#include <Servo.h>

#define WIFI_SSID "aneh"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyCdhp668BwAYNHbn8zHmWz2VqEzvL94Lgk"
#define DATABASE_URL "https://flutterdemoapp-5b9c9-default-rtdb.asia-southeast1.firebasedatabase.app/"

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
#define LampuShow2 32  // Lampu Showing Panel
#define ServoPin2 26   // Door
#define ServoPin3 25   // Door
#define Doorlock 33    // Solenoid

#define DHTTYPE DHTesp::DHT11
void WifiConnect();
void sendDataDHT(void *parameter);

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
DHTesp dht;
Servo servo;
Servo Servo2;

bool ledState0 = false;
bool ledState1 = false;
bool ledState2 = false;
bool ledState3 = false;
bool ledState4 = false;
bool ledStateShwopan1 = false;
bool ledStateShowpan2 = false;
bool ACState = false;
bool doorLock = false;
bool doorLock2 = false;
bool solenoid = false;

void setup()
{
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
    pinMode(solenoid, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    servo.attach(ServoPin2);
    Servo2.attach(ServoPin3);

    Serial.begin(115200);
    WifiConnect();
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    auth.user.email = "test@gmail.com";
    auth.user.password = "testtest";

    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);


    xTaskCreate(sendDataDHT, "sendDataDHT", 4096, NULL, 1, NULL);
}

void loop()
{
    if(Firebase.ready()){
    // Ruangan 1
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 1/Light"))
    {
        ledState0 = fbdo.boolData();
        digitalWrite(LampuMaket1, !ledState0);
    }
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 1/Door"))
    {
        doorLock = fbdo.boolData();
        if (doorLock)
        {
            servo.write(180);
        }
        else
        {
            servo.write(0);
        }
    }

    // Ruangan 2
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 2/Light"))
    {
        ledState2 = fbdo.boolData();
        digitalWrite(LampuMaket2, !ledState2);
    }
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 2/Door"))
    {
        doorLock2 = fbdo.boolData();
        if (doorLock2)
        {
            servo.write(180);
        }
        else
        {
            servo.write(0);
        }
    }
    // Ruangan 3
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 3/Light"))
    {
        ledState3 = fbdo.boolData();
        digitalWrite(LampuMaket3, !ledState3);
    }
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 3/Door"))
    {
        doorLock = fbdo.boolData();
        if (doorLock)
        {
            Servo2.write(180);
        }
        else
        {
            Servo2.write(0);
        }
    }
    // Ruangan 4
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 4/Light"))
    {
        ledState4 = fbdo.boolData();
        digitalWrite(LampuMaket4, !ledState4);
    }
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 4/Solenoid"))
    {
        solenoid = fbdo.boolData();
        digitalWrite(Doorlock, !solenoid);
    }
    // Showing Panel
    if (Firebase.RTDB.getBool(&fbdo, "Air Conditioner/Switch"))
    {
        ACState = fbdo.boolData();
        digitalWrite(AC, !ACState);
    }
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 1/Light")){
        ledStateShwopan1 = fbdo.boolData();
        digitalWrite(LampuShow1, !ledStateShwopan1);
        Serial.println(ledStateShwopan1);
    }
    if (Firebase.RTDB.getBool(&fbdo, "All Devices/Ruangan 2/Light")){
        ledStateShowpan2 = fbdo.boolData();
        digitalWrite(LampuShow2, !ledStateShowpan2);
    }
}
}

void WifiConnect()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi...");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(10);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("Connected to ");
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void sendDataDHT(void *parameter){
    for (;;)
    {
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    Firebase.RTDB.setFloat(&fbdo, "/humidity", humidity);
    Firebase.RTDB.setFloat(&fbdo, "/temperature", temperature);
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C ");

    //delay
    vTaskDelay(20000/portTICK_PERIOD_MS);
    }
}