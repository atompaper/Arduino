

/* We use FastLED library for our project */
#include "FastLED.h"

/* A library we use to get the median of a set of values from the moisture sensor */
#include "RunningMedian.h"

/* We use RTClib library for the RTC clock module */
#include "RTCLib.h" 
#include <Wire.h>

/* Keypad buttons definition.  
 * We define all the keys in the keypad, we're using keys 1 and 4 for our prototype */
#define key1 9 
#define key2 10
#define key3 11
#define key4 12

/* Define the color order of the LED strip used */
#define COLOR_ORDER GRB

/* Define the chipset of the LED strip used */
#define LED_TYPE WS2812B

/* Define the number of LEDs we need to illuminate in the circle */
#define NUM_LEDS 45 

/* Define the number of LEDs we need to illuminate in the plant lamp */
#define NUM_LEDS_LAMP 4 

/* Arrays for FastLed to address individual LEDs */
CRGB leds[NUM_LEDS];
CRGB leds_lamp[NUM_LEDS_LAMP]; 

/* Define the pin we use for the LED strip in the circle */
#define DATA_PIN 5 

/* Define the pin we use for the LED strip in the plant lamp */
#define DATA_PIN 4

/* RTC Module 
 * We store the time from the RTC module in a char */
RTC_DS3231 rtc;
char t[32]; // 

/* We use an array that we fill with 10 moisture sensor values before we calculate the median of those values */
RunningMedian samples = RunningMedian(10); 

/* We use these two white LEDs to incidate which plant setting is on
 * We define a variable keyval to store which of the two keys (1 or 4) has been pressed */
int keyval;
const int ledL = 7; 
const int ledR = 8;

/* Declare a variable for the soil moisture sensor */
int soilPin = A0;

void setup()
{
  /* Starts the serial communication */
  Serial.begin(9600);

  /* Necessary code for the FastLED library */
  FastLED.addLeds<NEOPIXEL, 5>(leds, NUM_LEDS); 
  FastLED.addLeds<NEOPIXEL, 4>(leds_lamp, NUM_LEDS_LAMP);
  
  rtc.begin();

  /* Setup for Keypad keys */
  pinMode(key1, INPUT_PULLUP); 
  pinMode(key2, INPUT_PULLUP);
  pinMode(key3, INPUT_PULLUP);
  pinMode(key4, INPUT_PULLUP);

  /* Setup Setting LEDs as output */
  pinMode(ledL, OUTPUT); 
  pinMode(ledR, OUTPUT);
}

void loop()
{

  int key1S = digitalRead(key1);
  int key2S = digitalRead(key2);
  int key3S = digitalRead(key3);
  int key4S = digitalRead(key4);

  /* if key 1 is pressed (which represents setting 1 on the prototype) then set keyval to 1 
   * We have to check key2 because when we used the keypad we found that physically 1 is mapped to key2 */ 
  if (!key2S) {
     keyval = 1; 
  }

 /* if key 4 is pressed (which represents setting 2 on the prototype) then set keyval to 4 
  * We have to check key3 because when we used the keypad we found that physically 4 is mapped to key3 */ 
  if (!key3S) { 
     keyval = 4; 
  }

  /* We run the function loop2 from below here*/
  loop2(); 
}

