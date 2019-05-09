
/* We use FastLED library for our project */
#include "FastLED.h"

/* Define the color order of the LED strip used */
#define COLOR_ORDER GRB

/* Define the chipset of the LED strip used */
#define LED_TYPE WS2812B

/* Define the number of LEDs we want to use */
#define NUM_LEDS 45

/* Array for FastLed to address individual LEDs */
CRGB leds[NUM_LEDS];

/* Define the pin we use for the LED strip */
#define DATA_PIN 6 

/* Ultrasonic proximity Sensors
 * Define the pins for the four proximity sensors */
const int trigPinA = 9;
const int echoPinA = 10;
const int trigPinB = 8;
const int echoPinB = 7;
const int trigPinC = 4;
const int echoPinC = 5;
const int trigPinD = 3;
const int echoPinD = 2; 
long durationA, durationB, durationC, durationD; 
int distanceA, distanceB, distanceC, distanceD;


void setup() 
{ 
  /* Starts the serial communication */
  Serial.begin(9600); 
  
  /* Necessary code for the FastLED library */
  FastLED.addLeds<NEOPIXEL, 6>(leds, NUM_LEDS); 

  /* Set the various trigPins as an Outputs and echoPins as Inputs */
  pinMode(trigPinA, OUTPUT); 
  pinMode(echoPinA, INPUT);
  pinMode(trigPinB, OUTPUT); 
  pinMode(echoPinB, INPUT);
  pinMode(trigPinC, OUTPUT); 
  pinMode(echoPinC, INPUT);
  pinMode(trigPinD, OUTPUT); 
  pinMode(echoPinD, INPUT);
}

