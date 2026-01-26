#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>

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
const char* fire_alarm_topic = "fire_alarm";
const char* security_alarm_topic = "security_alarm";
const char* online_reset_button_topic = "online_reset_button";
const char* secure_home_topic = "secure_home";
const char* front_door_topic = "front_door";
const char* light_switch_topic = "light_switch";
const char* light_state_topic = "light_state";
const char* auto_light_topic = "auto_light";
const char* dht_topic = "dht";

// Variables to hold alarm states
volatile bool fireAlarmTriggered = false;
volatile bool securityAlarmTriggered = false;
volatile bool securityAlarmEnabled = false;  // Disabled by default
volatile bool frontDoorUnlocked = false;
volatile bool lightOn = false;
volatile bool autoLightEnabled = false;  // Disabled by default
bool WiFiConnected = false;
bool MQTTConnected = false;

// Pin Definitions
#define FLAME_SENSOR_PIN 4
#define SMOKE_SENSOR_PIN 13
#define LED_PIN_FIRE 14
#define TRIG_PIN 5
#define ECHO_PIN 18
#define LED_PIN_SECURITY 27
#define BUZZER_PIN 19
#define DHTPIN 15
#define DHTTYPE DHT11
#define LED_PIN_FRONT_DOOR 2
#define LED_PIN_LIGHT 12
#define LDR_PIN 34
#define LDR_THRESHOLD 400  // Threshold for darkness detection

// Create a DHT object
DHT dht(DHTPIN, DHTTYPE);

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// MQTT client and Wi-Fi client
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Task handles
TaskHandle_t fireAlarmTaskHandle = NULL;
TaskHandle_t securityAlarmTaskHandle = NULL;
TaskHandle_t onlineResetButtonTaskHandle = NULL;
TaskHandle_t DHTTaskHandle = NULL;
TaskHandle_t pulseAlarmTaskHandle = NULL;
TaskHandle_t lcdUpdateTaskHandle = NULL;
TaskHandle_t frontDoorTaskHandle = NULL;
TaskHandle_t autoLightTaskHandle = NULL;

// Mutex for MQTT operations
SemaphoreHandle_t mqttMutex;

// Function prototypes
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
long measureDistance();
void turnOffAlarms();

void setup() {
  Serial.begin(115200);

  // Create mutex for MQTT operations
  mqttMutex = xSemaphoreCreateMutex();

  // Initialize the DHT sensor
  dht.begin();

  // Initialize pins
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(SMOKE_SENSOR_PIN, INPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(LED_PIN_FIRE, OUTPUT);
  pinMode(LED_PIN_SECURITY, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN_FRONT_DOOR, OUTPUT);
  pinMode(LED_PIN_LIGHT, OUTPUT);

  // Initialize all LEDs and Buzzer as OFF
  digitalWrite(LED_PIN_FIRE, LOW);
  digitalWrite(LED_PIN_SECURITY, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN_FRONT_DOOR, LOW);
  digitalWrite(LED_PIN_LIGHT, LOW);

  // Initialize the LCD
  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  // Connect to Wi-Fi
  setup_wifi();

  // Set MQTT server and callback function
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Connect to MQTT
  reconnect();

  // Publish initial OFF state for both alarms
  if (client.connected()) {
    client.publish(fire_alarm_topic, "OFF", true);
    Serial.println("Fire Alarm initialized to OFF");
    client.publish(security_alarm_topic, "OFF", true);
    Serial.println("Security Alarm initialized to OFF");
    client.publish(light_state_topic, "OFF", true);
    Serial.println("Light state initialized to OFF");
  }

  // Create FreeRTOS tasks with proper priorities
  xTaskCreatePinnedToCore(fireAlarmTask, "Fire Alarm Task", 4096, NULL, 3, &fireAlarmTaskHandle, 0);
  xTaskCreatePinnedToCore(securityAlarmTask, "Security Alarm Task", 4096, NULL, 3, &securityAlarmTaskHandle, 0);
  xTaskCreatePinnedToCore(onlineResetButtonTask, "Online Reset Button Task", 4096, NULL, 2, &onlineResetButtonTaskHandle, 0);
  xTaskCreatePinnedToCore(DHTTask, "DHT Task", 4096, NULL, 1, &DHTTaskHandle, 0);
  xTaskCreatePinnedToCore(pulseAlarmTask, "Pulse Alarm Task", 4096, NULL, 2, &pulseAlarmTaskHandle, 1);
  xTaskCreatePinnedToCore(lcdUpdateTask, "LCD Update Task", 4096, NULL, 1, &lcdUpdateTaskHandle, 1);
  xTaskCreatePinnedToCore(frontDoorTask, "Front Door Task", 4096, NULL, 1, &frontDoorTaskHandle, 1);
  xTaskCreatePinnedToCore(autoLightTask, "Auto Light Task", 4096, NULL, 1, &autoLightTaskHandle, 1);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  vTaskDelay(10 / portTICK_PERIOD_MS);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    WiFiConnected = true;
  } else {
    Serial.println("\nWiFi connection failed!");
    WiFiConnected = false;
  }
}

