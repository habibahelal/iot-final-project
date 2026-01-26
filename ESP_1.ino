#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

// Wi-Fi credentials
const char* ssid = "Orange-fares";
const char* password = "rmc136a1drd47r";

// MQTT Broker settings
const char* mqtt_server = "7723500f166547509bc34df058860232.s1.eu.hivemq.cloud";
const char* mqtt_username = "faresmohamed260";
const char* mqtt_password = "#Rmc136a1drd47r";
const int mqtt_port = 8883;

// Certificates for secure connection
static const char* root_ca PROGMEM = R"EOF(
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

// Define topics
const char* front_door_topic = "front_door";
const char* fan_state_topic = "fan_state";
const char* unlock_button_topic = "unlock_button";
const char* fan_power_topic = "fan_power";
const char* fan_speed_topic = "fan_speed";
const char* dht_topic = "dht";

// Variables
bool frontDoorOpen = false;
const int servoSpeed = 15;
bool fanOn = false;
int fanSpeed = 0;
float currentTemperature = 0.0;

// Define the keypad rows and columns
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns

// Define the keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};


// Pin Definitions
#define IR_SENSOR_PIN 15
#define LED_PIN_FRONT_DOOR 34
#define SERVO_PIN 13
#define SS_PIN 21 // Slave Select (SDA)
#define RST_PIN 22 // Reset pin
#define MOTOR_IN1 2
#define MOTOR_IN2 4
#define MOTOR_ENA 5

// Connect the keypad row and column pins
byte rowPins[ROWS] = {12, 14, 27, 26};
byte colPins[COLS] = {25, 33, 32};

// Create a Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

Servo myServo; 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

// Stored RFID UID
byte storedUID[4] = {0xAD, 0x80, 0x68, 0x21};

// Stored passcode (4-digit passcode)
const char savedPasscode[5] = "1234";

// MQTT client and Wi-Fi client
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Task handles
TaskHandle_t frontDoorTaskHandle = NULL;
TaskHandle_t unlockButtonTaskHandle = NULL;
TaskHandle_t checkPasscodeTaskHandle = NULL;
TaskHandle_t servoTaskHandle = NULL;
TaskHandle_t speed1TaskHandle = NULL;
TaskHandle_t speed2TaskHandle = NULL;
TaskHandle_t speed3TaskHandle = NULL;
TaskHandle_t powerTaskHandle = NULL;
TaskHandle_t temperatureMonitorTaskHandle = NULL;

// Function prototypes
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Initialize SPI bus
  mfrc522.PCD_Init(); // Initialize MFRC522 reader

  // Initialize pins
  pinMode(IR_SENSOR_PIN, INPUT);

  pinMode(LED_PIN_FRONT_DOOR, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  myServo.attach(SERVO_PIN);

  // Initialize all LEDs and Servo
  digitalWrite(LED_PIN_FRONT_DOOR, LOW);
  myServo.write(90);

  // Connect to Wi-Fi
  setup_wifi();

  // Set MQTT server and callback function
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Connect to MQTT
  reconnect();

  // Publish initial fan state
  if (client.connected()) {
    client.publish(fan_state_topic, "OFF", true);
    Serial.println("Fan state initialized to OFF");
  }

  // Create FreeRTOS tasks to handle LEDs based on MQTT messages
  xTaskCreatePinnedToCore(frontDoorTask, "Front Door Task", 10000, NULL, 1, &frontDoorTaskHandle, 1);
  xTaskCreatePinnedToCore(unlockButtonTask, "Unlock Button Task", 10000, NULL, 1, &unlockButtonTaskHandle, 1);
  xTaskCreatePinnedToCore(checkPasscodeTask, "Check Passcode Task", 10000, NULL, 1, &checkPasscodeTaskHandle, 1);
  xTaskCreate(powerTask, "Power Task", 10000, NULL, 1, &powerTaskHandle);
  xTaskCreate(speed1Task, "Speed 1 Task", 10000, NULL, 1, &speed1TaskHandle);
  xTaskCreate(speed2Task, "Speed 2 Task", 10000, NULL, 1, &speed2TaskHandle);
  xTaskCreate(speed3Task, "Speed 3 Task", 10000, NULL, 1, &speed3TaskHandle);
  xTaskCreate(temperatureMonitorTask, "Temperature Monitor Task", 10000, NULL, 1, &temperatureMonitorTaskHandle);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  delay(1000);  // Adjust delay as needed
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");

      // Subscribe to topics
      client.subscribe(unlock_button_topic);
      client.subscribe(fan_power_topic);
      client.subscribe(fan_speed_topic);
      client.subscribe(dht_topic);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Send the received message to the corresponding task
  if (String(topic) == unlock_button_topic && message == "LOW") {
    xTaskNotifyGive(unlockButtonTaskHandle);  // Use task notification for Online Reset button
  }
  if (String(topic) == fan_power_topic && message == "LOW") {
    xTaskNotifyGive(powerTaskHandle);  // Use task notification for button
  }
  if (String(topic) == fan_speed_topic && message == "LOW") {
    xTaskNotifyGive(speed1TaskHandle);  // Use task notification for button
  }
  if (String(topic) == fan_speed_topic && message == "MEDIUM") {
    xTaskNotifyGive(speed2TaskHandle);  // Use task notification for button
  }
  if (String(topic) == fan_speed_topic && message == "HIGH") {
    xTaskNotifyGive(speed3TaskHandle);  // Use task notification for button
  }
  if (String(topic) == dht_topic) {
    // Parse temperature value
    currentTemperature = message.toFloat();
    Serial.print("Temperature received: ");
    Serial.println(currentTemperature);
  }
}

