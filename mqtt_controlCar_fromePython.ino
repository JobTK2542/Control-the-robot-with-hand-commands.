/* include library */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883
#define MQTT_NAME "tawatchai"
/* define port */
WiFiClient client;
PubSubClient mqtt(client);

 
/* WIFI settings */
#define ssid  "COE_2.4GHz"
#define password  "coe11soi4"
 
/* data received from python */
String  data = "";

 /*-------definning Inputs------*/
int LS = D1;     // left sensor
int RS  = D2;      // right sensor

/*-------definning Outputs------*/
int LM1 = D3;      // left motor
int LM2 =  D4;       // left motor
int RM1 =  D5;       // right motor
int RM2 =  D6;       // right motor

/*-------PWM ควบคุมความเร็วมอเตอร์-----*/
int PWM1 = D7;    // PWM left motor
int PWM2 = D8;    // PWM right motor
int SPEED = 125;  // Speed PWM สามารถปรับความเร็วได้ถึง 255


int LS_show = 0;
int RS_show = 0;

void setup()
{
  pinMode(LS, INPUT);
  pinMode(RS, INPUT);
  pinMode(LM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(RM1, OUTPUT);
  pinMode(RM2, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  Serial.begin(115200);

   
  WiFi.begin(ssid, password);
 
  // Attempt to connect to WiFi network:
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    // Connect to WPA/WPA2 network. Change this line if using open or WEP  network:
    // Wait 3 seconds for connection:
    delay(3000);
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    mqtt.setCallback(callback);
  
}
void loop()
{
  connectMqtt();
  LS_show = digitalRead(LS);
  RS_show = digitalRead(RS);
//  Serial.print("ซ้าย = "); 
//  Serial.print(LS_show); 
//  Serial.print("   ขวา = ");
//  Serial.println(RS_show);
    

  if(data == "0") //stop
  {
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, LOW);
    Serial.println(" stop ");
  }
  
  if(data == "1") //right
  {
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, LOW);
    analogWrite(PWM2, SPEED);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, LOW);
    Serial.println(" right ");
  }
  if(data == "2") //left
  {
    analogWrite(PWM1, SPEED);
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, LOW);
    Serial.println(" left ");
  }
  if(data == "3") //backward
  {
    analogWrite(PWM1, SPEED);
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, HIGH);
    analogWrite(PWM2, SPEED);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, HIGH);
    Serial.println(" backward ");
  }
  if(data == "4") // Auto 
   {
    if (!(digitalRead(LS)) && !(digitalRead(RS)))    // Move Forward
    {
      analogWrite(PWM1, SPEED);
      digitalWrite(LM1, HIGH);
      digitalWrite(LM2, LOW);
      analogWrite(PWM2, SPEED);
      digitalWrite(RM1, HIGH);
      digitalWrite(RM2, LOW);
      Serial.println(" Auto Move Forward ");
    }
  
  
    if (!(digitalRead(LS)) && digitalRead(RS))    // Turn right
    {
      digitalWrite(LM1, HIGH);
      digitalWrite(LM2, LOW);
      analogWrite(PWM2, SPEED);
      digitalWrite(RM1, LOW);
      digitalWrite(RM2, LOW);
      Serial.println(" Auto Turn right ");
    }
  
    if (digitalRead(LS) && !(digitalRead(RS)))    // turn left
    {
      analogWrite(PWM1, SPEED);
      digitalWrite(LM1, LOW);
      digitalWrite(LM2, LOW);
      digitalWrite(RM1, HIGH);
      digitalWrite(RM2, LOW);
      Serial.println(" Auto turn left");
    }
  
   if (digitalRead(LS) && digitalRead(RS))    // stop
    {
      digitalWrite(LM1, LOW);
      digitalWrite(LM2, LOW);
      digitalWrite(RM1, LOW);
      digitalWrite(RM2, LOW);
      Serial.println(" Auto stop  ");
    }
   }
  if(data == "5")
  {
    Serial.println(" Forward ");
    analogWrite(PWM1, SPEED);
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, LOW);
    analogWrite(PWM2, SPEED);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, LOW);
  }

  mqtt.loop();
}

void connectMqtt()
{
  

    if (mqtt.connected() == false)
    {
        Serial.print("MQTT connection... ");
        if (mqtt.connect(MQTT_NAME, "",""))
        {
            Serial.println("connected");
            mqtt.subscribe("python/mqtt");
        }
    }
}
void callback(char *topic, byte *payload, unsigned int length)
{
    payload[length] = '\0';
    String topic_str = topic, payload_str = (char *)payload;
    Serial.println("[" + topic_str + "]: " + payload_str);

    data = payload_str;
}
