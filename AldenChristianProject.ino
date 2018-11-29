#include <Ethernet.h>
#include <SPI.h>
#include <EEPROM.h>
#include <sha1.h>
#include <Time.h>
#include <Twitter.h>
#include <avr/pgmspace.h>

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(10,0,0,80);
EthernetClient client;

IPAddress twitter_ip(199, 59, 149, 232);
uint16_t twitter_port = 80;

char buffer[512];
const static char consumer_key[] PROGMEM = "1067956091936563201-3UoqQZu7EVoKaWVT4ULspsu1XtWlDK";
const static char consumer_secret[] PROGMEM = "gnkyeh5mQEHp5izeM1Vzz4Xgp4wIGYCI8aLm4sy8DRUG0";

Twitter twitter(buffer, sizeof(buffer));

unsigned long timestamp;
boolean entering = false;
char* entryMessage = "I'm baaaaack!";
char* exitMessage = "I'm outta here!";

const int entryPin = A0;   // Define the input pin for entry sensor 
const int exitPin = A1;    // Define the input pin for the exit sensor 
int entryValue = 0;  // Define variable to store the entry value 
int exitValue = 0;   // Define variable to store the exit value

void setup() {  
  Serial.begin(9600);  // Open a serial connection to display the data  
  Serial.println("Attempting to get an P address using DHCP:");
  if (!Ethernet.begin(mac)) {
    Serial.println("Failed to get an IP address using DHCP, trying the static IP");
    Ethernet.begin(mac, ip);
  }

  Serial.print("My address:");
  Serial.println(Ethernet.localIP());
  delay(3000); // delay so we can read stuff on the Serial Monitor 
  
  pinMode(entryPin, INPUT);   
  pinMode(exitPin, INPUT);
  
  twitter.set_twitter_endpoint(PSTR("api.twitter.com"), 
   PSTR("/1/statuses/update.json"), twitter_ip, twitter_port, false);
  twitter.set_client_id(consumer_key, consumer_secret);

#if 0 
  // Read OAuth account identification from EEPROM.
  twitter.set_account_id(256, 384);
#else 
  // Set OAuth account identification from program memory.
  twitter.set_account_id(PSTR("1067956091936563201-3UoqQZu7EVoKaWVT4ULspsu1XtWlDK"), PSTR("gnkyeh5mQEHp5izeM1Vzz4Xgp4wIGYCI8aLm4sy8DRUG0"));
#endif   

}

void loop() {  
  entryValue = analogRead(entryPin); // Read the value from entry sensor  

  Serial.println(entryValue); //Uncomment this line to monitor the readings

  if (entryValue < 50) {
    entering = true;
    sendTweet();
  }

  exitValue = analogRead(exitPin);   // Read the value from exit sensor  
  if (exitValue < 50) {
    entering = false;
    sendTweet();
  }
  delay(10);
 /* Serial.print("Sensor values: ");     
  Serial.print(entryValue); // Print the entry sensor value  
  Serial.print(", ");
  Serial.println(exitValue); // Print the exit  
  delay(1000);  // Wait a moment, so we can read what’s been printed 
*/
}

void sendTweet() {
  
  if (twitter.is_ready()) {
    char tweet[140];
    timestamp = twitter.get_time();

    if(entering) {
      sprintf(tweet, "%02d:%02d:%02d: %s", hour(timestamp), minute(timestamp), second(timestamp), entryMessage);
    }
    else {
      sprintf(tweet, "%02d:%02d:%02d: %s", hour(timestamp), minute(timestamp), second(timestamp), exitMessage);  
    }

    Serial.println(tweet);
    Serial.print("Posting to Twitter: ");
    if (twitter.post_status(tweet)) {
      Serial.println("Status updated");
    }
    else {
      Serial.println("Update failed");
    }
  }
  delay(10000); // wait 10 seconds to avoid double triggering
}