// Task for Front Door
void frontDoorTask(void* parameter) {
  while (1) {
    if (!frontDoorOpen && digitalRead(IR_SENSOR_PIN) == LOW && checkRFID()) {
      frontDoorOpen = true;
      myServo.write(0);
      digitalWrite(LED_PIN_FRONT_DOOR, HIGH);
      Serial.println("Front Door unlocked!");
      // Publish sensor state to MQTT topics
      if (client.publish(front_door_topic, "UNLOCKED", true)) {
        Serial.println("Front Door state published successfully.");
      } else {
        Serial.println("Failed to publish Front Door state.");
      }
      while(digitalRead(IR_SENSOR_PIN) == LOW) {}
      delay(2000);
      myServo.write(90);
      frontDoorOpen = false;
      digitalWrite(LED_PIN_FRONT_DOOR, LOW);
      Serial.println("Front Door locked!");
      // Publish sensor state to MQTT topics
      if (client.publish(front_door_topic, "LOCKED", true)) {
        Serial.println("Front Door state published successfully.");
      } else {
        Serial.println("Failed to publish Front Door state.");
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay to avoid spamming
  }
}

void checkPasscodeTask(void* parameter) {
  while (1) {
    if (!frontDoorOpen && digitalRead(IR_SENSOR_PIN) == LOW) {
      char enteredPasscode[5] = ""; // Buffer to store the entered passcode
      int index = 0; // Index for the entered passcode buffer

      while (index < 4) {
        char key = keypad.getKey(); // Get the key pressed

        if (key) { // If a key is pressed
          enteredPasscode[index] = key; // Store the key in the buffer
          index++;
        }
      }
      enteredPasscode[4] = '\0'; // Null-terminate the string
      if (strcmp(enteredPasscode, savedPasscode) == 0) {
        frontDoorOpen = true;
        myServo.write(0);
        digitalWrite(LED_PIN_FRONT_DOOR, HIGH);
        Serial.println("Front Door unlocked!");
        // Publish sensor state to MQTT topics
        if (client.publish(front_door_topic, "UNLOCKED", true)) {
          Serial.println("Front Door state published successfully.");
        } else {
          Serial.println("Failed to publish Front Door state.");
        }
        while(digitalRead(IR_SENSOR_PIN) == LOW) {}
        delay(2000);
        myServo.write(90);
        frontDoorOpen = false;
        digitalWrite(LED_PIN_FRONT_DOOR, LOW);
        Serial.println("Front Door locked!");
        // Publish sensor state to MQTT topics
        if (client.publish(front_door_topic, "LOCKED", true)) {
          Serial.println("Front Door state published successfully.");
        } else {
          Serial.println("Failed to publish Front Door state.");
        }
      }   
    }
  }
}

void unlockButtonTask(void *parameter) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for the notification
    frontDoorOpen = true;
    myServo.write(0);
    digitalWrite(LED_PIN_FRONT_DOOR, HIGH);
    Serial.println("Front Door unlocked!");
    // Publish sensor state to MQTT topics
    if (client.publish(front_door_topic, "UNLOCKED", true)) {
      Serial.println("Front Door state published successfully.");
    } else {
      Serial.println("Failed to publish Front Door state.");
    }
    delay(5000);
    myServo.write(90);
    frontDoorOpen = false;
    digitalWrite(LED_PIN_FRONT_DOOR, LOW);
    Serial.println("Front Door locked!");
    // Publish sensor state to MQTT topics
    if (client.publish(front_door_topic, "LOCKED", true)) {
      Serial.println("Front Door state published successfully.");
    } else {
      Serial.println("Failed to publish Front Door state.");
    }
  }
}

