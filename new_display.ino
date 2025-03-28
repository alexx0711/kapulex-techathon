#include <SD.h>
#include <SPI.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library
#include <ft6336g.h> //touch library

//paramters define
#define MODEL ST7796S
#define CS   9    
#define CD   7
#define RST  8
#define MOSI  11
#define MISO  12
#define SCK   13
#define LED  -1   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V

//touch screen paramters define
#define OTT_MAX_TOUCH  2 
#define INT  5
#define CRST 6 
#define SCL  A5
#define SD_CS  4
#define SDA  A4

#define PIXEL_NUMBER  (my_lcd.Get_Display_Width()/4)

#define PINOUT 3

LCDWIKI_SPI my_lcd(MODEL,CS,CD,MISO,MOSI,RST,SCK,LED); //model,cs,dc,miso,mosi,reset,sck,led

//the definiens of touch mode as follow:
FT6336 my_tp(INT,CRST,SCL,SDA);



uint16_t s_width = my_lcd.Get_Display_Width();  
uint16_t s_heigh = my_lcd.Get_Display_Height();
// char file_name[FILE_NUMBER][FILE_NAME_SIZE_MAX];


void draw_bmp_picture(File fp)
{
  uint16_t i,j,k,l,m=0;
  uint8_t bpm_data[PIXEL_NUMBER*3] = {0};
  uint16_t bpm_color[PIXEL_NUMBER];
  fp.seek(54);
  for(i = 0;i < s_heigh;i++)
  {
    for(j = 0;j<s_width/PIXEL_NUMBER;j++)
    {
      m = 0;
      fp.read(bpm_data,PIXEL_NUMBER*3);
      for(k = 0;k<PIXEL_NUMBER;k++)
      {

        bpm_color[k]= my_lcd.Color_To_565(bpm_data[m+2], bpm_data[m+1], bpm_data[m+0]); //change to 565
        m +=3;
      }
      for(l = 0;l<PIXEL_NUMBER;l++)
      {
        my_lcd.Set_Draw_color(bpm_color[l]);
        my_lcd.Draw_Pixel(j*PIXEL_NUMBER+l,i);
      }    
     }
             Serial.print("Printing!");
   }    
}
//LED STUFF


//COLORS
#define BLACK        0x0000  /*   0,   0,   0 */
#define WHITE        0xFFFF  /* 255, 255, 255 */
#define SAGE_GREEN   0x53C8
#define DARK_GREEN   0x3B06


//BUTTONS


#define startX1 125
#define startY1 227
#define startX2 225
#define startY2 257

#define resetX1 250
#define resetY1 227
#define resetX2 350
#define resetY2 257


// typedef struct _button_info
// {
//      char button_name[20];
//      uint8_t button_name_size;
//      uint16_t button_name_colour;
//      uint16_t button_colour;
//      uint16_t button_x;
//      uint16_t button_y;     
// //  }button_info;

// button_info start = { "Start", 2, SAGE_GREEN, WHITE, startX1, startY1};
// button_info reset = { "Reset", 2, SAGE_GREEN, WHITE, resetX1, resetY1};

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc, boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}


boolean is_pressed(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t px,int16_t py)
{
    if((px > x1 && px < x2) && (py > y1 && py < y2))
    {
        my_tp.x[0]=0xFFFF;
        my_tp.y[0]=0xFFFF;
        return true;  
    } 
    else
    {
        return false;  
    }
 }


//TIME
#define hourX 130
#define hourY 130


#define minuteTenX 225
#define minuteTenY 130

#define minuteX 295
#define minuteY 130

int hour;
int minuteTens;
int minute;
int secondTens;
int second;
int totalTime;


