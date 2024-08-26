#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <Keypad.h>
#include <LittleFS.h>
#include <SD.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Update.h>
#include <WiFiClientSecure.h> // For SSL/TLS

// Pin Definitions
#define GAS_SENSOR_PIN 34 // Use a valid ADC pin on ESP32
#define BUZZER_PIN 5
#define DOOR_SERVO_PIN 16 
#define WINDOW_SERVO_PIN 19
#define RAIN_SENSOR_PIN 21 // Digital pin
#define IR_SENSOR_PIN 15 // Digital pin
#define LED_PIN 2
#define LDR_PIN 35 // Analog pin
#define LDR_LED_PIN 4

// LCD Configuration
#define SDA_PIN 22
#define SCL_PIN 23
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad Configuration
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 18, 27}; // adjust as needed
byte colPins[COLS] = {26, 25, 33, 32}; // adjust as needed
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String password = "1234"; // Set your password
String inputPassword = "";
int wrongPasswordAttempts = 0;

// MQTT Configuration
const char* mqtt_server = "b048343a55e54f71bce3cbced89c1555.s1.eu.hivemq.cloud";
const int mqtt_port = 8883; // Secure port for MQTT
const char* mqtt_user = "marihanemad2003";
const char* mqtt_password = "PE2seLECydJ@iGG";

// Root CA Certificate for SSL/TLS
static const char* ca_cert PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure espClient; // Use WiFiClientSecure for SSL/TLS
PubSubClient client(espClient);

Servo doorServo;
Servo windowServo;

void setup_wifi() {
  WiFi.begin("Shakmagiya2", "202424202"); // Replace with your Wi-Fi credentials
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Subscribe to the topic for controlling devices
      client.subscribe("home/control/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void activateBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  if (String(topic) == "home/control/door") {
    if (message == "OPEN") {
      doorServo.write(160); // Open the door
      client.publish("home/control/door/status", "OPEN"); // Publish door status
    } else if (message == "CLOSE") {
      doorServo.write(0); // Close the door
      client.publish("home/control/door/status", "CLOSE"); // Publish door status
    }
  } else if (String(topic) == "home/control/window") {
    if (message == "OPEN") {
      windowServo.write(90); // Open window
      client.publish("home/control/window/status", "OPEN"); // Publish window status
    } else if (message == "CLOSE") {
      windowServo.write(0); // Close window
      client.publish("home/control/window/status", "CLOSE"); // Publish window status
    }
  } else if (String(topic) == "home/control/led") {
    if (message == "ON") {
      digitalWrite(LED_PIN, HIGH);
      client.publish("home/control/led/status", "ON"); // Publish LED status
    } else if (message == "OFF") {
      digitalWrite(LED_PIN, LOW);
      client.publish("home/control/led/status", "OFF"); // Publish LED status
    }
  } else if (String(topic) == "home/control/buzzer") {
    if (message == "ON") {
      activateBuzzer();
      client.publish("home/control/buzzer/status", "ON"); // Publish buzzer status
    } else if (message == "OFF") {
      client.publish("home/control/buzzer/status", "OFF"); // Publish buzzer status
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  espClient.setCACert(ca_cert);

  // Initialize LCD
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");

  // Initialize Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize Sensors and Servos
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT); // Changed from OUTPUT to INPUT
  pinMode(LDR_LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LDR_LED_PIN, LOW);

  doorServo.attach(DOOR_SERVO_PIN);
  windowServo.attach(WINDOW_SERVO_PIN);
  doorServo.write(0);  // Door closed
  windowServo.write(0);  // Window closed

  lcd.clear();
  lcd.print("Enter Password:");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check Gas Sensor
  int gasValue = analogRead(GAS_SENSOR_PIN);
  Serial.print("Gas Sensor Value: ");
  Serial.println(gasValue);
  client.publish("home/sensors/gas", String(gasValue).c_str());

  if (gasValue > 1000) {
    digitalWrite(BUZZER_PIN, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FIRE DETECTED!");
    client.publish("home/control/buzzer", "ON"); // Publish buzzer status
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    client.publish("home/control/buzzer", "OFF"); // Publish buzzer status
  }

  // Check Rain Sensor
  int rainValue = digitalRead(RAIN_SENSOR_PIN);
  Serial.print("Rain Sensor Value: ");
  Serial.println(rainValue); // Print rain sensor value to Serial Monitor
  if (rainValue == LOW) {
    windowServo.write(90);  // Close window
    client.publish("home/sensors/rain", "CLOSE"); // Publish rain sensor status
  } else {
    windowServo.write(0);   // Open window
    client.publish("home/sensors/rain", "OPEN"); // Publish rain sensor status
  }

  // Check IR Sensor
  int irValue = digitalRead(IR_SENSOR_PIN);
  Serial.print("IR Sensor Value: ");
  Serial.println(irValue); // Print IR sensor value to Serial Monitor
  if (irValue == LOW) {
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    client.publish("home/sensors/ir", "ON"); // Publish IR sensor status
  } else {
    digitalWrite(LED_PIN, LOW);   // Turn off LED
    client.publish("home/sensors/ir", "OFF"); // Publish IR sensor status
  }

  // Check LDR and control LED
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  client.publish("home/sensors/ldr", String(ldrValue).c_str());

  if (ldrValue > 500) {
    digitalWrite(LDR_LED_PIN, HIGH); // Turn on LDR-controlled LED if it's dark
    client.publish("home/control/ldr_led", "ON"); // Publish LDR LED state
  } else {
    digitalWrite(LDR_LED_PIN, LOW);  // Turn off LDR-controlled LED if it's bright
    client.publish("home/control/ldr_led", "OFF"); // Publish LDR LED state
  }

  // Keypad Input for Door Control
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      if (inputPassword == password) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Granted");
        doorServo.write(120); // Open the door
        client.publish("home/control/door", "OPEN"); // Publish door status
        delay(2000); // Keep door open for 2 seconds
        doorServo.write(0); // Close the door
        client.publish("home/control/door", "CLOSE"); // Publish door status
        wrongPasswordAttempts = 0; // Reset wrong attempts
      } else {
        wrongPasswordAttempts++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Denied");
        delay(2000);
        if (wrongPasswordAttempts >= 3) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Alert! Locked Out");
          activateBuzzer();
          client.publish("home/control/buzzer", "ON"); // Publish buzzer status
          delay(3000); // Keep buzzer active for 3 seconds
          client.publish("home/control/buzzer", "OFF"); // Publish buzzer status
          wrongPasswordAttempts = 0; // Reset wrong attempts
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter Password:");
        }
      }
      inputPassword = ""; // Clear input
    } else if (key == '*') {
      inputPassword = ""; // Clear input
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
    } else {
      inputPassword += key;
      lcd.setCursor(0, 1);
      lcd.print(inputPassword);  // Print the actual input
    }
  }

  delay(100); // Small delay to prevent excessive MQTT traffic
}