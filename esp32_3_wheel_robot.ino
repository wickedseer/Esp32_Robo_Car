#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Motor A
int motor1Pin1 = 18; //direction
int motor1Pin2 = 5;  //direction
int enable1Pin = 19; //speed

// Motor B
int motor2Pin1 = 17; //direction
int motor2Pin2 = 16; //direction
int enable2Pin = 4; //speed

// Setting PWM properties
const int freq = 30000; //hz
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int resolution = 8;
int dutyCycle = 50;

const char* ssid = "K33.Sapience-Office";
const char* password = "P@ssw0rd";

#define BOTtoken "5063386548:AAHPxyZnQRbRlNK1cBFreryzCamLnj3oe4g"
#define CHAT_ID "-611425498"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 50;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/forward \n";
      welcome += "/backward \n";
      welcome += "/left \n";
      welcome += "/right \n";
      welcome += "/stop \n";

      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/forward@ESP32SoCWatchBot") {
      bot.sendMessage(chat_id, "Moving forwards", "");

      // Move DC motor forward with increasing speed
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH);

      while (dutyCycle <= 200) {
        ledcWrite(pwmChannel1, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(50);
      }
      
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW);
      
      while (dutyCycle <= 255) {
        ledcWrite(pwmChannel2, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(50);
      }
    }

    if (text == "/backward@ESP32SoCWatchBot") {
      bot.sendMessage(chat_id, "Moving backwards", "");
     
      Serial.println("Moving Backwards");
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);

      while (dutyCycle <= 200) {
        ledcWrite(pwmChannel1, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(50);
      }
      
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, HIGH);
      
      while (dutyCycle <= 255) {
        ledcWrite(pwmChannel2, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(50);
      }
    }

    if (text == "/stop@ESP32SoCWatchBot") {
      bot.sendMessage(chat_id, "stopped", "");
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, LOW);
      
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, LOW);
    }

    if (text == "/left@ESP32SoCWatchBot") {
      bot.sendMessage(chat_id, "Turning left", "");
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH);

      while (dutyCycle <= 255) {
        ledcWrite(pwmChannel2, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(50);
      }
      
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, LOW);
      delay(1000);
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW);
      
      while (dutyCycle <= 200) {
        ledcWrite(pwmChannel1, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(50);
      }
    }

    if (text == "/right@ESP32SoCWatchBot") {
      bot.sendMessage(chat_id, "Turning right", "");
      // Move DC motor backwards at maximum speed
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW);
      
      while (dutyCycle <= 255) {
        ledcWrite(pwmChannel2, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(50);
      }
      
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, LOW);
      delay(1000);
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH);

      
      while (dutyCycle <= 200) {
        ledcWrite(pwmChannel1, dutyCycle);
        Serial.print("Forward with duty cycle: ");
        Serial.println(dutyCycle);
        dutyCycle = dutyCycle + 5;
        delay(50);
      }
    }
  }
}

void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel1, freq, resolution);

  ledcSetup(pwmChannel2, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel1); //motor 1
  
  ledcAttachPin(enable2Pin, pwmChannel2); //motor 2

  Serial.begin(115200);

  // testing
  Serial.print("Testing DC Motor...");

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
