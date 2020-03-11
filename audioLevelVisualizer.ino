#include <FastLED.h>

#define NUM_LEDS 74
#define DATA_PIN_1 6 //right
#define DATA_PIN_2 5 //left
#define NUM_STRIPS 2
CRGB leds[NUM_STRIPS][NUM_LEDS];

#define INTERVAL 5.0
#define POWER 15

#define SENSIVITY_PIN A0
#define BRIGHTNES_PIN A1

#define BUTTON_PIN 2
int vv = 1000;

long currentMillis;

//COLORS//////////////////////////////////
int second = 26;
int thrid = 55;
int ombree = 5;
int color_step = (255/ombree);
void setColor(int strip ,int i){
    //blue
      if (i < second - ombree) leds[strip][i] = CRGB(0,0,255); 
    //from blue to green
      else if(i >= second - ombree and i < second) leds[strip][i] = CRGB(0,0 + ((i-second - ombree)*color_step) ,255 - ((i-second - ombree)*color_step)); 
    //green
      else if (i >= second  and i <= thrid - ombree) leds[strip][i] = CRGB(0,255,0); 
    // form green to red
      else if (i >= thrid - ombree and i < thrid) leds[strip][i] = CRGB(0 + ((i-thrid - ombree)*color_step), 255 - ((i-thrid - ombree)*color_step), 0);
    // red
      else if (i >= thrid) leds[strip][i] = CRGB(255,0,0);  
}
static bool isTwoChannel = false;
static int channelsValues[NUM_STRIPS] = {0,0};
void getMax(){
  float maxR = 0;
  float maxL = 0;
  currentMillis = millis();
  while (millis() - currentMillis < INTERVAL){
    float valR = analogRead(A3) * (float(NUM_LEDS) / 1023.0);
    float valL = analogRead(A5) * (float(NUM_LEDS) / 1023.0);
    maxR = max(maxR, valR);
    maxL = max(maxL, valL);
  }
  float sensivity = getSensivity();
  
  int maxVL = int(maxL*POWER*sensivity);
  int maxVR = int(maxR*POWER*sensivity);
  if (maxVL > NUM_LEDS) maxVL = NUM_LEDS;
  if (maxVR > NUM_LEDS) maxVR = NUM_LEDS;
  if (isTwoChannel){
    channelsValues[0] = maxVL;
    channelsValues[1] = maxVR;
  }
  else {
    int maxV = max(maxVL, maxVR);
    channelsValues[0] = maxV;
    channelsValues[1] = maxV;
  }
}

float getSensivity(){
  return analogRead(SENSIVITY_PIN) * (float(1.0) / 1023.0);
}

float getBrightnes(){
  return analogRead(BRIGHTNES_PIN) * (float(1.0) / 1023.0);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Init...");
  FastLED.addLeds<WS2812B, DATA_PIN_1, GRB>(leds[1], NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(leds[0], NUM_LEDS);
  FastLED.setBrightness(10);

  pinMode(BUTTON_PIN, INPUT);
  
}

void loop() {
  
    getMax();

   
    int brightnes = getBrightnes() * 100;
    float delayTime = 0.85 ; //0.8
    int buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == HIGH){
      isTwoChannel = !isTwoChannel;
    }
    FastLED.setBrightness(brightnes);

   
    for (int i = 0; i < NUM_LEDS; ++i){
      if (i < channelsValues[0]) {
        setColor(0,i); 
        delay(delayTime);
      }
      else leds[0][i] = CRGB::Black;   
      if (i < channelsValues[1]){
        setColor(1,i); 
        delay(delayTime);
      }
      else leds[1][i] = CRGB::Black;  
   }
//   FastLED.setBrightness(int((10 + max(channelsValues[0],channelsValues[1]) * 30/74)));
   
   FastLED.show();
}
