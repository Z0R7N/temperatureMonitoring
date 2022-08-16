int pin = 5;
int cnt = 0;

void setup() {
	Serial.begin(115200);
}

void loop() {
	pinMode(pin, OUTPUT);
	digitalWrite(pin, 0);
    delay(18);
	digitalWrite(pin, 1);
    delayMicroseconds(30);
    pinMode(pin, INPUT_PULLUP);
	while(cnt < 110){
		cnt++;
		Serial.print(digitalRead(pin));
		delayMicroseconds(10);
	}
	Serial.println();
	cnt = 0;
	delay(5000);
}

unsigned long timeLen(uint8_t pin, bool state, unsigned long timeout) {
    unsigned long micro = micros();
    while (digitalRead(pin) == state) {
        if (micros() - micro > timeout)
            return 0;
    }
    return micros() - micro;
}