void reconnect() {
  int attempts = 0;
  while (!client.connected() && attempts < 3) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client2", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      MQTTConnected = true;
      client.subscribe(online_reset_button_topic);
      client.subscribe(secure_home_topic);
      client.subscribe(front_door_topic);
      client.subscribe(light_switch_topic);
      client.subscribe(auto_light_topic);
      break;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      MQTTConnected = false;
      Serial.println(" try again in 5 seconds");
      attempts++;
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

  if (String(topic) == online_reset_button_topic && message == "ON") {
    xTaskNotifyGive(onlineResetButtonTaskHandle);
  }
  else if (String(topic) == secure_home_topic) {
    if (message == "ON") {
      securityAlarmEnabled = true;
      Serial.println("Security alarm system ENABLED");
    } else if (message == "OFF") {
      securityAlarmEnabled = false;
      // Turn off security alarm if it was active
      if (securityAlarmTriggered) {
        securityAlarmTriggered = false;
        digitalWrite(LED_PIN_SECURITY, LOW);
        if (!fireAlarmTriggered) {
          digitalWrite(BUZZER_PIN, LOW);
        }
        // Publish OFF state
        if (xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
          client.publish(security_alarm_topic, "OFF", true);
          xSemaphoreGive(mqttMutex);
        }
      }
      Serial.println("Security alarm system DISABLED");
    }
  }
  else if (String(topic) == front_door_topic) {
    if (message == "UNLOCKED") {
      frontDoorUnlocked = true;
      Serial.println("Front door UNLOCKED");
    } else if (message == "LOCKED") {
      frontDoorUnlocked = false;
      Serial.println("Front door LOCKED");
    }
  }
  else if (String(topic) == light_switch_topic) {
    if (message == "ON") {
      autoLightEnabled = false;  // Disable auto mode on manual control
      lightOn = true;
      digitalWrite(LED_PIN_LIGHT, HIGH);
      Serial.println("Light turned ON (manual)");
      // Publish light state
      if (xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        client.publish(light_state_topic, "ON", true);
        xSemaphoreGive(mqttMutex);
      }
    } else if (message == "OFF") {
      autoLightEnabled = false;  // Disable auto mode on manual control
      lightOn = false;
      digitalWrite(LED_PIN_LIGHT, LOW);
      Serial.println("Light turned OFF (manual)");
      // Publish light state
      if (xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        client.publish(light_state_topic, "OFF", true);
        xSemaphoreGive(mqttMutex);
      }
    }
  }
  else if (String(topic) == auto_light_topic) {
    if (message == "ON") {
      autoLightEnabled = true;
      Serial.println("Auto light mode ENABLED");
    } else if (message == "OFF") {
      autoLightEnabled = false;
      Serial.println("Auto light mode DISABLED");
    }
  }
}