void loop2() {
  
  /* define a variable that stores the values we get from the moisture sensor */
  long x = analogRead(soilPin); 
  
  /* We get 10 values of x and add them to a "samples" array as defined by the RunningMedian library 
   * j is a variable for counting to 10 */
  int j = 0;
  for (j = 0; j <= 10; j++)
  {
    samples.add(x);
  }

  /* Define a variable m whose value is equal to the median of 10 values stored in the "samples" array */
  float m = samples.getMedian(); 
  
  /* Define a variable z to store the hour value from the RTC Module */
  int z; 

  /* SETTING 1 -------------------------------------------------------
   * Check if setting 1 is selected by checking the value of keyval */
  if (keyval == 1) 
  { 
    /* Turns on one LED to indicate setting 1 is selected */
    digitalWrite(ledR,LOW);
    digitalWrite(ledL, HIGH);

    /*PLANT LAMP operation according to setting 1 */
    /* Gets the current time from RTC Module */
    /* Sets z as the current hour */
    /* Turns the lamp on if the time is between 8am and 2pm */
    /* Turns the lamp off if it's after 2pm or before 8am */
    DateTime now = rtc.now(); 
    sprintf(t, "%02d", now.hour());  
    z = now.hour();
    if (z >= 8 && z <= 14) 
    { 
       fill_solid (&(leds_lamp[0]), NUM_LEDS_LAMP, CRGB (255,255,255)); 
       FastLED.show();
    }
    else if (z>14 || z <8) 
    { 
       fill_solid (&(leds_lamp[0]), NUM_LEDS_LAMP, CRGB (0,0,0));
       FastLED.show();
    }

    /* CIRCLE LEDS operation according to setting 1 */
    /* UNDERWATERED
     * FadeRed is the function we use to get a fade-blinking red light
     * If the median moisture sensor value dips below 100, 
       the LED strip blinks in red color to indicate the soil is dry */
    if (m < 100) {
        FadeRed(); 
        FastLED.show(); 
    }

    /* NORMAL WATERED
     * When the moisture sensor values are between 100 and 400, 
       the circle LEDs glow in a changing shade of blue
     * The hue of the blue color depends on the value of m via 
       the linear function to set RGB to (0,240 - (2*m/5),255))
     * When the value of m is 100 the color is Light blue RGB(0,200,255)
     * When the value of m is 400 the color is Dark blue RGB(0,80,255) */
    else if (m >= 100 && m <= 400) 
    {
       fill_solid ( &(leds[0]), NUM_LEDS, CRGB (0,240 - (2*m/5),255));
       FastLED.show();
    }

    /* OVERWATERED
     * FadeBlue is the function we use to get a fade-blinking blue effect
     * If the median moisture sensor value exceeds 400, 
       the LED strip blinks in blue color to indicate overwatering */
    else if (m > 400) {
       FadeBlue(); 
       FastLED.show();
    }
     
  }

  /* SETTING 2 -------------------------------------------------------
   * Check if setting 2 is selected by checking the value of keyval */
  if (keyval == 4) { 

    /* Turns the other LED on to indicate setting 2 is selected */
    digitalWrite(ledR,HIGH);
    digitalWrite(ledL,LOW);

    /*PLANT LAMP operation according to setting 2 */
    /* Gets the current time from RTC Module */
    /* Sets z as the current hour */
    /* Turns the lamp on if the time is between 8am and 4pm */
    /* Turns the lamp off if it's after 4pm or before 8am */
    DateTime now = rtc.now(); 
    sprintf(t, "%02d", now.hour());
    z = now.hour();
    if (z >= 8 && z <= 16) 
    { 
       fill_solid (&(leds_lamp[0]), NUM_LEDS_LAMP, CRGB (255,255,255)); 
       FastLED.show();
    }
    else if (z > 14 || z < 8) 
    { 
       fill_solid (&(leds_lamp[0]), NUM_LEDS_LAMP, CRGB (0,0,0));
       FastLED.show();
    }

    /* CIRCLE LEDS operation according to setting 2 */
    /* UNDERWATERED
     * If the median moisture sensor value dips below 200, 
       the LED strip blinks in red color to indicate the soil is dry */
    if (m < 200) {
        FadeRed(); 
        FastLED.show(); 
    }

    /* NORMAL WATERED
     * When the moisture sensor values are between 200 and 500, 
       the circle LEDs glow in a changing shade of blue 
     * The mapping is slightly different than in setting 1 as the m values range has changed 
     * This time the linear function sets RGB to (0,280 - (2*m/5),255))*/
    else if (m >= 200 && m <= 500) 
    {
       fill_solid ( &(leds[0]), NUM_LEDS, CRGB (0,280 - (2*m/5),255));
       FastLED.show();
    }

    /* OVERWATERED
     * FadeBlue is the function we use to get a fade-blinking blue effect
     * If the median moisture sensor value exceeds 500, 
       the LED strip blinks in blue color to indicate overwatering */
    
    else if (m > 500) 
    {
       FadeBlue(); 
       FastLED.show();
    }
  }

  /* A condition that zeroes the variable we use for the median after we get ten values of x. 
   * This lets us calculate a fresh median for the next ten values */
  if (j=10)
  {
    m=0; 
    delay(100);
  } 
}

/*-------------------------------------------------------------*/

/* The function we call to make the LEDs fade-blink in Red color */
void FadeRed(){ // The function we call to make the LEDs blink in Red color
  for (int k =0; k<256; k++)
  { 
    /* The fill_solid code lets us color all the Leds we've defined with NUM_LEDS simultaneously
     * An increasing value of k, divided by 3, determines the Red value in the RGB scheme, with Green and Blue set to 0. 
     * This lets us get increasingly brighter shades of red*/
    fill_solid ( &(leds[0]), NUM_LEDS, CRGB (k/3,0,0));
    FastLED.show();
  }
  for (int k =256; k>=0; k=k-2)
  {
    /* Once k has reached its maximum value, we run a reverse cycle that decreases its value back to 0. 
     * This is how we get increasingly darker shades of red and, 
       combined with the cycle above, a fade - blinking effect */
    fill_solid ( &(leds[0]), NUM_LEDS, CRGB (k/3,0,0));
    FastLED.show();
  } 
}

/* Same as the FadeRed function, but used to get shades of blinking blue color on the LED strip */
void FadeBlue(){
  for (int k =128; k<256; k++){
    fill_solid ( &(leds[0]), NUM_LEDS, CRGB (0,k,k));
    FastLED.show();
    }
  for (int k =255; k>=128; k=k-2){
    fill_solid ( &(leds[0]), NUM_LEDS, CRGB (0,k,k));
    FastLED.show();
    }
}


      
  
