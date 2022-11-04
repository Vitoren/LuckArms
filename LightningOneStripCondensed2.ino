 #include <FastLED.h>

//config
//some good ones: maxL = 20, maxSim = 12
const short max_times = 5; //max number times a flash can repeat

//range was 15-35

const int flash_delay_min = 12; //7 minimum delay between a flash turning on and off was 5 15
const int flash_delay_max = 35; //15 45 maximum delay between a flash turning on and off
const short max_sim = 3; //max simultaneous flashes 8
//const int fcol [] = {70,70,255}; //flash color //blue-white (when on)
const int fcol [] = {120,120,255}; //flash color //blue-white (when on) -> was 80,90,220
const int ncol [] = {1,5,30};
//const int ncol[] = {0,0,0};
const short maxLength = 180; //max flash length (leds) was 35
const short minLength = 70; // min flash length

const int max_wait = 1500; //maximum time to wait before issuing a new flash section after one finishes 1500
const int max_tries_to_enter = 3; //max number of times random coords will be generated when trying to add a new flash block

//Number of LEDS per strip
const int numLeds = 288;
//Total # of leds across all strips
const int total_leds = 1300;

//global var (for efficiency)
int count = 0;
int start;
int fin;
bool ok;
int fdel;
int maxFlashes;
int i;
int j;
int temp;

int stripNum;
//int sel;

//Pin numbers
int sel0 = 5;
int sel1 = 6;
int sel2 = 7;

int switchDelay = 1;

CRGB leds[numLeds];

//led boolean strips
//byte strip[numLeds];
byte strip[numLeds];

typedef struct //data structure for a flash block
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

flash_t check;
void setup(){
  //Set the ports
  pinMode(sel0,OUTPUT); //A
  pinMode(sel1,OUTPUT); //B
  pinMode(sel2,OUTPUT); //C
  
  FastLED.addLeds<WS2812, 3, GRB>(leds, numLeds);
  Serial.begin(9600);

  //set the flash array to be all invalid and be updated when first ran
  for(i = 0; i<max_sim; ++i){
    flashes[i].valid = false;
    flashes[i].nextT = millis()+i*100;
  }
  //Set all the leds to the non-flash color
  for(i = 0; i<numLeds; ++i){
    leds[i] = CRGB(ncol[0],ncol[1],ncol[2]);
  }

  digitalWrite(sel0,LOW);
  digitalWrite(sel1,LOW);
  digitalWrite(sel2,LOW);
  delay(switchDelay);
  FastLED.show();
  delay(switchDelay);
  digitalWrite(sel0,HIGH);
  delay(switchDelay);
  FastLED.show();
  delay(switchDelay);
  digitalWrite(sel0,LOW);
//  sel=0;
}

bool getBit(int index){
    return strip[index%numLeds]&(1<<(index/numLeds));
}

void setBit(int index,bool value){
    if(value){
        strip[index%numLeds]=strip[index%numLeds]|(1<<(index/numLeds));
    }
    else{
        strip[index%numLeds]=strip[index%numLeds]&(~(1<<(index/numLeds)));
    }
}

void setPorts(){
    if(stripNum==0){
      digitalWrite(sel0,LOW);
      digitalWrite(sel1,LOW);
      digitalWrite(sel2,LOW);
      //sel=0;
    }
    else if(stripNum == 1){
      digitalWrite(sel0,HIGH);
      digitalWrite(sel1,LOW);
      digitalWrite(sel2,LOW);
     // sel=1;
    }
    else if(stripNum == 2){
      digitalWrite(sel0,LOW);
      digitalWrite(sel1,HIGH);
      digitalWrite(sel2,LOW);
     // sel=2;
    }
    else if(stripNum == 3){
      digitalWrite(sel0,HIGH);
      digitalWrite(sel1,HIGH);
      digitalWrite(sel2,LOW);
    //  sel=3;
    }
    else if(stripNum == 4){
        digitalWrite(sel0,LOW);
        digitalWrite(sel1,LOW);
        digitalWrite(sel2,HIGH);
    }
    else if(stripNum == 5){
        digitalWrite(sel0,HIGH);
        digitalWrite(sel1,LOW);
        digitalWrite(sel2,HIGH);
    }
    //delay(switchDelay);
}

void showLeds(int st, int en){
    for(i=st;i<=en;++i){
        //Change to a different strip if necessary
        if(stripNum != i/numLeds){
            if(i != st){ //no unshown changes have been made if i=st when the stripnum is different
                FastLED.show();
            }
            stripNum=i/numLeds;
            setPorts();
            //Sync the LED array with packed data
            for(j = 0; j<numLeds; j++){
                if(getBit(j + stripNum*numLeds)){
                    leds[j] = CRGB(fcol[0],fcol[1],fcol[2]);
                }
                else{
                    leds[j] = CRGB(ncol[0],ncol[1],ncol[2]);
                }
            }
        }
        //Set the LED array
        if(getBit(i)){
            leds[i%numLeds] = CRGB(fcol[0],fcol[1],fcol[2]);
        }
        else{
            leds[i%numLeds] = CRGB(ncol[0],ncol[1],ncol[2]);
        }
    }
    FastLED.show();
}

//finds a valid start position for a flash block
void setSP(flash_t* f, flash_t flashes[]){
    for(count = 0; count < max_tries_to_enter; ++count){
    //generate random start and end points
    start = micros()%(total_leds-minLength);
    fin = start+ (micros()%(maxLength-minLength))+minLength;
    //make the flash block in bounds if it exceeds the max number of leds
    if(fin >= total_leds){
      fin = total_leds-1;
    }
    //ensure the flash block doesnt overlap with another
    ok = true;
    for(i = 0; i<max_sim; ++i){
      check = flashes[i];
      if(check.valid){
        if(start >= check.st && start < check.en){
          ok = false;
          break;
        }
        if(fin >= check.st && fin <= check.en){
          ok = false;
          break;
        }
      }
    }
    //if the block doesnt overlap with another, set the start and end values, make it valid and return.
    if(ok){
      f->st = start;
      f->en = fin;
      f->valid = true;
      return;
    }
  }
}

//update each of the flash blocks

void flashUpdate(flash_t* f,flash_t flashes[]){
  if(millis() >= f->nextT){
    if(f->valid){
      if(f->on){ //turn it off
        for(i = f->st; i<=f->en;++i){
          //strip[i] = false;
          setBit(i,0);
        }
        f->on = false;
        f->nextT = millis()+(f->del);
        showLeds(f->st,f->en);
       
        if(f->maxF == f->timesF){
          f->valid = false;
          f->nextT = millis() + (micros()%max_wait);
        }
      }
    //turn lights on
      else{
        for(i = f->st; i<= f->en; ++i){
          setBit(i,1);
        }
        showLeds(f->st,f->en);
        f->on = true;
        f->timesF = (f->timesF)+1;
        f->nextT = (f->del)+millis();
      }
    }
    else{ //flash invalid, make a new one
      setSP(f,flashes);
      fdel = (micros()%(flash_delay_max-flash_delay_min))+flash_delay_min;
      maxFlashes = (micros()%max_times)+1;
      //put em in
      f->del = fdel;
      f->maxF = maxFlashes;
      //set other vals
      f->on = false;
      f->timesF = 0;
      f->nextT = 0;
    }
  }
}

void loop(){
  for(i = 0; i<max_sim; ++i){
    flashUpdate(&flashes[i],flashes);
  }
}
