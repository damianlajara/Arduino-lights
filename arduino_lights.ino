// Missing libraries can be downloaded from adafruit
#include <Adafruit_GFX.h> // Core graphics library
#include <SPI.h>
#include <Wire.h> // this is needed even tho we aren’t using it
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define PIN 6
#define NUMPIXELS 60

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Size of the color selection boxes, the paintbrush size and the amount of ROWS that fit on the screen
#define BOXSIZE 40
#define PENRADIUS 3
#define COLUMNS 6 //shades
#define ROWS 7 //colors
int oldcolor, currentcolor;

// Fixed-size offset for every box
#define MULTIPLIER_2 2
#define MULTIPLIER_3 3
#define MULTIPLIER_4 4
#define MULTIPLIER_5 5
#define MULTIPLIER_6 6
#define MULTIPLIER_7 7

#define DELAYVAL 50 // delay for 50ms
/** RED SHADES (Dark-to-light)
* Maroon – 128,0,0 => 0x8000
* Firebrick – 178,34,34 => 0xB104
* Crimon – 220,20,60 => 0xD8A7
* Red – 255,0,0 => 0xF800
* Indian Red – 205,92,92 => 0xCAEB
* light Coral – 240,128,128 => 0xF410
*/
uint16_t shadesOfRed[7] = {
  0x8000,
  0xB104,
  0xD8A7,
  0xF800,
  0xCAEB,
  0xF410
};

/** BLUE SHADES
* Navy – 0,0,128 => 0x0010
* Blue – 0,0,255 => 0x001F
* Deep Sky Blue – 0,191,255 => 0x05FF
* Light Sky Blue – 145,206,250 => 0x967F
* Cyan – 0,255,255 => 0x07FF
* Turquiose – 64,224,208 => 0x471A
*/

uint16_t shadesOfBlue[7] = {
  0x0010,
  0x001F,
  0x05FF,
  0x967F,
  0x07FF,
  0x471A
};

/** GREEN SHADES
* Olive – 128,128,0 => 0x8400
* Yellow Green – 154,205,50 => 0x9E66
* Lawn Green – 124,252,0 => 0x7FE0
* Green – 0,128,0 => 0x0400
* Lime Green – 50,205,50 => 0x3666
* Lime – 0,255,0 => 0x07E0
*/

uint16_t shadesOfGreen[7] = {
  0x8400,
  0x9E66,
  0x7FE0,
  0x0400,
  0x3666,
  0x07E0
};

/** ORANGE AND YELLOW SHADES
* Orange Red – 250,80,0 => 0xFA80
* Dark Orange – 255,140,0 => 0xFC60
* Orange – 255,165,0 => 0xFD20
* Golden Rod – 218,165,32 => 0xDD24
* Gold – 255,215,0 => 0xFEA0
* Yellow – 255,255,0 => 0xFFE0
*/

uint16_t shadesOfOrangeAndYellow[7] = {
  0xFA80,
  0xFC60,
  0xFD20,
  0xDD24,
  0xFEA0,
  0xFFE0
};

/** PURPLE AND PINK SHADES
* Blue Violet – 138,43,226 => 0x895C
* Purple – 128,0,128 => 0x8010
* Dark Magenta – 139,0,139 => 0x8811
* Medium Orchid – 186,85,211 => 0xBABA
* Deep Pink – 255,20,147 => 0xF8B2
* Magenta – 255,0,255 => 0xF81F
*/

uint16_t shadesOfPurpleAndPink[7] = {
  0x895C,
  0x8010,
  0x8811,
  0xBABA,
  0xF8B2,
  0xF81F
};

/** BROWN SHADES
* Saddle Brown – 139,69,19 => 0x8A22
* Sienna – 160,82,45 => 0xA285
* Chocolate – 210,105,30 => 0xD343
* Sandy Brown – 244,164,96 => 0xF52C
* Tan – 210,180,140 => 0xD5B1
* Wheat – 245,222,179 => 0xF6F6
*/

uint16_t shadesOfBrown[7] = {
  0x8A22,
  0xA285,
  0xD343,
  0xF52C,
  0xD5B1,
  0xF6F6
};

/** BLACK AND WHITE SHADES
* Black – 0,0,0 => 0x0000
* Dim Grey – 96,96,96 => 0x630C
* Gray – 128,128,128 => 0x8410
* Silver – 192,192,192 => 0xC618
* Light Gray – 211,211,211 => 0xD69A
* White – 255,255,255 => 0xFFFF
*/

uint16_t shadesOfBlackAndWhite[7] = {
  0x0000,
  0x630C,
  0x8410,
  0xC618,
  0xD69A,
  0xFFFF
};

