//  1 Channel Receiver | 1 Kanal Alıcı
//  PWM output on pin D5
#include <RF24.h>
#include <SPI.h>
#include <Servo.h>
#include <nRF24L01.h>
int ch_width_5 = 0;
int ch_width_7 = 0;
Servo ch5;
Servo ch7;
struct Signal {
  byte aux1;
  byte aux3;
};
Signal data;
const uint64_t pipeIn = 000322;
RF24 radio(9, 10);
void ResetData() {
  data.aux1 = 0; // Define the initial value of each data input.
  data.aux3 = 0; // The middle position for analog channels
}
void setup() {
  // Set the pins for each PWM signal
  ch5.attach(5);
  ch7.attach(7);

  ResetData(); // Configure the NRF24 module
  radio.begin();
  radio.openReadingPipe(1, pipeIn);
  radio.setChannel(100);
  radio.setAutoAck(false);
  radio.setDataRate(
      RF24_250KBPS); // The lowest data rate value for more stable communication
  radio.setPALevel(RF24_PA_MAX); // Output power is set for maximum
  radio.startListening();        // Start the radio comunication for receiver
}
unsigned long lastRecvTime = 0;
void recvData() {
  while (radio.available()) {
    radio.read(&data, sizeof(Signal));
    lastRecvTime = millis(); // Receive the data
  }
}
void loop() {
  recvData();
  unsigned long now = millis();
  if (now - lastRecvTime > 1000) {
    ResetData(); // Signal lost.. Reset data
  }
  ch_width_5 = map(data.aux1, 0, 255, 1000, 2000); // pin D5 (PWM signal)
  ch_width_7 = map(data.aux3, 0, 1, 1000, 2000);
  // ch_width_7 = map(data.aux3, 0, 1023, 0, 180);
  ch5.writeMicroseconds(ch_width_5); // Write the PWM signal
  ch7.writeMicroseconds(ch_width_7);
  // ch7.write(ch_width_7);
}