#include <Arduino.h>


#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

//#define SerialBT Serial

#define clockPin1 12 //Arduino pin connected to Clock Pin 11 of 74HC595
#define latchPin1 13 //Arduino pin connected to Latch Pin 12 of 74HC595
#define dataPin1 14  //Arduino pin connected to Data Pin 14 of 74HC595




String message = "";
String message2 = "";
unsigned int msglen;
char incomingChar;

bool enter = 0;
bool txt = 0;
bool nt = 0;

byte p = 0;
void Store();
int pic[] = {0, 36, 36, 0, 0, 66, 60, 0};                   //{0,36,36,0,0,66,60,0};


unsigned long previousMillis = 0;        // will store last time LED was updated


unsigned long interval= 300;



char msg[150];  //Change the text here.
int scrollspeed = 400; //Set the scroll speed ( lower=faster)
#define charsize 5

int x;
int y;

//Columns

//BITMAP
//Bits in this array represents one LED of the matrix
// 8 is # of rows, 6 is # of LED matrices
byte bitmap[8][3];

int numZones = sizeof(bitmap) / 8; // One Zone refers to one 8 x 8 Matrix ( Group of 8 columns)
int maxZoneIndex = numZones - 1;
int numCols = numZones * 8;

//FONT DEFENITION
byte alphabets[][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0}, //@ as SPACE
  //{8,28,54,99,65},//<<
  {31, 36, 68, 36, 31},//A
  {127, 73, 73, 73, 54},//B
  {62, 65, 65, 65, 34},//C
  {127, 65, 65, 34, 28},//D
  {127, 73, 73, 65, 65},//E
  {127, 72, 72, 72, 64},//F
  {62, 65, 65, 69, 38},//G
  {127, 8, 8, 8, 127},//H
  {0, 65, 127, 65, 0},//I
  {2, 1, 1, 1, 126},//J
  {127, 8, 20, 34, 65},//K
  {127, 1, 1, 1, 1},//L
  {127, 32, 16, 32, 127},//M
  {127, 32, 16, 8, 127},//N
  {62, 65, 65, 65, 62},//O
  {127, 72, 72, 72, 48},//P
  {62, 65, 69, 66, 61},//Q
  {127, 72, 76, 74, 49},//R
  {50, 73, 73, 73, 38},//S
  {64, 64, 127, 64, 64},//T
  {126, 1, 1, 1, 126},//U
  {124, 2, 1, 2, 124},//V
  {126, 1, 6, 1, 126},//W
  {99, 20, 8, 20, 99},//X
  {96, 16, 15, 16, 96},//Y
  {67, 69, 73, 81, 97},//Z
};



//Clear bitmap

int X1[25][8];


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(latchPin1, OUTPUT);
  pinMode(clockPin1, OUTPUT);
  pinMode(dataPin1, OUTPUT);

  //SerialBT.begin(115200);


  shiftOut(dataPin1, clockPin1, MSBFIRST, 0B00000000);
  shiftOut(dataPin1, clockPin1, MSBFIRST, 0B00000000);
  shiftOut(dataPin1, clockPin1, MSBFIRST, 0B11111111);
  Store();
  delay(2000);

  //Clear bitmap
  for (int row = 0; row < 8; row++) {
    for (int zone = 0; zone <= maxZoneIndex; zone++) {
      bitmap[row][zone] = 0;
    }
  }
}