uint16_t allShades[COLUMNS][ROWS] = {
// // Red
// { 0x8000, 0xB104, 0xD8A7, 0xF800, 0xCAEB, 0xF410 },
// // Green
// { 0x8400, 0x9E66, 0x7FE0, 0x0400, 0x3666, 0x07E0 },
// // Blue
// { 0x0010, 0x001F, 0x05FF, 0x967F, 0x07FF, 0x471A },
// // Orange and Yellow
// { 0xFA80, 0xFC60, 0xFD20, 0xFD20, 0xFEA0, 0xFFE0 },
// // Purple and pink
// { 0x895C, 0x8010, 0x8811, 0xBABA, 0xF8B2, 0xF81F },
// // Brown
// { 0x8A22, 0xA285, 0xD343, 0xF52C, 0xD5B1, 0xF6F6 },
// // Black and White
// { 0x0000, 0x630C, 0x8410, 0xC618, 0xD69A, 0xFFFF }

  //Red Green Blue Orange Purple Brown Black
  {0x8000, 0x8400, 0x0010, 0xFA80, 0x895C , 0x8A22 , 0x0000},
  {0xB104, 0x9E66, 0x001F, 0xFC60, 0x8010 , 0xA285 , 0x630C},
  {0xD8A7, 0x7FE0, 0x05FF, 0xFD20, 0x8811 , 0xD343 , 0x8410},
  {0xF800, 0x0400, 0x967F, 0xFD20, 0xBABA , 0xF52C , 0xC618},
  {0xCAEB, 0x3666, 0x07FF, 0xFEA0, 0xF8B2 , 0xD5B1 , 0xD69A},
  {0xF410, 0x07E0, 0x471A, 0xFFE0, 0xF81F , 0xF6F6 , 0xFFFF}
};

//Declare the pixel object, so we can change the color later on in the code
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//5312 – ext

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  
  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) { //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, c); //turn every third pixel on
      }
      pixels.show();
      
      delay(wait);
      
      for (int i=0; i < pixels.numPixels(); i=i+3) {
      pixels.setPixelColor(i+q, 0); //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) { // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, Wheel( (i+j) % 255)); //turn every third pixel on
      }
      pixels.show();
      
      delay(wait);
      
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0); //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r – g – b – back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 – WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 – WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 – WheelPos * 3);
  } else {
    WheelPos -= 170;
    return pixels.Color(WheelPos * 3, 255 – WheelPos * 3, 0);
  }
}

