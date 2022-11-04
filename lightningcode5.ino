#include <FastLED.h>

//config
//some good ones: maxL = 20, maxSim = 12
const short max_times = 5; //max times a flash will repeat
const int flash_delay_min = 7;
const int flash_delay_max = 20;
const short max_sim = 1; //max simultaneous flashes
const int fcol [] = {150,175,255}; //flash color //light blue
const int ncol[] = {2,25,40};
const int ecol [] = {ncol[0]+10,ncol[0]+10,ncol[2]+40};
const short maxLength = 100; //max flash length (leds) was 35
const short minLength = 50; // min flash length
const short ext = 0; //how far to make the light flash extend (the dimmer part)
const int numLeds = 200; //was 195
const int max_wait = 1500; //time to wait before issuing a new sequence
const int max_tries_to_enter = 10; //max number of times random coords will be generated when adding a sequence

CRGB leds[numLeds];
typedef struct 
  {
    bool on; //currently on or off
    bool valid;
    int st; //start index
    int en; //end index
    int maxF; // max # of flashes
    int del; //flash delay
    int timesF; //times flashed
    long nextT; //time of next update
  } flash_t;

flash_t flashes [max_sim];

void setup(){
  FastLED.addLeds<WS2812, 2, GRB>(leds, numLeds);
  Serial.begin(9600);
  for(int i = 0; i<max_sim; ++i){
    flashes[i].valid = false;
    flashes[i].nextT = 0;
  }
  for(int i = 0; i<numLeds; ++i){
    leds[i] = CRGB(ncol[0],ncol[1],ncol[2]);
  }
  FastLED.show();
}

void setSP(flash_t* f){
  int count = 0;
  while(count < max_tries_to_enter){
    int start = micros()%(numLeds-minLength); //adjust this
    int fin = start+ (micros()%(maxLength-minLength))+minLength;
    if(fin >= numLeds){
      fin = numLeds-1;
    }
    bool ok = true;
    flash_t check;
    for(int i = 0; i<max_sim; ++i){
      check = flashes[i];
      if(check.valid){
        if(start >= check.st && start < check.en){
          ok = false;
        }
        if(fin >= check.st && fin <= check.en){
          ok = false;
        }
      }
    }
    if(ok){
      f->st = start;
      f->en = fin;
      f->valid = true;
      return;
    }
    count++;
  }
}


void flashUpdate(flash_t* f){
  if(millis() >= f->nextT){
  if(f->valid == false && millis()>= f->nextT){ //make a new flash
    //make values
    setSP(f);
    int fdel = (micros()%(flash_delay_max-flash_delay_min))+flash_delay_min;
    int maxFlashes = (micros()%max_times)+1;
    //put em in
    f->del = fdel;
    f->maxF = maxFlashes;
    //set other vals
    f->on = false;
    //f->valid = true;
    f->timesF = 0;
    f->nextT = 0;
  }
  //time to update
  if(f->valid && millis()>= f->nextT){
    //currently mid-flash
    if(f->on){
      //turn off the lights
      for(int j = (f->st)-ext; j <= (f->en)+ext; ++j){
        if(j >=0 && j<numLeds){
          leds[j] = CRGB(ncol[0],ncol[1],ncol[2]);
        }
      }
      f->on = false;
      f->nextT = millis()+(f->del);
      FastLED.show();
      //if done flashing, make it invalid
      if(f->maxF == f->timesF){
        f->valid = false;
        f->nextT = millis()+(micros()%max_wait);
      }
    }
    else{
      //turn on lights (main flash)
      for(int j = f->st; j<= (f->en); j++){
        leds[j] = CRGB(fcol[0],fcol[1],fcol[2]);
      }
      //turn on lights (flash extend)
      
      for(int j = 1; j<=ext; j++){
        if((f->en)+j < numLeds){
          leds[(f->en)+j] = CRGB(ecol[0],ecol[0],ecol[2]);
        }
        if((f->st)-j >= 0){
          leds[(f->st)-j] = CRGB(ecol[0],ecol[0],ecol[2]);
        }
      }
      
      FastLED.show();
      //do other stuff
      f->on = true;
      f->timesF = (f->timesF)+1;
      f->nextT = (f->del)+millis();
    }
  }
  }
}

void printFlash(int index){
  Serial.print(index);
  Serial.print(": Start: ");
  Serial.print(flashes[index].st);
  Serial.print(" End: ");
  Serial.print(flashes[index].en);
  Serial.print(" #fs: ");
  Serial.print(flashes[index].timesF);
  Serial.print(" maxF: ");
  Serial.print(flashes[index].maxF);
  Serial.print(" nextUpdate: ");
  Serial.print(flashes[index].nextT);
  Serial.println();
  
}

void loop(){
  for(int i = 0; i<max_sim; ++i){
    //printFlash(i);
    flashUpdate(&flashes[i]);
  }
}