byte l; // Size of common array
void loop() {

  const int P1[][8] = {{0x00, 0x66, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00},
    {0xC3, 0xE7, 0x7E, 0x3C, 0x3C, 0x7E, 0xE7, 0xC3},
    {0x00, 0x66, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00},
    {0xC3, 0xE7, 0x7E, 0x3C, 0x3C, 0x7E, 0xE7, 0xC3}
  };


  const int P2[][8] = {{0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x24, 0x18, 0x18, 0x24, 0x00, 0x00},
    {0x00, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x00},
    {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81},
    {0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, 0x00},
    {0x24, 0x18, 0x18, 0x24, 0x46, 0x81, 0x00, 0x00},
    {0x18, 0x3C, 0x42, 0x81, 0x00, 0x00, 0x00, 0x00},
    {0x18, 0x3C, 0x5A, 0x81, 0x00, 0x00, 0x00, 0x00},
    {0x18, 0x3C, 0x5A, 0x99, 0x00, 0x00, 0x00, 0x00},
    {0x18, 0x3C, 0x5A, 0x99, 0x18, 0x00, 0x00, 0x00},
    {0x18, 0x3C, 0x5A, 0x99, 0x18, 0x18, 0x00, 0x00},
    {0x18, 0x3C, 0x5A, 0x99, 0x18, 0x18, 0x18, 0x00},
    {0x18, 0x3C, 0x5A, 0x99, 0x18, 0x18, 0x18, 0x18},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
  };

const int P3[][8] = {{0x3C,0x42,0x81,0x91,0xA9,0x91,0x42,0x3C},
{0x3C,0x42,0x81,0x89,0x95,0x89,0x42,0x3C},
{0x3C,0x42,0x85,0x8B,0x85,0x81,0x42,0x3C},
{0x3C,0x42,0x81,0x89,0x95,0x89,0x42,0x3C},
{0x3C,0x42,0x81,0x91,0xA9,0x91,0x42,0x3C},
{0x3C,0x42,0xA1,0xD1,0xA1,0x81,0x42,0x3C}
};

const int P4[][8] = {{0x3C,0x42,0xA5,0x81,0xA5,0x99,0x42,0x3C},
{0x3C,0x42,0xA5,0x81,0xBD,0x81,0x42,0x3C},
{0x3C,0x42,0xA5,0x81,0x99,0xA5,0x42,0x3C},
{0x3C,0x42,0xA5,0x81,0xBD,0x81,0x42,0x3C},

};

const int P5[][8] = {{0x24,0x24,0xFF,0x24,0x24,0xFF,0x24,0x24}};


const int P6[][8] = {{0x00,0x00,0x08,0x14,0x22,0x41,0x00,0x00}};

const int U[][8]= {{0x00,0x42,0x42,0x42,0x42,0x42,0x3C,0x00}};
const int B[][8]= {{0x3C,0x22,0x22,0x3C,0x22,0x22,0x3C,0x00}};
const int H1[][8]= {{0x00,0x66,0x99,0x81,0x42,0x24,0x18,0x00},
{0x00,0x66,0x99,0x81,0x42,0x24,0x18,0x01},
{0x00,0x66,0x99,0x81,0x42,0x24,0x1A,0x01},
{0x00,0x66,0x99,0x81,0x42,0x24,0x1A,0x01},
{0x00,0x66,0x99,0x81,0x4A,0x24,0x1A,0x01},
{0x00,0x66,0x99,0x91,0x4A,0x24,0x1A,0x01},
{0x00,0x66,0xB9,0x91,0x4A,0x24,0x1A,0x01},
{0x00,0x66,0xB9,0x91,0x4A,0x24,0x1A,0x01},
{0x80,0x66,0xB9,0x91,0x4A,0x24,0x1B,0x02}

};

  while (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    if (  incomingChar != '\n') {
      message += String(incomingChar);
      /****/      Serial.print("Incoming message is.....................................................="); Serial.println(String(message));
    }
    if (txt == 1 && incomingChar != '\n')
    { nt = 1;
      message2 += String(incomingChar);
      /****/      Serial.print("Incoming message2 is.....................................................="); Serial.println(String(message2));
    }

    //Serial.write(incomingChar);
  }

  if (message == "txt" || message == "txt\n")
  { txt = 1; nt = 0;  message2 = "";
    /****/   Serial.println("you are in txt");
  }
  else if (message == "p1" || message == "p1\n")
  { txt = 0;
    enter = 0;


    byte l1 = sizeof ( P1 ) / sizeof ( P1[8] );
    l = l1;
    for (int x1 = 0 ; x1 < l; x1++)
    {
      for (int y1 = 0 ; y1 < 8; y1++)
      {
        X1[x1][y1] = P1[x1][y1];
      }

    }  /****/ Serial.print("Incoming lenght of common arrayy is=  p1  ");  Serial.println(l);
  }
  else if (message == "p2" || message == "p2\n")
  { txt = 0;
    enter = 0;
    byte l1 = sizeof ( P2 ) / sizeof ( P2[8] );
    l = l1;
    for (int x1 = 0 ; x1 < l; x1++)
    {
      for (int y1 = 0 ; y1 < 8; y1++)
      {
        X1[x1][y1] = P2[x1][y1];
      }
    }  /****/ Serial.print("Incoming lenght of common arrayy is=  p2  "); Serial.println(l);
  }
  else if (message == "p3" || message == "p3\n")
  {txt = 0;
    enter = 0;
    byte l1 = sizeof ( P3 ) / sizeof ( P3[8] );
    l = l1;
    for (int x1 = 0 ; x1 < l; x1++)
    {
      for (int y1 = 0 ; y1 < 8; y1++)
      {
        X1[x1][y1] = P3[x1][y1];
      }
    } 
  }

  else if (message == "p4" || message == "p4\n")
  { txt = 0;
    enter = 0;
    byte l1 = sizeof ( P4 ) / sizeof ( P4[8] );
    l = l1;
    for (int x1 = 0 ; x1 < l; x1++)
    {
      for (int y1 = 0 ; y1 < 8; y1++)
      {
        X1[x1][y1] = P4[x1][y1];
      }
    } 
  }

    else if (message == "p5" || message == "p5\n")
  { txt = 0;
    enter = 0;
    byte l1 = sizeof ( P5 ) / sizeof ( P5[8] );
    l = l1;
    for (int x1 = 0 ; x1 < l; x1++)
    {
      for (int y1 = 0 ; y1 < 8; y1++)
      {
        X1[x1][y1] = P5[x1][y1];
      }
    } 
  }

    else if (message == "p6" || message == "p6\n")
  { txt = 0;
    enter = 0;
    byte l1 = sizeof ( P6 ) / sizeof ( P6[8] );
    l = l1;
    for (int x1 = 0 ; x1 < l; x1++)
    {
      for (int y1 = 0 ; y1 < 8; y1++)
      {
        X1[x1][y1] = P6[x1][y1];
      }
    } 
  }

    else if (message == "b" || message == "b\n")
  { txt = 0;
    enter = 0;
    byte l1 = sizeof ( B ) / sizeof ( B[8] );
    l = l1;
    for (int x1 = 0 ; x1 < l; x1++)
    {
      for (int y1 = 0 ; y1 < 8; y1++)
      {
        X1[x1][y1] = B[x1][y1];
      }
    } }
   else if (message == "u" || message == "u\n")
  { txt = 0;
    enter = 0;
    byte l1 = sizeof ( U ) / sizeof ( U[8] );
    l = l1;
    for (int x1 = 0 ; x1 < l; x1++)
    {
      for (int y1 = 0 ; y1 < 8; y1++)
      {
        X1[x1][y1] = U[x1][y1];
      }
    } 
  }
    else if (message == "h1" || message == "h1\n")
  { txt = 0;
    enter = 0;
    byte l1 = sizeof ( H1 ) / sizeof ( H1[8] );
    l = l1;
    for (int x1 = 0 ; x1 < l; x1++)
    {
      for (int y1 = 0 ; y1 < 8; y1++)
      {
        X1[x1][y1] = H1[x1][y1];
      }
    }  } 
  
  else if (message == "s++" || message == "s++\n")
    {scrollspeed=scrollspeed+10;txt = 0;message2="";
    /****/ SerialBT.println("Speed INcrease");
    }
    else if (message == "s--" || message == "s--\n")
    {scrollspeed=scrollspeed-10;txt = 0;message2="";
    /****/ SerialBT.println("Speed INcrease");
    }

 else if (message == "e++" || message == "e++\n")
    {interval=interval+10;txt = 0;message2="";
    /****/ SerialBT.println("Speed INcrease");
    }
    else if (message == "e--" || message == "e--\n")
    {interval=interval-10;txt = 0;message2="";
    /****/ SerialBT.println("Speed INcrease");
    }
    
  else if (txt == 1  && nt == 1)
  { msglen = message2.length();
    /****/  Serial.print("msglen is"); Serial.println(msglen);
    // Copy character by character into array
    for (int i = 0; i < msglen; i++) {
      msg[i] = message2.charAt(i);
      /****/     Serial.print(msg[i]);
    }

      message2 = "";
    nt = 0;
    enter = 1;
  
  } 

  message = "";
if(scrollspeed<20)
{scrollspeed=20;}
if(scrollspeed>1000)
{scrollspeed=1000;}

if(interval<20)
{interval=20;}
if(interval>1000)
{interval=1000;}


  if (txt == 0)
  {





    //byte l =sizeof ( X1 ) / sizeof ( X1[8] );
    //Serial.println(l);
    for (int i = 0; i < l;)
    {
      view(X1[i]);


      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        i++;
      }


    }
  }

  if (enter == 1)
  { XProcess();
  }









}