void loop() 
{
  /* Calculate distance for first ultrasonic sensor */
  /* Clear the trigPin */
  digitalWrite(trigPinA, LOW); 
  delayMicroseconds(2);
  /* Set the trigPin on HIGH state for 10 microseconds */
  digitalWrite(trigPinA, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPinA, LOW); 
  /* Read the echoPin and returns the sound wave travel time in microseconds */
  durationA = pulseIn(echoPinA, HIGH);
  /* Calculate how far the person is (in cm) from the proximity sensor */
  distanceA = durationA*0.034/2; 

  /* Calculate distance for second ultrasonic sensor */
  digitalWrite(trigPinB, LOW);
  delayMicroseconds(2); 
  digitalWrite(trigPinB, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPinB, LOW);
  durationB = pulseIn(echoPinB, HIGH); 
  distanceB= durationB*0.034/2; 

  /* Calculate distance for third ultrasonic sensor */
  digitalWrite(trigPinC, LOW);
  delayMicroseconds(2); 
  digitalWrite(trigPinC, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPinC, LOW);
  durationC = pulseIn(echoPinC, HIGH); 
  distanceC= durationC*0.034/2; 

  /* Calculate distance for fourth ultrasonic sensor */
  digitalWrite(trigPinD, LOW);
  delayMicroseconds(2); 
  digitalWrite(trigPinD, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPinD, LOW);
  durationD = pulseIn(echoPinD, HIGH); 
  distanceD= durationD*0.034/2; 

  /* Delay of 500 microseconds after all the data from the proximity sensors has been read */
  delay(500); 
 
  /* We use a distance of less than 30 cm as a measure of being 'near' the plant. 
   * We now test which one of four scenarious is true: 
   * Four people near, three people near, two people near, one person near or no one near*/

  
  /* FOUR PEOPLE NEAR
   * Condition that can only be met if four people are standing around the plant 
   * Output on the LED strip to visualize the plant's "mood" when four people are near 
   * All LEDs defined by NUM_LEDS fade-blink orange 
   * FadeOrange is a function defined further below */
  if (distanceA < 30 && distanceB < 30 && distanceC <30 && distanceD <30) 
  { 
    FadeOrange(); 
    FastLED.show();
  }

  
  /* THREE PEOPLE NEAR
   * Condition that can only be met if three people are standing near the plant*/
  else if ((distanceA < 30 && distanceB < 30 && distanceC < 30 && distanceD > 30) 
            || (distanceA < 30 && distanceB < 30 && distanceC > 30 && distanceD < 30) 
            || (distanceA < 30 && distanceB > 30 && distanceC < 30 && distanceD < 30) 
            || (distanceA > 30 && distanceB < 30 && distanceC < 30 && distanceD < 30))
  { 
    /* We define a variable minthree that uses the default minimum 
       function to find the closest distance to the prototype */
    int minthree = min(min(distanceA, distanceB), min(distanceD, distanceC)); 
    
    /* Output of the LED strip to visualize the plant's "mood" when three people are near. 
     * All LEDS from 0 to NUM_LEDS (49) are colored in RGB 47, 242, 21 - bright breen*/
    fill_solid( &(leds[0]), NUM_LEDS, CRGB(47, 242, 21)); 

    /* Set the brightness of the LED strip to be equal to a linear function of the value of minthree
     * The function sets brightness to (155*((1/minthree) + 0.6))
     * This maps 1 cm to maximum brightness (255) and 30 cm to minimum brightness (100)
     * This achieves the effect of making the LED strip brighter or dimmer, 
       depending on how close person is */
    FastLED.setBrightness (155*((1/minthree)+0.6)); 
    FastLED.show();
  }

  
  /* TWO PEOPLE NEAR
   * Condition that can only be met if two people are standing near the plant */
  else if ((distanceA < 30 && distanceB < 30 && distanceC > 30 && distanceD > 30) 
            || (distanceA < 30 && distanceB > 30 && distanceC < 30 && distanceD > 30) 
            || (distanceA < 30 && distanceB > 30 && distanceC > 30 && distanceD < 30) 
            || (distanceA > 30 && distanceB < 30 && distanceC < 30 && distanceD > 30) 
            || (distanceA > 30 && distanceB < 30 && distanceC > 30 && distanceD < 30) 
            || (distanceA > 30 && distanceB > 30 && distanceC < 30 && distanceD < 30))
  { 
    /*Same as above*/
    int mintwo = min(min(distanceA, distanceB), min(distanceC, distanceD)); 
    
    /* Output of the LED strip to visualize the plant's "mood" when two people are near 
     * All LEDS from 0 to NUM_LEDS (49) are colored in RGB 147, 211, 19 - yellow green */
    fill_solid( &(leds[0]), NUM_LEDS, CRGB(147, 211, 19)); 
    
    /*Same as above*/
    FastLED.setBrightness (155*((1/mintwo)+0.6)); 
    FastLED.show();
  }
  
  
  /* ONE PERSON NEAR
   * Condition that can only be met if one person is standing near the plant.*/    
  else if ((distanceA < 30 && distanceB > 30 && distanceC > 30 && distanceD > 30) 
            || (distanceA > 30 && distanceB < 30 && distanceC > 30 && distanceD > 30) 
            || (distanceA > 30 && distanceB > 30 && distanceC < 30 && distanceD > 30) 
            || (distanceA > 30 && distanceB > 30 && distanceC > 30 && distanceD < 30))
  { 
    /*Same as above*/
    int minone = min(min(distanceA, distanceB), min(distanceC, distanceD)); 
    
    /* Output of the LED strip to visualize the plant's "mood" when one person is near. 
     * All LEDS from 0 to NUM_LEDS (49) are colored in RGB 200,200,200 - white*/
    fill_solid( &(leds[0]), NUM_LEDS, CRGB(200, 200, 200)); 
    
    /*Same as above*/
    FastLED.setBrightness (155*((1/minone)+0.6)); 
    FastLED.show();
  }
       
  
  /* NO ONE NEAR
   * If all above conditions are false then nobody is near the plant */
  else 
  { 
    /* Output of the LED strip to visualize the plant's "mood" when nobody is near. 
     * All LEDS from 0 to NUM_LEDS (49) are colored in RGB 50, 50, 50 - white with low brightness */
    fill_solid( &(leds[0]), NUM_LEDS, CRGB(50, 50, 50)); 
    FastLED.show();
  }
    
}

/*-------------------------------------------------------------*/

/* The function we call to make the LEDs fade-blink in Orange color */
void FadeOrange()
{ 
  for (int k =0; k<256; k++)
  {
    /* The fill_solid code lets us color all the Leds we've defined with NUM_LEDS simultaneously 
     * An increasing value of k with the ratios between Red and Green set as 1.7 gives orange shade. 
     * This lets us get increase the brightness of orange */
    fill_solid ( &(leds[0]), NUM_LEDS, CRGB (k,k/1.7,0));
    FastLED.show();
  }

  for (int k =256; k>=0; k=k-2)
  {
    /* Once k has reached its maximum value, we run a reverse cycle that decreases its value back to 0. 
     * This is how we get increasingly darker shades of orange and, 
       combined with the cycle above, a fade blinking effect */
    fill_solid ( &(leds[0]), NUM_LEDS, CRGB (k,k/1.7,0));
    FastLED.show();
  } 
}

    
