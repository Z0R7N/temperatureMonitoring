int pin = 5;

void setup() {
	Serial.begin(115200);
}

void loop() {
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
	
	// // start receiving data from sensor
	// // check for respons for sensor
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
			Serial.print("Humidity: ");
			Serial.println(data[0]);
            Serial.print("Temperature: ");
			Serial.println(data[3] & 0x80 ? (data[2] + (1 - (data[3] & 0x7F) * 0.1)) * -1 : (data[2] + (data[3] & 0x7F) * 0.1));
		}
	}
	Serial.println();
	delay(5000);
}

unsigned long timeLen(bool state, unsigned long timeout) {
    unsigned long micro = micros();
    while (digitalRead(pin) == state) {
        if (micros() - micro > timeout){
			Serial.println("timeout");
			delay(1000);
            // return 0;
		}
    }
    return micros() - micro;
} 