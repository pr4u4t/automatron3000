void setup() {
  // Initialize the serial communication at 9600 baud
  Serial.begin(9600);

  // Seed the random number generator with an analog pin
  randomSeed(analogRead(0));
}

void loop() {
  // Generate 4 random integers
  int random1 = random(0, 101); // Random number between 0 and 100
  int random2 = random(0, 101);
  int random3 = random(0, 101);
  int random4 = random(0, 101);

  // Send the integers as a comma-separated line with \r\n at the end
  Serial.print(random1);
  Serial.print(",");
  Serial.print(random2);
  Serial.print(",");
  Serial.print(random3);
  Serial.print(",");
  Serial.print(random4);
  Serial.print("\r\n");

  // Wait for 1 second before sending the next set
  delay(1000);
}

