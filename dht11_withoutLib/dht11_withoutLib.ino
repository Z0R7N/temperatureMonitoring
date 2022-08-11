#define pin 2

float temperatureC;
int humi;

void setup() {
	Serial.begin(115200);
	
}


int wait_status(int time, int status) {
    int count = 0;
    while (digitalRead(pin) == status){
        if (count > time)
            return -1;
        delayMicroseconds(1);
        count++;
    }
    if (count == 0)
        return -1;
    return count;
}

void loop() {
	int res = 0;
    uint8_t data[5];
    while(1) {
		if (preparing_for_receiving_data() == -1) {
			Serial.println("not ready for receving data");
			continue;
		}
		
		for (int i = 1, j = 0; i < 41; i++) {
            if ((res = wait_status(50, 0)) == -1) {
				Serial.println("Error during sending data.");
                break;
            }
            if ((res = wait_status(70, 1)) == -1) {
                Serial.println("Error during sending data.");
                break;
            }

            if (res > 12) {
                data[j] <<= 1;
                data[j] += 1;
            }
            else
                data[j] <<= 1;

            if (i % 8 == 0)
                j++;
        }
        if (data[0] + data[1] + data[2] + data[3] != data[4]) {
            Serial.println("Invalid checksum");
            continue;
        }
		temperatureC = data[3] & 0x80 ? (data[2] + (1 - (data[3] & 0x7F) * 0.1)) * -1 : (data[2] + (data[3] & 0x7F) * 0.1);
		humi = data[0];
        Serial.print("Temperature: ");
		Serial.println(temperatureC);
        Serial.print("Humidity: ");
		Serial.println(humi, 1);
	
		Serial.println();
		delay(5000);
	}
}

int preparing_for_receiving_data() {
    // turning on data pin.
    pinMode(pin, OUTPUT); // Data pin.
    
    // turning off pin to the ground for 18 miliseconds.
    digitalWrite(pin, 0);
    delay(18);
    // turning on pin to the ground for 30 microseconds.
    digitalWrite(pin, 1);
    delayMicroseconds(30);
    pinMode(pin, INPUT_PULLUP);

    // Responses from server that confirm sensor is ready to send data.
    if (wait_status(80, 0) == -1) {
        printf("%s\n", "Wrong response from server: must have being returning 0 during 80 microseconds");
        return -1;
        }
    if (wait_status(80, 1) == -1) {
        printf("%s\n", "Wrong response from server: must have being returning 1 during 80 microseconds");
        return -1;
    }
    return 1;
}