void setup() {


  my_lcd.Init_LCD();
  Serial.begin(9600);
  // pinMode(SD_CS, OUTPUT);
   
  // if (!SD.begin(SD_CS)) 
  // {
  //   my_lcd.Set_Text_Back_colour(SAGE_GREEN);
  //   my_lcd.Set_Text_colour(WHITE);    
  //   my_lcd.Set_Text_Size(1);
  //   my_lcd.Print_String("SD Card Init fail!",0,0);
  // }
  // File bmp_file;
  // bmp_file = SD.open("01.bmp", 1);
  // Serial.print("SD File opened ");
  // draw_bmp_picture(bmp_file);
  // Serial.print("SD filed closed ");
  // bmp_file.close();



  cli();
  TCCR1A = 0;               // Normal operation (no PWM)
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, Prescaler = 1024
  OCR1A = 15624;            // (16MHz / (1024 * 1Hz)) - 1 = 15624 (1 sec interval)
  TIMSK1 |= (1 << OCIE1A);  // Enable Timer1 compare interrupt

  sei();
  // put your setup code here, to run once:

  my_lcd.Set_Rotation(1);




  // pinMode(SD_CS, OUTPUT);
  pinMode(PINOUT, OUTPUT);
  my_tp.FT6336_Init(0, my_lcd.Get_Display_Width(), my_lcd.Get_Display_Height());



  hour = 0;
  minuteTens = 0;
  minute = 2;
  secondTens = 0;
  second = 0;
  // my_lcd.Fill_Screen(SAGE_GREEN); 


   
     
  my_lcd.Set_Text_Mode(0);
  my_lcd.Set_Text_Size(10);
  my_lcd.Set_Text_colour(WHITE);
  my_lcd.Set_Text_Back_colour(DARK_GREEN);
  my_lcd.Print_Number_Int(hour, 130, 130, 0, ' ',10);
  my_lcd.Draw_Char(180, 125, ':', WHITE, DARK_GREEN, 10, 1);
  my_lcd.Print_Number_Int(minuteTens, 225, 130, 0, ' ',10);
  my_lcd.Print_Number_Int(minute, 295, 130, 0, ' ',10);

  my_lcd.Set_Draw_color(WHITE);


  my_lcd.Fill_Round_Rectangle(125, 227, 225, 257, 3); // Increase height
  show_string("Start", startX1+5, startY1+5, 2, SAGE_GREEN, WHITE, 1);

  my_lcd.Fill_Round_Rectangle(250, 227, 350, 257, 3); // Increase height
  show_string("Reset", resetX1+5, resetY1+5, 2, SAGE_GREEN, WHITE, 1);

// my_lcd.Fill_Round_Rectangle(20, 100, 240, 160, 5);
// show_string(start.button_name, start.button_x, start.button_y, start.button_name_size, start.button_name_colour, start.button_colour, 1);
// my_lcd.Print_String("Start/Pause", 25, 105);


// // my_lcd.Print_String("Reset", 25, 185);
// show_string(reset.button_name, reset.button_x, reset.button_y, reset.button_name_size, reset.button_name_colour, reset.button_colour, 1);
// my_lcd.Fill_Round_Rectangle(80, 20, 240, 100, 5);

  totalTime = second + secondTens * 10 + minute * 60 + minuteTens * 600 + hour * 3600;
}


volatile bool countdownActive = false; // Timer starts only when true
volatile bool resetFlag = false;
volatile bool minuteFlag = false;
volatile bool minuteTensFlag = false;
volatile bool hourFlag = false;
// volatile int prevMinuteTens = -1;  // Store previous tens value

void updateHour() {
  my_lcd.Set_Draw_color(DARK_GREEN);
  my_lcd.Fill_Rectangle(hourX, hourY, hourX+55, hourY+75);
  my_lcd.Set_Text_Size(10);
  my_lcd.Set_Text_colour(WHITE);
  my_lcd.Print_Number_Int(hour, hourX, hourY, 0, ' ', 10);
  hourFlag = false;
}

void updateMinutesUp() {
  int8_t newMinuteTens = (minute / 10) % 6;  // Extract tens place, stays in range 0-5
  int8_t minuteOnes = minute % 10;           // Extract ones place
      my_lcd.Set_Draw_color(DARK_GREEN);
    my_lcd.Set_Text_colour(WHITE);
    my_lcd.Set_Text_Size(10);
  // If minutes reach 60, increment hour and reset minutes
  if (minute >= 60) {
    minute -= 60;
    hour++;
    updateHour();
  }

  // Only update minuteTens if it changed
  // if (newMinuteTens != prevMinuteTens) {

    my_lcd.Fill_Rectangle(minuteTenX, minuteTenY, minuteTenX+55, minuteTenY+75);

    my_lcd.Print_Number_Int(newMinuteTens, minuteTenX, minuteTenY, 0, ' ', 10);
    
  //   prevMinuteTens = newMinuteTens;  // Update stored value
  // }

  // Always update ones place
  my_lcd.Fill_Rectangle(minuteX, minuteY, minuteX+55, minuteTenY+75);
  my_lcd.Print_Number_Int(minuteOnes, minuteX, minuteY, 0, ' ', 10);
}

