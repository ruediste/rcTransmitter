/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use the Serial Monitor to change each node's behavior.
 */
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// instantiate an object for the nRF24L01 transceiver
RF24 radio(PB0, PA4); //CE , CSN

// Let these addresses be used for the pair, 1: sender->transmitter 2: receiver->transmitter
uint8_t address[][6] = {"00001", "00002"};
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

const int pinRead = PIN_A3;

void setup()
{

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("I2C LCD with");
  lcd.setCursor(0, 1);
  lcd.print("STM32F103C8T6");

  // initialize the transceiver on the SPI bus
  if (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1)
    {
    } // hold in infinite loop
  }

  // print example's introductory prompt
  Serial.println(F("RF24/Tranmitter"));

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(float)); // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[0]);
  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[1]); // using pipe 1

  radio.stopListening(); // put radio in TX mode

  // For debugging info
  // printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails(); // (larger) function that prints human readable data

} // setup

void loop()
{

  // This device is a TX node

  float payload = analogRead(pinRead) / 1024.;
  unsigned long start_timer = micros();               // start the timer
  bool report = radio.write(&payload, sizeof(float)); // transmit & save the report
  unsigned long end_timer = micros();                 // end the timer

  if (true){
    if (report)
    {
      Serial.print(F("Transmission successful! ")); // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer); // print the timer result
      Serial.print(F(" us. Sent: "));
      Serial.println(payload); // print payload sent
      payload += 0.01;         // increment float payload
    }
    else
    {
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
    }
}
  //delay(500);

} // loop
