


float getDistance(int16_t encoderCount) {
  float rotasjoner = (float) encoderCount / (75.81 * 12.0);
  float cm = rotasjoner * 12.2522;
  return cm;
}

float getSpeed(float distance, unsigned long ms) {
  float cmSecond = distance / ((float) ms / 1000);
  return cmSecond;
}

void setup() {

}

void loop() {

}
