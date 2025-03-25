#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library
#include <ft6336g.h> //touch library

//paramters define
#define MODEL ST7796S
#define CS   10    
#define CD   9
#define RST  8
#define MOSI  11
#define MISO  12
#define SCK   13
#define LED  -1   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V

//touch screen paramters define
#define OTT_MAX_TOUCH  2 
#define INT  7
#define CRST 6 
#define SCL  A5
#define SDA  A4

LCDWIKI_SPI my_lcd(MODEL,CS,CD,MISO,MOSI,RST,SCK,LED); //model,cs,dc,miso,mosi,reset,sck,led

//the definiens of touch mode as follow:
FT6336 my_tp(INT,CRST,SCL,SDA);

//LED STUFF
#define LED_PIN 4 


//COLORS
#define BLACK        0x0000  /*   0,   0,   0 */
#define BLUE         0x001F  /*   0,   0, 255 */
#define RED          0xF800  /* 255,   0,   0 */
#define GREEN        0x07E0  /*   0, 255,   0 */
#define CYAN         0x07FF  /*   0, 255, 255 */
#define MAGENTA      0xF81F  /* 255,   0, 255 */
#define YELLOW       0xFFE0  /* 255, 255,   0 */
#define WHITE        0xFFFF  /* 255, 255, 255 */
#define NAVY         0x000F  /*   0,   0, 128 */
#define DARKGREEN    0x03E0  /*   0, 128,   0 */
#define DARKCYAN     0x03EF  /*   0, 128, 128 */
#define MAROON       0x7800  /* 128,   0,   0 */
#define PURPLE       0x780F  /* 128,   0, 128 */
#define OLIVE        0x7BE0  /* 128, 128,   0 */
#define LIGHTGREY    0xC618  /* 192, 192, 192 */
#define DARKGREY     0x7BEF  /* 128, 128, 128 */
#define ORANGE       0xFD20  /* 255, 165,   0 */
#define GREENYELLOW  0xAFE5  /* 173, 255,  47 */
#define PINK         0xF81F  /* 255,   0, 255 */
#define PURP         0xB2AC

/******************* UI details */
#define BUTTON_R 35
#define BUTTON_SPACING_X 35
#define BUTTON_SPACING_Y 10
#define EDG_Y 10
#define EDG_X 20

typedef struct _button_info
{
     char button_name[20];
     uint8_t button_name_size;
     uint16_t button_name_colour;
     uint16_t button_colour;
     uint16_t button_x;
     uint16_t button_y;     
 }button_info;

button_info start = { "Start/Pause", 3, BLACK, WHITE, 25, 105 };
button_info reset = { "Reset", 3, BLACK, WHITE, 25, 185 };

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
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
int hour;
int minuteTens;
int minute;
int secondTens;
int second;
int totalTime;


void setup() {
  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
  cli();
  TCCR1A = 0;               // Normal operation (no PWM)
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, Prescaler = 1024
  OCR1A = 15624;            // (16MHz / (1024 * 1Hz)) - 1 = 15624 (1 sec interval)
  TIMSK1 |= (1 << OCIE1A);  // Enable Timer1 compare interrupt

  sei();
  // my_tp.FT6336_Init(1, my_lcd.Get_Display_Width(), my_lcd.Get_Display_Height());
  // put your setup code here, to run once:
  my_lcd.Init_LCD();
  my_lcd.Set_Rotation(1);
  //    if(my_tp.FT6336_Init(my_lcd.Get_Rotation(),my_lcd.Get_Display_Width(),my_lcd.Get_Display_Height()))
  //  {
  //     return; 
  //  } 
  hour = 0;
  minuteTens = 0;
  minute = 0;
  secondTens = 0;
  second = 5;
  my_lcd.Fill_Screen(WHITE); 
  my_lcd.Set_Text_Mode(0);
  my_lcd.Set_Text_colour(BLACK);
  my_lcd.Set_Text_Back_colour(WHITE);
   my_lcd.Set_Text_Size(3);
  my_lcd.Print_String("Welcome!", 175, 125);
  my_lcd.Print_String("Touch Screen To Begin", 60, 175);

  while (!(my_tp.ctp_status&(1<<0))){
    my_tp.FT6336_Scan();
  }
  my_lcd.Fill_Screen(PURP);



my_lcd.Set_Draw_color(WHITE);
my_lcd.Fill_Round_Rectangle(20, 20, 240, 60, 5);
  // updateTimeDisplay();
// my_lcd.Print_Number_Int(hour, 23, 23, 0, ' ',10);
// my_lcd.Draw_Char(35, 23, ':', BLACK, PURP, 3, 1);

// my_lcd.Print_Number_Int(minuteTens, 50, 23, 0, ' ',10);
// my_lcd.Print_Number_Int(minute, 67, 23, 0, ' ',10);
// my_lcd.Draw_Char(79, 23, ':', BLACK, PURP, 3, 1);
// my_lcd.Print_Number_Int(secondTens, 102, 23, 0, ' ',10);
// my_lcd.Print_Number_Int(second, 119, 23, 0, ' ',10);

my_lcd.Fill_Round_Rectangle(20, 100, 240, 160, 5);
show_string(start.button_name, start.button_x, start.button_y, start.button_name_size, start.button_name_colour, start.button_colour, 1);
// my_lcd.Print_String("Start/Pause", 25, 105);
my_lcd.Fill_Round_Rectangle(20, 180, 240, 240, 5);
// my_lcd.Print_String("Reset", 25, 185);
show_string(reset.button_name, reset.button_x, reset.button_y, reset.button_name_size, reset.button_name_colour, reset.button_colour, 1);
// my_lcd.Fill_Round_Rectangle(80, 20, 240, 100, 5);
// my_lcd.Fill_Round_Rectangle(140, 20, 240, 140, 5);
my_lcd.Set_Draw_color(GREEN);
my_lcd.Fill_Round_Rectangle(260, 20, 460, 280, 5);
my_tp.x[0] = 0;
my_tp.y[0] = 0;
second = 5;
}