void view(int X[8])
{ for (int i = 0; i < 8; i++)
  { shiftOut(dataPin1, clockPin1, MSBFIRST, ~X[i]);
    shiftOut(dataPin1, clockPin1, MSBFIRST, ~X[i]);
    shiftOut(dataPin1, clockPin1, LSBFIRST, (128 >> i));
    Store();
  }
}


void XProcess()
{
  for (int charIndex = 0; charIndex < (msglen - 1); charIndex++) /////////////(sizeof(msg)-1); charIndex++)
  {
    int alphabetIndex = msg[charIndex] - '@';
    if (alphabetIndex < 0) alphabetIndex = 0;

    //Draw one character of the message
    // Each character is 5 columns wide, loop two more times to create 2 pixel space betwen characters
    for (int col = 0; col < charsize + 1  ; col++)
    {
      for (int row = 0; row < 8; row++)
      {
        // Set the pixel to what the alphabet say for columns 0 thru 4, but always leave columns 5 and 6 blank.
        bool isOn = 0;
        if (col < 5) isOn = bitRead( alphabets[alphabetIndex][col], 7 - row ) == 1;
        Plot( numCols - 1, row, isOn); //Draw on the rightmost column, the shift loop below will scroll it leftward.
      }
      for (int refreshCount = 0; refreshCount < scrollspeed; refreshCount++)
        RefreshDisplay();
      //Shift the bitmap one column to left
      for (int row = 0; row < 8; row++)
      {
        for (int zone = 0; zone < numZones; zone++)
        {
          //This right shift would show as a left scroll on display because leftmost column is represented by least significant bit of the byte.
          bitmap[row][zone] = bitmap[row][zone] >> 1;
          // Shift over lowest bit from the next zone as highest bit of this zone.
          if (zone < maxZoneIndex) bitWrite(bitmap[row][zone], 7, bitRead(bitmap[row][zone + 1], 0));
        }
      }
    }
  }
}


//FUNCTIONS
// Displays bitmap array in the matrix
void RefreshDisplay()
{
  for (int row = 0; row < 8; row++) {


    for (int zone = maxZoneIndex; zone >= 0; zone--)
    {
      shiftOut(dataPin1, clockPin1, LSBFIRST, ~bitmap[row][zone]);
    }
    shiftOut(dataPin1, clockPin1, LSBFIRST, (128 >> row));
    Store();

  }
}

// Converts row and colum to bitmap bit and turn it off/on
void Plot(int col, int row, bool isOn)
{
  int zone = col / 8;
  int colBitIndex = x % 8;
  byte colBit = 1 << colBitIndex;
  if (isOn)
    bitmap[row][zone] =  bitmap[y][zone] | colBit;
  else
    bitmap[row][zone] =  bitmap[y][zone] & (~colBit);
}
// Plot each character of the message one column at a time, updated the display, shift bitmap left.




void Store()
{ digitalWrite(latchPin1, 1);
  delayMicroseconds(10);
  digitalWrite(latchPin1, 0);
  delayMicroseconds(10);
}
