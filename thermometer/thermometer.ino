#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "DIR-1";
const char* password =  "3!r@fdQX";
String GAS_ID = "your code for google sheets";
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client;

int pin = 5;
int pinPower = 4;
float temp;
int humi;
byte tries = 15;


void setup() {
	Serial.begin(115200);
	WiFi.begin(ssid, password);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(pinPower, OUTPUT);
	
	delay(100);
	Serial.println();
	Serial.println();
	connectWifi();
}

void loop() {
	bool correctData = sensor();
	digitalWrite(LED_BUILTIN, 0);
	delay(20);
	bool cnct = checkConnection();
	
	if (!cnct) {
		digitalWrite(LED_BUILTIN, 1);
		connectWifi();
	} else {
		if (correctData){
			digitalWrite(LED_BUILTIN, 0);
			Serial.println();
			Serial.print("Humidity: ");
			Serial.println(humi);
			Serial.print("Temperature: ");
			Serial.println(temp);
			Serial.println();
			
			sendData();
			
			ESP.deepSleep(600e6);
		}
	}
	
}

void sendData() {
	digitalWrite(LED_BUILTIN, 1);
	if (!client.connect(host, httpsPort)) {
		Serial.println("connection failed");
		return;
	}
	
	String string_temperature =  String(temp);
	String string_humidity =  String(humi); 
	String url = "/macros/s/" + GAS_ID + "/exec?temp=" + string_temperature + "&humi=" + string_humidity;
	// Serial.print("requesting URL: ");
	// Serial.println(url);

	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
	 "Host: " + host + "\r\n" +
	 "User-Agent: BuildFailureDetectorESP8266\r\n" +
	 "Connection: close\r\n\r\n");

	Serial.println("request sent");
	
	
	// while (client.connected()) {
		// String line = client.readStringUntil('\n');
		// if (line == "\r") {
			// Serial.println("headers received");
			// break;
		// }
	// }
	// String line = client.readStringUntil('\n');
	// if (line.startsWith("{\"state\":\"success\"")) {
		// Serial.println("esp8266/Arduino CI successfull!");
	// } else {
		// Serial.println("esp8266/Arduino CI has failed");
	// }
	// Serial.print("reply was : ");
	// Serial.println(line);
	// Serial.println("closing connection");
	// Serial.println("==========");
	Serial.println();
	digitalWrite(LED_BUILTIN, 0);
}

bool checkConnection(){
	return WiFi.status() == WL_CONNECTED;
}

void connectWifi(){
	Serial.print("Connecting");
	while (--tries && WiFi.status() != WL_CONNECTED) {
		digitalWrite(LED_BUILTIN, 1);
		delay(250);
		digitalWrite(LED_BUILTIN, 0);
		Serial.print(".");
		delay(250);
	}
	tries = 15;
	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("Non Connecting to WiFi..");
		digitalWrite(LED_BUILTIN, 1);
	} else {
		digitalWrite(LED_BUILTIN, 0);
		Serial.println("");
		Serial.println("WiFi connected");
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
		client.setInsecure();
	}
}

bool sensor(){
	digitalWrite(LED_BUILTIN, 1);
	digitalWrite(pinPower, 1);
	delay(2000);
	// check for connection
	bool connectSns = true;
	// data from pin
	uint8_t data[5];
	// bit from pin
	uint8_t dataBit;
	// control summa
	uint8_t checkSum;
	// clear data
	for (int i = 0; i < 5; i++) {
        data[i] = 0;
	}
	uint8_t result [2];
	
	// start sending signal to sensor
	pinMode(pin, OUTPUT);
	digitalWrite(pin, 0);
    delay(18);
    pinMode(pin, INPUT_PULLUP);
	
	// int len = 35;
	// int cnt = 0;
	// int array[len];
	// bool stt;
	// while (cnt < len) {
		// stt = digitalRead(pin);
		// array[cnt] = timeLen(stt, 100);
		// cnt++;
	// }

	// for (int i = 0; i < len; i++) {
		// Serial.print(array[i]);
		// Serial.print(" ");
	// }
	
	
	// start receiving data from sensor
	// check for respons for sensor
	int x = timeLen(1, 20);
	if (x == 0) {
		Serial.print(x);
		Serial.print(" time, 1, ");
		Serial.println("DHT ERROR: NO REPLY 1");
		connectSns = false;
    }
	x = timeLen(0, 87);
    if (x == 0) {
		Serial.print(x);
		Serial.print(" time, 0, ");
        Serial.println("DHT ERROR: NO REPLY 2");
		connectSns = false;
    }
	x = timeLen(1, 90);
    if (x == 0) {
		Serial.print(x);
		Serial.print(" time, 1, ");
		Serial.println("DHT ERROR: NO REPLY 3");
		connectSns = false;
    }
	// receiving data from sensor
	if (connectSns){
		for (int i = 0; i < 40; i++) {
			dataBit = timeLen(0, 57);
			dataBit = timeLen(1, 100);
			if (dataBit) {
				data[i / 8] <<= 1;
				data[i / 8] += dataBit > 45 ? 1 : 0;
			} else {
				Serial.println("DHT ERROR: TIMEOUT");
				break;
			}
		}
		
		// check up control summa
		checkSum = data[0] + data[1] + data[2] + data[3];
		if (data[4] != checkSum) {
			Serial.println("DHT ERROR: CHECKSUM");
			connectSns = false;
		}
		
		if (connectSns) {
			// print data
			humi = data[0];
			temp = data[3] & 0x80 ? (data[2] + (1 - (data[3] & 0x7F) * 0.1)) * -1 : (data[2] + (data[3] & 0x7F) * 0.1);
			
		} else {
			temp = -1000;
			humi = -1;
		}
	}
	digitalWrite(pinPower, 0);
	return connectSns;
}

unsigned long timeLen(bool state, unsigned long timeout) {
    unsigned long micro = micros();
    while (digitalRead(pin) == state) {
        if (micros() - micro > timeout){
			// Serial.print(micros() - micro);
			// Serial.println(" timeout");
			// return micros() - micro;
            return 0;
		}
    }
    return micros() - micro;
} 