// Example of creating a 3x5 matrix sprite for each digit (0-9)
// Define each digit as a 3x5 matrix, where 1 represents a pixel on, and 0 represents a pixel off.

// Sprites for digits 0-9 (8x8 pixels each)
uint8_t digit_sprites[10][8] = {
    // Digit 0
    {0x3E, 0x51, 0x49, 0x45, 0x51, 0x3E, 0x00, 0x00},
    // Digit 1
    {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00},
    // Digit 2
    {0x62, 0x51, 0x49, 0x49, 0x49, 0x46, 0x00, 0x00},
    // Digit 3
    {0x22, 0x41, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    // Digit 4
    {0x18, 0x14, 0x12, 0x7F, 0x10, 0x10, 0x00, 0x00},
    // Digit 5
    {0x4F, 0x49, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00},
    // Digit 6
    {0x3E, 0x49, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00},
    // Digit 7
    {0x01, 0x01, 0x7F, 0x01, 0x01, 0x01, 0x00, 0x00},
    // Digit 8
    {0x36, 0x49, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    // Digit 9
    {0x46, 0x49, 0x49, 0x49, 0x49, 0x3E, 0x00, 0x00}
};


void drawDigit(int x, int y, uint8_t digit) {
  my_lcd.Set_Draw_color(BLACK);
  // This will draw a 3x5 pixel digit on the screen
  for (int i = 0; i < 8; i++) {
    uint8_t row = digit_sprites[digit][i];
    for (int j = 0; j < 8; j++) {
      if (row & (1 << j)) {
        my_lcd.Draw_Pixel(x + j, y + i);  // Draw the pixel
      }
    }
  }
}


bool startBool = 0;
bool resetBool = 0;
void loop() {
// my_lcd.Set_Draw_color(WHITE);
// my_lcd.Fill_Round_Rectangle(20, 20, 240, 60, 5);

  totalTime = second + secondTens * 10 + minute * 60 + minuteTens * 600 + hour * 3600;
  my_tp.FT6336_Scan();
    if (is_pressed(180, 20, 240, 240, my_tp.x[0], my_tp.y[0]) && my_tp.ctp_status&(1<<0)){
    my_lcd.Set_Draw_color(PURP);
    my_lcd.Fill_Round_Rectangle(20, 260, 240, 280, 5); // Increase height
    show_string("Paused", 20, 260, 3, BLACK, WHITE, 1);
  }
    if (is_pressed(100, 20, 160, 240, my_tp.x[0], my_tp.y[0]) && my_tp.ctp_status&(1<<0)){
    my_lcd.Set_Draw_color(PURP);
    my_lcd.Fill_Round_Rectangle(20, 260, 240, 280, 5); // Increase height
    show_string("Reset", 20, 260, 3, BLACK, WHITE, 1);
        // resetTimer(); // Reset the timer

  }

    updateTimeDisplay();



// my_lcd.Set_Text_colour(WHITE);
// my_lcd.Set_Text_Back_colour(PURP);
// my_lcd.Print_String("Main Screen", 0, 0);


  // put your main code here, to run repeatedly:

}


void updateTimeDisplay() {
  // Draw the hour digits
  drawDigit(23, 23, hour);
  drawDigit(50, 23, minuteTens);
  drawDigit(67, 23, minute);
  drawDigit(102, 23, secondTens);
  drawDigit(119, 23, second);
}


// void resetTimer() {
//   // Reset the countdown timer variables to initial values
//   hour = 0;
//   minuteTens = 0;
//   minute = 0;
//   secondTens = 0;
//   second = 5;
//   totalTime = second + secondTens * 10 + minute * 60 + minuteTens * 600 + hour * 3600;
//   updateTimeDisplay();
// }

ISR(TIMER1_COMPA_vect) {
  if (totalTime > 0) {
    totalTime--;  // Decrease total time (in seconds)
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // Blink LED

    // Update hours, minutes, and seconds
    int remaining = totalTime;
    hour = remaining / 3600;
    remaining %= 3600;
    minuteTens = (remaining / 600);
    minute = (remaining % 600) / 60;
    secondTens = (remaining % 60) / 10;
    second = remaining % 10;

    // Update the LCD with the current time
    updateTimeDisplay();
  } else {
    digitalWrite(LED_PIN, HIGH); // Keep LED on when countdown finishes
  }
}