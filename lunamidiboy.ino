
#include <Midiboy.h>
#include "Arduboy.h"
Arduboy arduboy;

//#define frequencyPot   100                              //frequency pot tied to pin 15 which is A1 - 0 and 1023
#define tempoPot       1                              //tempo pot tied to pin 16 which is A2
#define buttonPin      9                              //programming button tied to pin 17 which is A3
#define ledPin         10                             //status led tied to pin 18 which is A4
#define speakerPin     11                             //speaker or output pin 19 whish is A5
//if you use a speaker it should be at least a 16 ohm speaker an should have a
//resistor, maybe 200ohm to 1K ohm, between the negative lead and ground.
//a potentiometer would be even better.

int currentStep = 0;                                  //this is just to track which tone in memory we are currently playing

int steps[] = {500, 500, 100, 100, 100, 100, 100, 100,             // this is our tone storage areae
               100, 100, 100, 100, 100, 100, 100, 100,              //I used 64 tones or 8 tones per beat
               500, 500, 100, 100, 100, 100, 100, 100,              //you can change these manually and experiment if you like
               100, 100, 100, 100, 100, 100, 100, 100,
               500, 500, 100, 100, 100, 100, 100, 100,
               100, 100, 100, 100, 100, 100, 100, 100,
               500, 500, 100, 100, 100, 100, 100, 100,
               100, 100, 100, 100, 100, 100, 100, 100
              };

int tempo = 10;                                      //tempo or speed between tones
int duration = 0;                                   //how long each of the 64 tones plays
int frequency = 0;                                  //current tone
int pitchval = 1;
int frequencyPot = 100;
char textBuf[23];

void setup() {
  /*
     Midiboy.begin();
     Midiboy.setButtonRepeatMs(50);
  */
  arduboy.begin();
  arduboy.setFrameRate(15);
  pinMode(PIN_SPEAKER_1, OUTPUT);

  /*pinMode (frequencyPot, INPUT);
    pinMode (tempoPot, INPUT);
    pinMode (buttonPin, INPUT);
    digitalWrite(buttonPin, HIGH);

    pinMode (ledPin, OUTPUT);
    pinMode (speakerPin, OUTPUT);  */
}

void print_cur(String Bytes) {
  arduboy.setCursor(0, 0);
  arduboy.print(Bytes);
}
// Display message at specified location in specified font size.
void printText(const char *message, byte x, byte y, byte textSize) {
  arduboy.setCursor(x, y);
  arduboy.setTextSize(textSize);
  arduboy.print(message);
}

/* Sound definitions */
byte sound1(int i) {
  return i / 13 >> (1 + ((i >> 12) & 3)) | i / 2 >> 2 & (i / 6) >> 7 | i & 31 * i * (i >> 8);
}
byte sound2(int i) {
  return (i / 3 >> (i % 40 + 5) | i / (24 + i & 3) >> (i % (15 - ((i >> 15) % 8) * 6) + 5)) / 8;
}

byte sound3(int i) {  
  return   ( (i * 15) & (i >> 5)) | ((i *2) & (i>>7)) | ((i*8)&(i>>11)) * (i >> 8);
}

void freqout(int freq, int t) {
  int hperiod;
  long cycles, i;
  
  hperiod = (500000 / ((freq - 7) * pitchval));

  cycles = ((long)freq * (long)t) / 1000;

  for (i = 0; i <= cycles; i++)
  {
    digitalWrite(PIN_SPEAKER_1, HIGH);
    //digitalWrite(PIN_SPEAKER_1, a % 2);
    delayMicroseconds(hperiod);
    digitalWrite(PIN_SPEAKER_1, LOW);
    delayMicroseconds(hperiod - 1);
  }
}

void buttonChecks() {
  if (arduboy.pressed(UP_BUTTON) )   {
    if (frequencyPot < 1023) {
      frequencyPot += 7;
    }
  }
  if (arduboy.pressed(DOWN_BUTTON) ) {
    if (frequencyPot > 0) {
      frequencyPot -= 7;
    }
  }
  if (arduboy.pressed(LEFT_BUTTON) ) {
    if (tempo <256 ) {
      tempo += 1;
    }
  }
  if (arduboy.pressed(RIGHT_BUTTON) ) {
    if (tempo > 0) {
      tempo -= 1;
    }
  }
}

void loop() {


  if (!(arduboy.nextFrame()))
    return;

  arduboy.clear();


  print_cur(F("Dur:     Freq:"));
  sprintf(textBuf, "%d", tempo);
  printText(textBuf, 34, 0, 1);
  sprintf(textBuf, "%d", frequencyPot);
  printText(textBuf, 90, 0, 1);

  for (int i = 0; i < 63; i++)                     //64 individual notes played
  {

    currentStep = i;                                 //save our current position in the loop for later
    int y = steps[i] % 64; 
    arduboy.drawPixel( i*4 % 72, y , 1);

    // checkset values from buttons.
    buttonChecks();

    if ( arduboy.pressed(B_BUTTON) )               //is the program button being pressed
    { //if so lets write a new tone the this location

      steps[currentStep] = (frequencyPot);  //read the frequency pot and set the new tone actual set up UP and Down
      freqout (steps[currentStep], duration);           //set the parameters for frequout below and play it
      freqout (steps[currentStep] + 64, duration);     //play another tone a little bit different than the original to give
      //it a little more depth. this can be changed to your liking
      freqout (steps[currentStep] + 128, duration);     //play another tone a little bit different than the original to give
      //it a little more depth. this can be changed to your liking
    } else {                                            //else play the tone

      freqout (steps[currentStep], duration);           //set the parameters for frequout below and play it
      freqout (steps[currentStep] + 64, duration);     //play another tone a little bit different than the original to give
      //it a little more depth. this can be changed to your liking
      freqout (steps[currentStep] + 128, duration);     //play another tone a little bit different than the original to give
      //it a little more depth. this can be changed to your liking


    }
    // tempo = (analogRead(tempoPot)/4);                    //read the tempo pot
    duration = tempo / 8;                                 //set the individual tone durations
    delay(tempo);                                         //wait a bit
  }
  arduboy.display();
}
