/*
 * drawing a flower with 4 petals
 * 
 * 2 pressure sensors (force-sensitive resistor): one for adding or rotating, one for deleting or reseting
 * 1 NeoPixel 8x8 Matrix RGB
 * 
 * @AUTHOR:Xia Zhou
 * @DATE:Jan 2017
 */
#include <Adafruit_NeoPixel.h>

const int neo_pin = 6;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(64, neo_pin);

int petal_num = 4;
int petal_pixels = 9;

// color for each petal: red,yellow,green,blue
uint32_t petal_colors[] = {
  pixels.Color(255, 0, 255),
  pixels.Color(250, 250, 74),
  pixels.Color(107, 235, 0),
  pixels.Color(85, 125, 225)
};
    
//pixels for the 4 petals
int petals[]= {
  28,20,12,5,6,15,23,30,29,
  36,37,38,47,55,62,61,52,44,
  35,43,51,58,57,48,40,33,34,
  27,26,25,16,8,1,2,11,19
};

// pixels for petal eyes 
int petals_inner[] = {
  21,14,45,54,42,49,18,9
};

//pressure sensor
const int fsrAnalogPin_1 = 0; //pressure sensor 1 : draw a petal OR rotate the flower
const int fsrAnalogPin_2 = 3; //pressure sensor 2 : erase a petal OR reset the flower
int fsrReading1;
int fsrReading2;

void setup() {
  Serial.begin(9600);
  pixels.setBrightness(20); //reduce the brightness (0~255)
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'
  Serial.println("Ready to start");

}

int petal_completed = 0; // how many petals have been lit on the shield
bool blink_eye = true; // true: flower is incomplete; false: flower is complete

// simulate reset
void(*resetFunc) (void) = 0;

void loop() {
  fsrReading1 = analogRead(fsrAnalogPin_1);
  fsrReading2 = analogRead(fsrAnalogPin_2);

  if(fsrReading1<300 && fsrReading2<300 ){
    return;
  }

  if(blink_eye){  // if flower is in complete, then either to draw a petal or erase a petal
    //to draw a petal
    if(fsrReading1>300){  
      Serial.println(petal_completed);
      Serial.println(blink_eye);
      Serial.print("Analog reading 1 = ");
      Serial.println(fsrReading1);
      Serial.print("Analog reading 2 = ");
      Serial.println(fsrReading2);
     
      if(petal_completed < petal_num){  // draw pixels one by one to form a trace
          Serial.println("Drawing a petal");
          drawPetals(petal_completed,100);  
          petal_completed++;
          delay(500);
      }

      // if flower is complete: to blink petal eyes
      if(petal_completed == petal_num){
        Serial.println("Finish the flower");
          if(blink_eye){
            for (int m=0; m<5; m++){
              drawEye(pixels.Color(0,0,0),50);
              drawEye(pixels.Color(112,128,105),50);
              blink_eye = false;
            }
          }
      }
    }

   //to erase a petal
    if(fsrReading2 > 300){
      Serial.print("Analog reading 1 = ");
      Serial.println(fsrReading1);
      Serial.print("Analog reading 2 = ");
      Serial.println(fsrReading2);
      
      if(petal_completed > 0){
        Serial.println("Erasing a petal");
        erasePetals(petal_completed-1,petal_pixels); //erase a previous petal
        petal_completed--;
      }
    }
  }else if(fsrReading1>300){ // if flower is complete and sensor 1 is pressed: to rotate
    Serial.println("Start rotating");
          //color shift right once a time
          uint32_t temp;
          for (int n=0; n<3; n++){
            temp = petal_colors[n+1];
            petal_colors[n+1]=petal_colors[0];
            petal_colors[0] = temp;
          }
          for(int i=0;i<4;i++){
            drawPetals(i,0); 
            drawEye(pixels.Color(0,0,0),0);
            delay(20);
          }
          delay(50);
          drawEye(pixels.Color(112,128,105),0); 
  }else if(fsrReading2>300){ // if flower is complete and sensor 2 is pressed: to reset
    Serial.println("Reset . . .");
    Serial.println(petal_completed);
    Serial.println(blink_eye);
    delay(100);
    resetFunc();
  }

}


// light the pixels one by one of a petal
void drawPetals(int i, int wait){
  Serial.println(i);
      for(int j=i*petal_pixels; j< (i+1)*petal_pixels; j++){
        pixels.setPixelColor(petals[j],petal_colors[i]);
        pixels.show();
        delay(wait);
      }
}

// light all the petal eyes 
void drawEye(uint32_t color, int wait){
  for (int i=0; i<8; i++){
      pixels.setPixelColor(petals_inner[i],color);
      pixels.show();
  }
  delay(wait);
}

//dim petals/eyes gradually to zero of a petal
void erasePetals(int i, int number){
  Serial.println(i);
  for(int dim=1; dim<7; dim++){
      for(int j=i*number; j< (i+1)*number; j++){
           dimPetals(j,90-15*dim,number);
      }
          delay(100);
  }
  delay(500);
}

//change the color to mimich dimming effect
void dimPetals(int j, int color_value, int number){
  if(number==2){// dim eye
    pixels.setPixelColor(petals_inner[j],color_value,color_value,color_value);
  }else{ // dim petal
    pixels.setPixelColor(petals[j],color_value,color_value,color_value);
  }
  pixels.show();
}