void updateMinutesDown(){
  my_lcd.Set_Draw_color(DARK_GREEN);
  my_lcd.Fill_Rectangle(minuteX, minuteY, minuteX+55, minuteY+75);
  my_lcd.Set_Text_Size(10);
  my_lcd.Set_Text_colour(WHITE);
  my_lcd.Print_Number_Int(minute, minuteX, minuteY, 0, ' ', 10);
  minuteFlag = false;
}

void updateMinuteTens(){
     my_lcd.Set_Draw_color(DARK_GREEN);
    my_lcd.Fill_Rectangle(minuteTenX, minuteTenY, minuteTenX+55, minuteTenY+75);
    my_lcd.Set_Text_colour(WHITE);
    my_lcd.Print_Number_Int(minuteTens, minuteTenX, minuteTenY, 0, ' ', 10);
    minuteTensFlag = false;
}

void resetLoop(){
  my_tp.FT6336_Scan();
    if (is_pressed(10, 125, 60, 175, my_tp.x[0], my_tp.y[0]) && my_tp.ctp_status&(1<<0)){
      minute+=5;
      updateMinutesUp();
    }
    if (is_pressed(10, 185, 60, 235, my_tp.x[0], my_tp.y[0]) && my_tp.ctp_status&(1<<0)){
      minute +=15;
      updateMinutesUp();
    }
    if (is_pressed(10, 245, 60, 295, my_tp.x[0], my_tp.y[0]) && my_tp.ctp_status&(1<<0)){
      minute +=30;
      updateMinutesUp();
    }
    if (is_pressed(10, 305, 60, 355, my_tp.x[0], my_tp.y[0]) && my_tp.ctp_status&(1<<0)){
    hour = 0;
    updateHour();
    minuteTens = 0;
    my_lcd.Fill_Rectangle(225, 125, 280, 200);
    my_lcd.Print_Number_Int(0, 225, 125, 0, ' ', 10);
    minute = 0;
    my_lcd.Fill_Rectangle(300, 125, 355, 200);
    my_lcd.Print_Number_Int(0, 300, 125, 0, ' ', 10);
    }

  
if (is_pressed(320-startY2, startX1, 320-startY1, startX2, my_tp.x[0], my_tp.y[0]) && my_tp.ctp_status&(1<<0)){ //start_button
  countdownActive = true;
  resetFlag = false;
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Fill_Round_Rectangle(resetX1, resetY1, resetX2, resetY2, 3);
    show_string("Reset", resetX1+5, resetY1+5, 2, SAGE_GREEN, WHITE, 1);
  my_lcd.Set_Draw_color(SAGE_GREEN);
  my_lcd.Fill_Round_Rectangle(125, 260, 355, 310, 3);
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Fill_Round_Rectangle(startX1, startY1, startX2, startY2, 3); // Increase height
    show_string("Pause", startX1+5, startY1+5, 2, SAGE_GREEN, WHITE, 1);


}
}