bool checkRFID() {
  // Check if a new card is present
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // Compare the UID with the stored one
  if (compareRFID(mfrc522.uid.uidByte, storedUID, mfrc522.uid.size)) {
    return true;
  } else {
    return false;
  }
}

bool compareRFID(byte *readUID, byte *storedUID, byte length) {
  for (byte i = 0; i < length; i++) {
    if (readUID[i] != storedUID[i]) {
      return false; // UID does not match
    }
  }
  return true; // UID matches
}

void speed1Task(void *parameter) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for the notification
    if (fanOn) {
      fanSpeed = 1;
      setFanSpeed(fanSpeed);
    }
  }
}

void speed2Task(void *parameter) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for the notification
    if (fanOn) {
      fanSpeed = 2;
      setFanSpeed(fanSpeed);
    }
  }
}

void speed3Task(void *parameter) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for the notification
    if (fanOn) {
      fanSpeed = 3;
      setFanSpeed(fanSpeed);
    }
  }
}

void setFanSpeed(int speed) {
  if (fanOn) {
    switch (speed) {
      case 1:
        digitalWrite(MOTOR_IN1, HIGH);
        digitalWrite(MOTOR_IN2, LOW);
        analogWrite(MOTOR_ENA, 128);
        Serial.println("Fan is on low speed!");
        break;
      case 2:
        digitalWrite(MOTOR_IN1, HIGH);
        digitalWrite(MOTOR_IN2, LOW);
        analogWrite(MOTOR_ENA, 192);
        Serial.println("Fan is on medium speed!");
        break;
      case 3:
        digitalWrite(MOTOR_IN1, HIGH);
        digitalWrite(MOTOR_IN2, LOW);
        analogWrite(MOTOR_ENA, 255);
        Serial.println("Fan is on high speed!");
        break;
    }
  }
}

void stopFan() {
        digitalWrite(MOTOR_IN1, LOW); // Low speed
        digitalWrite(MOTOR_IN2, LOW);
        analogWrite(MOTOR_ENA, 0);
}

void powerTask(void *pvParameters) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for the notification
    if((!fanOn)) {
      fanOn = true;
      fanSpeed = 1;
      setFanSpeed(fanSpeed);
      Serial.println("Fan is turned on!");
      // Publish fan state to MQTT topics
      if (client.publish(fan_state_topic, "ON", true)) {
        Serial.println("Fan state published successfully.");
      } else {
      Serial.println("Failed to publish Fan state.");
      }
    } else {
      fanOn = false;
      stopFan();
      Serial.println("Fan is turned off!");
      // Publish fan state to MQTT topics
      if (client.publish(fan_state_topic, "OFF", true)) {
        Serial.println("Fan state published successfully.");
      } else {
        Serial.println("Failed to publish Fan state.");
      }
    }
  }
}

void temperatureMonitorTask(void *pvParameters) {
  while (1) {
    // Check if temperature exceeds threshold
    if (currentTemperature > 35.0) {
      if (!fanOn) {
        // Auto-enable fan due to high temperature
        fanOn = true;
        fanSpeed = 1; // Set to low speed
        setFanSpeed(fanSpeed);
        Serial.println("Auto Fan ON - High temperature detected!");
        
        // Publish fan state to MQTT
        if (client.publish(fan_state_topic, "ON", true)) {
          Serial.println("Fan state published successfully (Auto).");
        } else {
          Serial.println("Failed to publish Fan state.");
        }
      }
    }
    
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Check every 2 seconds
  }
}