//Maps the 5:6:4 RGB HEX values to it’s distinct r, g, and b values and updates the color of the neopixels
void showPixelsWithColor(uint16_t color) {
  int r,g,b;
  switch (color) {
  // Shades of Red
  case 0x8000:
    r = 128; g = 0; b = 0;
    break;
  case 0xB104:
    r = 178; g = 34; b = 34;
    break;
  case 0xD8A7:
    r = 220; g = 20; b = 60;
    break;
  case 0xF800:
    r = 255; g = 0; b = 0;
    break;
  case 0xCAEB:
    r = 205; g = 92; b = 92;
    break;
  case 0xF410:
    r = 240; g = 128; b = 128;
    break;
  
  // Shades of Blue
  case 0x0010:
    r = 0; g = 0; b = 128;
    break;
  case 0x001F:
    r = 0; g = 0; b = 255;
    break;
  case 0x05FF:
    r = 0; g = 191; b = 255;
    break;
  case 0x967F:
    r = 145; g = 206; b = 250;
    break;
  case 0x07FF:
    r = 0; g = 255; b = 255;
    break;
  case 0x471A:
    r = 64; g = 224; b = 208;
    break;
  case 0x067A:
    r = 0; g = 206; b = 209;
    break;
  
  // Shades Of Green
  case 0x8400:
    r = 128; g = 128; b = 0;
    break;
  case 0x9E66:
    r = 154; g = 205; b = 50;
    break;
  case 0x7FE0:
    r = 124; g = 252; b = 0;
    break;
  case 0x0400:
    r = 0; g = 128; b = 0;
    break;
  case 0x3666:
    r = 50; g = 205; b = 50;
    break;
  case 0x07E0:
    r = 0; g = 255; b = 0;
    break;
  case 0x9FD3:
    r = 152; g = 251; b = 152;
    break;
  
  // Shades of Orange and Yellow
  case 0xFA80:
    r = 250; g = 80; b = 0;
    break;
  case 0xFC60:
    r = 255; g = 140; b = 0;
    break;
  case 0xFD20:
    r = 255; g = 165; b = 0;
    break;
  case 0xDD24:
    r = 218; g = 165; b = 32;
    break;
  case 0xFEA0:
    r = 255; g = 215; b = 0;
    break;
  case 0xFFE0:
    r = 255; g = 255; b = 0;
    break;
  case 0xF731:
    r = 240; g = 230; b = 140;
    break;
  
  // Shades of Purple and Pink
  case 0x895C:
    r = 138; g = 43; b = 226;
    break;
  case 0x8010:
    r = 128; g = 0; b = 128;
    break;
  case 0x8811:
    r = 139; g = 0; b = 139;
    break;
  case 0xBABA:
    r = 186; g = 85; b = 211;
    break;
  case 0xF8B2:
    r = 255; g = 20; b = 147;
    break;
  case 0xF81F:
    r = 255; g = 0; b = 255;
    break;
  case 0xFB56:
    r = 255; g = 105; b = 180;
    break;
  
  // Shades of Brown
  case 0x8A22:
    r = 139; g = 69; b = 19;
    break;
  case 0xA285:
    r = 160; g = 82; b = 45;
    break;
  case 0xD343:
    r = 210; g = 105; b = 30;
    break;
  case 0xF52C:
    r = 244; g = 164; b = 96;
    break;
  case 0xD5B1:
    r = 210; g = 180; b = 140;
    break;
  case 0xF6F6:
    r = 245; g = 222; b = 179;
    break;
  case 0xFF38:
    r = 255; g = 228; b = 196;
    break;
  
  // Shades of Black and White
  case 0x0000:
    r = 0; g = 0; b = 0;
    break;
  case 0x630C:
    r = 96; g = 96; b = 96;
    break;
  case 0x8410:
    r = 128; g = 128; b = 128;
    break;
  case 0xC618:
    r = 192; g = 192; b = 192;
    break;
  case 0xD69A:
    r = 211; g = 211; b = 211;
    break;
  case 0xDEFB:
    r = 220; g = 220; b = 220;
    break;
  case 0xFFFF:
    r = 255; g = 255; b = 255;
    break;
  
  default:
    Serial.print(“Error in showPixelsWithColor() switch statement\nColor is: “);
    Serial.print(color);
    break;
  }
  
  // Display the color of the neopixel stick
  for(int i=0;i<NUMPIXELS;i++) {
    pixels.setPixelColor(i, pixels.Color(r,g,b));
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
  Serial.print(“Changed neopixel color to: “);
  Serial.print(color);
  Serial.print(“\nR, G, B: “);
  Serial.print(r);
  Serial.print(“, “);
  Serial.print(b);
  Serial.print(“, “);
  Serial.print(g);
  Serial.print(“\n”);
}

void turnPixelsOff() {
  //Set the neo pixels to off to begin with!
  for(int i=0;i<NUMPIXELS;i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
  }
  void displayLogo() {
    tft.setCursor(65, 100);
    tft.setTextColor(ILI9341_WHITE); tft.setTextSize(5);
    tft.println(“Novo”);
    tft.setCursor(50, 150);
    tft.setTextColor(ILI9341_WHITE); tft.setTextSize(5);
    tft.println(“Aduro”);
  }
  
  void createBoxes(int delayval = DELAYVAL) {
  //First Row
  for(int i = 0; i < ROWS; i++) {
    tft.fillRect(BOXSIZE*i, 0, BOXSIZE, BOXSIZE, shadesOfRed[i]);//xywh
    delay(delayval);
  }
  
  //Second Row
  for(int i = 0; i < ROWS; i++) {
    tft.fillRect(BOXSIZE*i, BOXSIZE, BOXSIZE, BOXSIZE, shadesOfGreen[i]);//xywh
    delay(delayval);
  }
  
  //Third Row
  for(int i = 0; i < ROWS; i++) {
    tft.fillRect(BOXSIZE*i, BOXSIZE*MULTIPLIER_2, BOXSIZE, BOXSIZE, shadesOfBlue[i]);//xywh
    delay(delayval);
  }
  
  //Fourth Row
  for(int i = 0; i < ROWS; i++) {
    tft.fillRect(BOXSIZE*i, BOXSIZE*MULTIPLIER_3, BOXSIZE, BOXSIZE, shadesOfOrangeAndYellow[i]);//xywh
    delay(delayval);
  }
  
  //Fifth Row
  for(int i = 0; i < ROWS; i++) {
    tft.fillRect(BOXSIZE*i, BOXSIZE*MULTIPLIER_4, BOXSIZE, BOXSIZE, shadesOfPurpleAndPink[i]);//xywh
    delay(delayval);
  }
  
  //Sixth Row
  for(int i = 0; i < ROWS; i++) {
    tft.fillRect(BOXSIZE*i, BOXSIZE*MULTIPLIER_5, BOXSIZE, BOXSIZE, shadesOfBrown[i]);//xywh
    delay(delayval);
  }
  
  //Seventh Row
  for(int i = 0; i < ROWS; i++) {
    tft.fillRect(BOXSIZE*i, BOXSIZE*MULTIPLIER_6, BOXSIZE, BOXSIZE, shadesOfBlackAndWhite[i]);//xywh
    delay(delayval);
  }
  
  //Eighth Row (Special Effects)
  for(int i = 0; i < ROWS; i++) {
    tft.drawRect(BOXSIZE*i, BOXSIZE*MULTIPLIER_7, BOXSIZE, BOXSIZE, allShades[0][i]);//xywh
    tft.setCursor(BOXSIZE*i+18, BOXSIZE*MULTIPLIER_7+14);
    tft.setTextColor(ILI9341_WHITE); tft.setTextSize(2);
    tft.println(i+1);
    delay(delayval);
  }
}

void setup(void) {
  
  Serial.begin(9600);
  Serial.println(F(“Touch Paint!”));
  
  pixels.begin(); // This initializes the NeoPixel library.
  tft.begin();
  
  if (!ts.begin()) {
    Serial.println(“Couldn’t start touchscreen controller”);
    while (1);
  }
  Serial.println(“Touchscreen started”);
  
  tft.fillScreen(ILI9341_BLACK);
  //displayLogo();
  //delay(1000);
  createBoxes(0);
  turnPixelsOff();
  
  //showPixelsWithColor(shadesOfBlackAndWhite[5]);

}

void loop() {
  
  // See if there’s any touch data for us
  if (ts.bufferEmpty()) {
    //displayLogo();
    return;
  }
  
  // // You can also wait for a touch
  // if (! ts.touched()) {
  // return;
  // }
  
  Serial.print(“touchscreen touched!”);
  
  // Retrieve a point
  TS_Point p = ts.getPoint();
  
  Serial.print(“X = “); Serial.print(p.x);
  Serial.print(“\tY = “); Serial.print(p.y);
  Serial.print(“\tPressure = “); Serial.println(p.z);
  
  // Scale from ~0->4000 to tft.width using the calibration #’s
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  
  //tft.fillCircle(p.x, p.y, 10, ILI9341_RED);//p.x, p.y, PENRADIUS, currentcolor
  
  Serial.print(“(“); Serial.print(p.x);
  Serial.print(“, “); Serial.print(p.y);
  Serial.println(“)”);
  
  pixels.setBrightness(100);
  
  if (p.y < BOXSIZE) {
    if (p.x < BOXSIZE) {
      showPixelsWithColor(shadesOfRed[0]);
    } else if (p.x < BOXSIZE*2) {
      showPixelsWithColor(shadesOfRed[1]);
    } else if (p.x < BOXSIZE*3) {
      showPixelsWithColor(shadesOfRed[2]);
    } else if (p.x < BOXSIZE*4) {
      showPixelsWithColor(shadesOfRed[3]);
    } else if (p.x < BOXSIZE*5) {
      showPixelsWithColor(shadesOfRed[4]);
    } else if (p.x < BOXSIZE*6) {
      showPixelsWithColor(shadesOfRed[5]);
    }
  } else if (p.y < BOXSIZE*2) {
    if (p.x < BOXSIZE) {
      showPixelsWithColor(shadesOfGreen[0]);
    } else if (p.x < BOXSIZE*2) {
      showPixelsWithColor(shadesOfGreen[1]);
    } else if (p.x < BOXSIZE*3) {
      showPixelsWithColor(shadesOfGreen[2]);
    } else if (p.x < BOXSIZE*4) {
      showPixelsWithColor(shadesOfGreen[3]);
    } else if (p.x < BOXSIZE*5) {
      showPixelsWithColor(shadesOfGreen[4]);
    } else if (p.x < BOXSIZE*6) {
      showPixelsWithColor(shadesOfGreen[5]);
    }
  } else if (p.y < BOXSIZE*3) {
    if (p.x < BOXSIZE) {
      showPixelsWithColor(shadesOfBlue[0]);
    } else if (p.x < BOXSIZE*2) {
      showPixelsWithColor(shadesOfBlue[1]);
    } else if (p.x < BOXSIZE*3) {
      showPixelsWithColor(shadesOfBlue[2]);
    } else if (p.x < BOXSIZE*4) {
      showPixelsWithColor(shadesOfBlue[3]);
    } else if (p.x < BOXSIZE*5) {
      showPixelsWithColor(shadesOfBlue[4]);
    } else if (p.x < BOXSIZE*6) {
      showPixelsWithColor(shadesOfBlue[5]);
    }
  } else if (p.y < BOXSIZE*4) {
    if (p.x < BOXSIZE) {
      showPixelsWithColor(shadesOfOrangeAndYellow[0]);
    } else if (p.x < BOXSIZE*2) {
      showPixelsWithColor(shadesOfOrangeAndYellow[1]);
    } else if (p.x < BOXSIZE*3) {
      showPixelsWithColor(shadesOfOrangeAndYellow[2]);
    } else if (p.x < BOXSIZE*4) {
      showPixelsWithColor(shadesOfOrangeAndYellow[3]);
    } else if (p.x < BOXSIZE*5) {
      showPixelsWithColor(shadesOfOrangeAndYellow[4]);
    } else if (p.x < BOXSIZE*6) {
      showPixelsWithColor(shadesOfOrangeAndYellow[5]);
    }
  } else if (p.y < BOXSIZE*5) {
    if (p.x < BOXSIZE) {
      showPixelsWithColor(shadesOfPurpleAndPink[0]);
    } else if (p.x < BOXSIZE*2) {
      showPixelsWithColor(shadesOfPurpleAndPink[1]);
    } else if (p.x < BOXSIZE*3) {
      showPixelsWithColor(shadesOfPurpleAndPink[2]);
    } else if (p.x < BOXSIZE*4) {
      showPixelsWithColor(shadesOfPurpleAndPink[3]);
    } else if (p.x < BOXSIZE*5) {
      showPixelsWithColor(shadesOfPurpleAndPink[4]);
    } else if (p.x < BOXSIZE*6) {
      showPixelsWithColor(shadesOfPurpleAndPink[5]);
    }
  } else if (p.y < BOXSIZE*6) {
    if (p.x < BOXSIZE) {
      showPixelsWithColor(shadesOfBrown[0]);
    } else if (p.x < BOXSIZE*2) {
      showPixelsWithColor(shadesOfBrown[1]);
    } else if (p.x < BOXSIZE*3) {
      showPixelsWithColor(shadesOfBrown[2]);
    } else if (p.x < BOXSIZE*4) {
      showPixelsWithColor(shadesOfBrown[3]);
    } else if (p.x < BOXSIZE*5) {
      showPixelsWithColor(shadesOfBrown[4]);
    } else if (p.x < BOXSIZE*6) {
      showPixelsWithColor(shadesOfBrown[5]);
    }
  } else if (p.y < BOXSIZE*7) {
    if (p.x < BOXSIZE) {
      showPixelsWithColor(shadesOfBlackAndWhite[0]);
    } else if (p.x < BOXSIZE*2) {
      showPixelsWithColor(shadesOfBlackAndWhite[1]);
    } else if (p.x < BOXSIZE*3) {
      showPixelsWithColor(shadesOfBlackAndWhite[2]);
    } else if (p.x < BOXSIZE*4) {
      showPixelsWithColor(shadesOfBlackAndWhite[3]);
    } else if (p.x < BOXSIZE*5) {
      showPixelsWithColor(shadesOfBlackAndWhite[4]);
    } else if (p.x < BOXSIZE*6) {
      showPixelsWithColor(shadesOfBlackAndWhite[5]);
    }
  } else if (p.y < BOXSIZE*8) { //SPECIAL EFFECTS
    if (p.x < BOXSIZE) {
      colorWipe(pixels.Color(255, 0, 0), 2);//red
      colorWipe(pixels.Color(0, 255, 0), 2);//blue
      colorWipe(pixels.Color(0, 0, 255), 2);//green
      turnPixelsOff(); //TURN OFF!
    } else if (p.x < BOXSIZE*2) {
      theaterChase(pixels.Color( 0, 0, 127), 2); // Blue
      theaterChase(pixels.Color( 0, 127, 0), 2); // green
      theaterChase(pixels.Color( 127, 0, 0), 2); // red
      turnPixelsOff();
    } else if (p.x < BOXSIZE*3) {
      rainbow(2);
      turnPixelsOff();
    } else if (p.x < BOXSIZE*4) {
      rainbowCycle(2);
      turnPixelsOff();
    } else if (p.x < BOXSIZE*5) {
      theaterChaseRainbow(2);
      turnPixelsOff();
    } else if (p.x < BOXSIZE*6) {
      colorWipe(pixels.Color(139, 0, 139), 2);
      colorWipe(pixels.Color(255, 20, 147), 2);
      turnPixelsOff();
    }
  }
}//end loop

