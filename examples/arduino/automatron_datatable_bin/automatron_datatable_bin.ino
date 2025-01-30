#pragma pack(push, 1)
struct DataPacket {
  const uint8_t magic[3] = {0xf1, 0xf2, 0xf3};
  int32_t random1;
  int32_t random2;
  int32_t random3;
  int32_t random4;
};
#pragma pack(pop)

void setup() {
  // Initialize serial communication at 9600 baud
  Serial.begin(9600);

  // Seed the random number generator with an analog pin
  randomSeed(analogRead(0));
}

void loop() {
  // Create an instance of the DataPacket struct
  DataPacket packet;

  // Assign random values to the struct fields
  packet.random1 = random(0, 101);
  packet.random2 = random(0, 101);
  packet.random3 = random(0, 101);
  packet.random4 = random(0, 101);

  // Send the struct over Serial as binary data
  Serial.write((uint8_t*)&packet, sizeof(packet));

  // Wait for 1 second before sending the next set
  delay(1000);
}
