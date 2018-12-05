#include<arduino.h>

#define IR_Send D1
#define IR_RCV D2
#define LED D3

const unsigned int measure_duration = 1000; //measure duration in ms
const unsigned int frequency = 38000; //frequency in Hz
const unsigned int interval = 360; //sleep time between measurements in minutes

unsigned long last_pulse = 0;
unsigned int pulse_count =0;

void pulseDetect(){
	//check if the last pulse is at least 80% of a period past to prevent double counts
	if (last_pulse + (1000000/frequency)*0.8 <= micros()){ 
		pulse_count++;
		last_pulse = micros();
	}
}

void setup(){
	serial.beginn(74880);
	//pinMode(IR_Send, OUTPUT);
	analogWriteFreq(frequency); //set the PWM frequency to desired value (ESP8266 supports 1k-100k)
	analogWriteRange(2); //set the PWM possibilities (default 255 or 1023). Here 0=off, 1=50%, 2=100% 
	pinMode(LED, OUTPUT);
	pinMode(IR_RCV, INPUT);
	attachInterrupt(digitalPinToInterrupt(IR_RCV), pulseDetect, FALLING); //attaching an interrupt on falling edge to that PIN
}

void loop(){
	pulse_count = 0;
	digitalWrite(LED, HIGH); //indicate measurement with constant light
	analogWrite(IR_Send, 1); //turn on the IR LED => now the interrupt should trigger multiple times
	delay(measure_duration); //keep it on for defined period
	analogWrite(IR_Send, 0); //turn off the IR LED
	digitalWrite(LED, LOW); //deactivate measurement indication
	serial.Print("Pulses detected: ");
	serial.Println(pulse_count);
	//all/most pulses are beeing detected
	if (pulse_count >= ((measure_duration/1000)/frequency*0.8)){
		Serial.println("Light Barrier active -> no food available! Activating Alarm!");
		for(int i=0; i<10; i++){
			digitalWrite(LED, HIGH);
			delay(1000);
			digitalWrite(LED, LOW);
			delay(1000);
		}
	}
	//TODO: for testing, will be replaced by deep sleep
	delay(10000);	
}