// Task for Fire Alarm - monitors sensors
void fireAlarmTask(void *parameter) {
  while (1) {
    if (!fireAlarmTriggered && (digitalRead(FLAME_SENSOR_PIN) == LOW || digitalRead(SMOKE_SENSOR_PIN) == LOW)) {
      // Take mutex before MQTT operation
      if (xSemaphoreTake(mqttMutex, portMAX_DELAY) == pdTRUE) {
        if (client.publish(fire_alarm_topic, "ON", true)) {
          Serial.println("Fire Alarm state published successfully.");
        } else {
          Serial.println("Failed to publish Fire Alarm state.");
        }
        xSemaphoreGive(mqttMutex);
      }
      
      fireAlarmTriggered = true;
      Serial.println("Fire Alarm triggered!");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Function to measure distance using the ultrasonic sensor
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if (duration == 0) return -1; // Timeout
  
  long distance = (duration * 0.034) / 2;
  return distance;
}

// Task for Security Alarm
void securityAlarmTask(void *parameter) {
  while (1) {
    // Only monitor if security alarm is enabled
    if (securityAlarmEnabled) {
      long distance = measureDistance();
      if (!securityAlarmTriggered && distance < 50 && distance > 0) {
        // Take mutex before MQTT operation
        if (xSemaphoreTake(mqttMutex, portMAX_DELAY) == pdTRUE) {
          if (client.publish(security_alarm_topic, "ON", true)) {
            Serial.println("Security Alarm state published successfully.");
          } else {
            Serial.println("Failed to publish Security Alarm state.");
          }
          xSemaphoreGive(mqttMutex);
        }
        
        securityAlarmTriggered = true;
        Serial.println("Security Alarm triggered!");
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Single task for pulsing both alarms
void pulseAlarmTask(void *parameter) {
  while (1) {
    if (fireAlarmTriggered || securityAlarmTriggered) {
      // Turn on appropriate LED(s)
      if (fireAlarmTriggered) {
        digitalWrite(LED_PIN_FIRE, HIGH);
      }
      if (securityAlarmTriggered) {
        digitalWrite(LED_PIN_SECURITY, HIGH);
      }
      digitalWrite(BUZZER_PIN, HIGH);
      vTaskDelay(500 / portTICK_PERIOD_MS);

      // Turn off LEDs and buzzer
      digitalWrite(LED_PIN_FIRE, LOW);
      digitalWrite(LED_PIN_SECURITY, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS);
    } else {
      // No alarms active, just wait
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
}

void onlineResetButtonTask(void *parameter) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    turnOffAlarms();
  }
}

void turnOffAlarms() {
  // Publish OFF state for alarms before resetting
  if (xSemaphoreTake(mqttMutex, portMAX_DELAY) == pdTRUE) {
    if (fireAlarmTriggered) {
      client.publish(fire_alarm_topic, "OFF", true);
      Serial.println("Fire Alarm state set to OFF");
    }
    if (securityAlarmTriggered) {
      client.publish(security_alarm_topic, "OFF", true);
      Serial.println("Security Alarm state set to OFF");
    }
    xSemaphoreGive(mqttMutex);
  }
  
  fireAlarmTriggered = false;
  securityAlarmTriggered = false;

  // Turn off all LEDs and Buzzer
  digitalWrite(LED_PIN_FIRE, LOW);
  digitalWrite(LED_PIN_SECURITY, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("Alarms reset!");
}

void DHTTask(void *parameter) {
  while (1) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      String temperatureString = String(temperature, 1);
      
      // Take mutex before MQTT operation
      if (xSemaphoreTake(mqttMutex, portMAX_DELAY) == pdTRUE) {
        if (client.publish(dht_topic, temperatureString.c_str(), true)) {
          Serial.print("Temperature published: ");
          Serial.println(temperatureString);
        } else {
          Serial.println("Failed to publish temperature.");
        }
        xSemaphoreGive(mqttMutex);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(30000));
  }
}

void lcdUpdateTask(void *parameter) {
  while (1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi: ");
    lcd.print(WiFiConnected ? "OK" : "FAIL");
    
    lcd.setCursor(0, 1);
    lcd.print("MQTT: ");
    lcd.print(MQTTConnected ? "OK" : "FAIL");
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void frontDoorTask(void *parameter) {
  while (1) {
    if (frontDoorUnlocked) {
      digitalWrite(LED_PIN_FRONT_DOOR, HIGH);
    } else {
      digitalWrite(LED_PIN_FRONT_DOOR, LOW);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void autoLightTask(void *parameter) {
  while (1) {
    if (autoLightEnabled) {
      int ldrValue = analogRead(LDR_PIN);
      
      // Check if it's dark (LDR value below threshold)
      if (ldrValue < LDR_THRESHOLD) {
        if (!lightOn) {
          lightOn = true;
          digitalWrite(LED_PIN_LIGHT, HIGH);
          Serial.print("Auto Light ON - Dark detected (LDR: ");
          Serial.print(ldrValue);
          Serial.println(")");
          
          // Publish light state
          if (xSemaphoreTake(mqttMutex, portMAX_DELAY) == pdTRUE) {
            client.publish(light_state_topic, "ON", true);
            xSemaphoreGive(mqttMutex);
          }
        }
      } else {
        // It's bright enough
        if (lightOn) {
          lightOn = false;
          digitalWrite(LED_PIN_LIGHT, LOW);
          Serial.print("Auto Light OFF - Bright detected (LDR: ");
          Serial.print(ldrValue);
          Serial.println(")");
          
          // Publish light state
          if (xSemaphoreTake(mqttMutex, portMAX_DELAY) == pdTRUE) {
            client.publish(light_state_topic, "OFF", true);
            xSemaphoreGive(mqttMutex);
          }
        }
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Check every second
  }
}