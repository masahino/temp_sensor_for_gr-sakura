/*GR-SAKURA Sketch Template Version: V1.01*/
#include <rxduino.h>
#include <Ethernet.h>
#include "DHT11.h"
#include "tkdn_usb.h"

#include "TM1637.h"

DHT11 *dht11;
TEthernet Ethernet;
EthernetServer server(80);
byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x02, 0x8C};
byte myip[] = {192, 168, 1, 223};
static unsigned long start;
static int client_status, client_timer;

/* Button(P) */
const int pinButton = 4;

/* 7seg led */
TM1637 tm1637(6, 7); /* D6 clock, D7 DIO */

static void timer()
{
  client_timer += 1;
  if (client_timer >= 10000) {
    digitalWrite(PIN_LED2, HIGH);
    system_reboot(REBOOT_USERAPP);
  }
  if (digitalRead(PIN_SW) == LOW){
    system_reboot(REBOOT_USERAPP);
  }

  if (digitalRead(pinButton)) {
    system_reboot(REBOOT_USERAPP);
  }
}

static void led_disp(int temp, int humid)
{
  int8_t NumTab[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int8_t ListDisp[4];

  ListDisp[0] = temp / 10;
  ListDisp[1] = temp % 10;
  ListDisp[2] = humid / 10;
  ListDisp[3] = humid % 10;
  tm1637.display(ListDisp);
}

void setup()
{
  pinMode(PIN_LED0, OUTPUT);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_SW, INPUT);
  pinMode(pinButton, INPUT);
  digitalWrite(PIN_LED0, HIGH);
  delay(1000);
  dht11 = new DHT11(2);
  dht11->begin();

  /* 7seg init */
  tm1637.init();
  tm1637.set(BRIGHT_DARKEST);

  Serial.begin(9600);
  digitalWrite(PIN_LED0, LOW);
  
  Ethernet.begin(mac, myip);
  server.begin();

  start = millis();
  client_status = 0;
  client_timer = 0;
  timer_regist_userfunc(timer);

  /* for 7seg */
  int8_t ListDisp[4];
  for(int i = 0; i < 4; i++) {
    ListDisp[i] = 0;
  }
  tm1637.display(ListDisp);

  digitalWrite(PIN_LED0, HIGH);
}

void loop()
{
  static char msg[32];
  float temp, humid;
  int   iret;

  
  EthernetClient client = server.available();
  
  client_status = 0;
  client_timer = 0;
  if (client) {
    digitalWrite(PIN_LED1, HIGH);
    client_status = 1;
    if (TKUSB_IsConnected() == 1) {
      Serial.println("new client");
    }
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      Ethernet.processPackets();
      delay(10);
      digitalWrite(PIN_LED3, HIGH);
      if (client.available()) {
        char c = client.read();
        if (TKUSB_IsConnected() == 1) {
          Serial.write(c);
        }
        if (c == '\n' && currentLineIsBlank) {
          iret = dht11->read(&temp, &humid);
          int length = 0;
          if ( iret == 0 ){
            digitalWrite(PIN_LED2, LOW);
            snprintf(msg,32,"T: %5.2f C, H: %5.2f %%", temp, humid);
            led_disp((int)temp, (int)humid);
            if (TKUSB_IsConnected() == 1) {
              Serial.println(msg);
            }
//            client.println(msg);
          }else{
            digitalWrite(PIN_LED2, HIGH);
            snprintf(msg,32,"Errcode = %4d",iret);
            if (TKUSB_IsConnected() == 1) {
              Serial.println(msg);
            }
//            client.println(msg);
            //delay(60*1000);
          }
          length = strlen(msg)+2;
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.print("Content-Length: ");
          client.println(length);
          if (TKUSB_IsConnected() == 1) {
            Serial.println(length);
          }
          client.println("");
          client.println(msg);
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    digitalWrite(PIN_LED3, LOW);
    client.stop();
//  delay(1000);
    digitalWrite(PIN_LED1, LOW);
  }
}