void loop() {
  if(resetFlag){
    resetLoop();
  }
  else if (minuteFlag){
    updateMinutesDown();
  }
  else if (minuteTensFlag){
    updateMinuteTens();
  }
  else if (hourFlag){
    updateHour();
  }
  else{
  // if(countdownActive && totalTime>0){
  //   digitalWrite(GREEN_PIN, HIGH);
  //   digitalWrite(RED_PIN, LOW);
  //   digitalWrite(BLUE_PIN, LOW);
  // }
  // else if (!countdownActive && totalTime>0){
  //   digitalWrite(RED_PIN, HIGH);
  //   digitalWrite(GREEN_PIN, LOW);
  //   digitalWrite(BLUE_PIN, LOW);
  // }


// my_lcd.Set_Draw_color(WHITE);
// my_lcd.Fill_Round_Rectangle(20, 20, 240, 60, 5);



  my_tp.FT6336_Scan();
    
    if (is_pressed(320-startY2, startX1, 320-startY1, startX2, my_tp.x[0], my_tp.y[0]) && my_tp.ctp_status&(1<<0)){
    my_lcd.Set_Draw_color(WHITE);
    my_lcd.Fill_Round_Rectangle(startX1, startY1, startX2, startY2, 3); // Increase height
    countdownActive = !countdownActive;

    if (countdownActive){
    show_string("Pause", startX1+5, startY1+5, 2, SAGE_GREEN, WHITE, 1);
    }
    else{
    show_string("Start", startX1+5, startY1+5, 2, SAGE_GREEN, WHITE, 1);
    }

  }
    if (is_pressed(320-resetY2, resetX1, 320-resetY1, resetX2, my_tp.x[0], my_tp.y[0]) && my_tp.ctp_status&(1<<0)){
    my_lcd.Set_Draw_color(SAGE_GREEN);
    my_lcd.Fill_Round_Rectangle(resetX1, resetY1, resetX2, resetY2, 3); // Increase height
    show_string("Reset", resetX1+5, resetY1+5, 2, SAGE_GREEN, WHITE, 1);
    countdownActive = 0; //pause
    my_lcd.Set_Draw_color(WHITE);
        my_lcd.Fill_Round_Rectangle(startX1, startY1, startX2, startY2, 3); // Increase height
    show_string("Start", startX1+5, startY1+5, 2, SAGE_GREEN, WHITE, 1);

    resetFlag = true;



    hour = 0;
   updateHour();
    minuteTens = 0;
    updateMinuteTens();
    minute = 0;
    updateMinutesDown();
    secondTens = 0;
    second = 0;

    my_lcd.Set_Draw_color(WHITE);
    my_lcd.Fill_Circle(150, 285, 25);
    my_lcd.Set_Text_Size(2);
    my_lcd.Set_Text_colour(SAGE_GREEN);
    my_lcd.Print_String("+5", 135, 275);


    my_lcd.Fill_Circle(210, 285, 25);
    my_lcd.Print_String("+15", 190, 275);

    my_lcd.Fill_Circle(270, 285, 25);
    my_lcd.Print("+30", 250, 275);

    my_lcd.Fill_Circle(330, 285, 25);
    my_lcd.Print("CLR", 313, 275);



    


    // updateTimeDisplay();



// my_lcd.Set_Text_colour(WHITE);
// my_lcd.Set_Text_Back_colour(PURP);
// my_lcd.Print_String("Main Screen", 0, 0);


  // put your main code here, to run repeatedly:

}}
}



ISR(TIMER1_COMPA_vect) {
    totalTime = second + secondTens * 10 + minute * 60 + minuteTens * 600 + hour * 3600;
    if (totalTime > 0 && countdownActive) {
        digitalWrite(PINOUT, LOW);
        totalTime--;  // Decrease total time (in seconds)
        // digitalWrite(BLUE_PIN, !digitalRead(BLUE_PIN));
        // digitalWrite(RED_PIN, !digitalRead(RED_PIN));
        // digitalWrite(GREEN_PIN, !digitalRead(GREEN_PIN));
        // Update hours, minutes, and seconds
        int remaining = totalTime;
        int newHour = remaining / 3600;
        remaining %= 3600;
        int newMinuteTens = (remaining / 600);

        int newMinute = (remaining % 600) / 60;
        secondTens = (remaining % 60) / 10;
        second = remaining % 10;
      if (newHour != hour){
        hour = newHour; 
        hourFlag = true;
      }
            if (newMinuteTens != minuteTens){
        minuteTens = newMinuteTens; 
        minuteTensFlag = true;

      }

      if (newMinute != minute){
        minute = newMinute;
        minuteFlag = true;

      }

    } else if (totalTime == 0) {
      digitalWrite(PINOUT, HIGH);
//DONE!
    }
}
