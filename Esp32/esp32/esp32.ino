//#include <SPI.h>

//static const int spiClk = 1000000; // 1 MHz
//SPIClass * hspi = NULL;

void setup() {
  Serial.begin(115200);
  //hspi = new SPIClass(HSPI);
  //hspi->begin(); 
  //pinMode(15, OUTPUT); //HSPI SS
}

void loop() {
  //hspiCommand();
  if(Serial.available())
  {
      String receivedString = Serial.readStringUntil('\n');
      
      Serial.println("OK");
  }
}
/*
void hspiCommand() {
  byte stuff = 0b11001100;
  
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(15, LOW);
  hspi->transfer(stuff);
  digitalWrite(15, HIGH);
  hspi->endTransaction();
  delay(100);
}
*/
