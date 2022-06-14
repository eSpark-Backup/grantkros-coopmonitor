#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AudioPlayer.h>
#include <RTCZero.h>
#include <images.h>
#include "ArduinoLowPower.h"
#include "wiring_private.h"       //*****addition*****
TwoWire myWire(&sercom2, 22, 38); //*****addition*****

#define Serial SerialUSB

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels (before : 64 )

#define NEXT 7
#define BACK 6
#define UP 5
#define DOWN 3

#define DEBOUNCE 200

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &myWire, OLED_RESET);

#define LOGO_HEIGHT   65
#define LOGO_WIDTH    129

#define BATTERY_WIDTH 17
#define BATTERY_HEIGHT 11

#define CURRENT_TIME 0
#define OPEN_TIME 1
#define CLOSE_TIME 2
#define OPEN_TIME_LAST_CHANCE 3

#define HOURS 0
#define MINUTES 1
#define AM_PM 2

#define TIME_DIMENSIONS 3
#define TIME_TYPES 4

#define OUTSIDE 1
#define ENTERED 2

#define FIRST 0
#define SECOND 1
#define THIRD 2
#define FOURTH 3
#define FIFTH 4
#define SIXTH 5
#define SEVENTH 6
#define EIGHTH 7

#define MENU 10
#define CHANGE_OPEN_TIME MENU
#define CHANGE_CLOSE_TIME (MENU + 1)
#define CHANGE_CURRENT_TIME (MENU + 2)
#define FACTORY_RESET (MENU + 3)
#define CHANGE_STATUS (MENU + 4)
#define DEVICE_INFORMATION (MENU + 5)
#define DOOR_SOUND (MENU + 6)
#define LAST_CHANCE (MENU + 7)
#define CHANGE_LANGUAGE (MENU + 8)
#define TEST_MODE (MENU + 9)
#define REBOOT (MENU + 10)
#define POWER_OFF (MENU + 11)

#define SUBMENU 30
#define FACTORY_RESET_SUBMENU SUBMENU
#define CHANGE_STATUS_SUBMENU (SUBMENU + 1)
#define DEVICE_INFORMATION_SUBMENU (SUBMENU + 2)
#define DOOR_SOUND_SUBMENU (SUBMENU + 3)
#define LAST_CHANCE_SUBMENU (SUBMENU + 4)
#define REBOOT_SUBMENU (SUBMENU + 5)
#define POWER_OFF_SUBMENU (SUBMENU + 6)

#define DEVICE_DETAILS 40
#define FIRMWARE_VERSION (DEVICE_DETAILS)
#define SERIAL_NUMBER (DEVICE_DETAILS + 1)

#define LAST_CHANCE_TIMER 50
#define LAST_CHANCE_TIMER_SET_TIME (LAST_CHANCE_TIMER + 1)
#define LAST_CHANCE_TIMER_MINS (LAST_CHANCE_TIMER + 2)

#define MINS_5 0
#define MINS_10 1
#define MINS_15 2

#define HIGHLIGHTER_LEN_MENU 128
#define HIGHLIGHTER_HEIGHT_MENU 10
#define HIGHLIGHTER_LEN 20
#define SPACING 6

#define BATTERY_FULL 0
#define BATTERY_HALF 1
#define BATTERY_DEAD 2

uint8_t state = OUTSIDE;
uint8_t previous_state = OUTSIDE;
uint8_t time_keep[TIME_TYPES][TIME_DIMENSIONS] = {0};
uint8_t time_position = 0;
uint8_t position = 0;
uint8_t status_position = 0;
uint8_t device_position = 0;
uint8_t chance_position = 0;
uint8_t yes_no_position = 0;
uint8_t mins_position = 0;
uint8_t menu_count = 0;
uint8_t screen = 0;
uint8_t last_screen = 0;
uint32_t tick = 0;
uint8_t switched = false;
uint8_t entered_scroll_page_main = false;
uint8_t entered_scroll_page_status = false;
uint8_t entered_scroll_page_device = false;
uint8_t entered_scroll_page_lastChance = false;
uint8_t entered_scroll_page_reboot = false;
uint8_t entered_scroll_page_power_off = false;
uint8_t entered_scroll_page_mins = false;
uint32_t print_tick = 0;
uint8_t battery_pc = 100;
uint8_t last_battery_pc = 100;
uint8_t battery_icon = BATTERY_FULL;
uint8_t last_battery_icon = BATTERY_FULL;
uint8_t last_chance_open_state = false;
uint8_t last_chance_mins = 0;
uint8_t door_state = false;
uint8_t door_state_test_mode = false;
uint32_t test_mode_time_tracking = 0;

RTCZero rtc;

const uint8_t farmSound[] = {0x02, 0x00, 0x77, 0x6e, 0x6e, 0x69, 0x67, 0x69, 0x6e, 0x70, 0x73, 0x74, 0x74, 0x77, 0x7a, 0x7c, 0x80, 0x86, 0x8c, 0x8c, 0x8c, 0x8b, 0x8b, 0x8a, 0x83, 0x7e, 0x7c, 0x7a, 0x7a, 0x7a, 0x7b, 0x7e, 0x83, 0x88, 0x8b, 0x8e, 0x8e, 0x8b, 0x8a, 0x8d, 0x8d, 0x8d, 0x8d, 0x8e, 0x8f, 0x8e, 0x8d, 0x89, 0x85, 0x85, 0x83, 0x7e, 0x7a, 0x79, 0x7b, 0x7a, 0x76, 0x74, 0x79, 0x7f, 0x7f, 0x7f, 0x86, 0x8c, 0x90, 0x90, 0x8f, 0x91, 0x93, 0x91, 0x8e, 0x8c, 0x8c, 0x8b, 0x88, 0x86, 0x83, 0x80, 0x7d, 0x7b, 0x7b, 0x7c, 0x7b, 0x79, 0x74, 0x6e, 0x6e, 0x6e, 0x6e, 0x72, 0x79, 0x80, 0x83, 0x86, 0x8d, 0x90, 0x8f, 0x8c, 0x88, 0x87, 0x88, 0x87, 0x85, 0x86, 0x88, 0x88, 0x89, 0x89, 0x88, 0x86, 0x84, 0x82, 0x83, 0x83, 0x83, 0x84, 0x87, 0x89, 0x88, 0x87, 0x85, 0x86, 0x8a, 0x8a, 0x87, 0x88, 0x89, 0x8b, 0x88, 0x84, 0x86, 0x88, 0x87, 0x80, 0x7c, 0x7d, 0x7f, 0x7f, 0x7c, 0x79, 0x7b, 0x7d, 0x7c, 0x79, 0x7b, 0x7d, 0x7c, 0x7c, 0x7d, 0x7c, 0x7a, 0x7a, 0x7d, 0x80, 0x83, 0x85, 0x82, 0x80, 0x81, 0x7e, 0x7a, 0x77, 0x78, 0x7c, 0x7c, 0x79 ,0x78, 0x7c, 0x7f, 0x7b, 0x79, 0x7a, 0x7c, 0x80, 0x82, 0x84, 0x88, 0x8c, 0x91, 0x95, 0x97, 0x97, 0x94, 0x93, 0x92, 0x90, 0x8d, 0x89, 0x86, 0x83, 0x7e, 0x79, 0x73, 0x6d, 0x6a, 0x6a, 0x6b, 0x69, 0x6b, 0x70, 0x72, 0x72, 0x75, 0x7b, 0x80, 0x82, 0x82, 0x85, 0x8a, 0x8f, 0x8f, 0x8e, 0x91, 0x92, 0x8f, 0x8b, 0x8a, 0x8a, 0x88, 0x82, 0x7c, 0x79, 0x75, 0x71, 0x70, 0x74, 0x7a, 0x7d, 0x7e, 0x7c, 0x7c, 0x7e, 0x7d, 0x7c, 0x7d, 0x7f, 0x82, 0x81, 0x7f, 0x80, 0x81, 0x7f, 0x78, 0x74, 0x73, 0x74, 0x74, 0x72, 0x6e, 0x71, 0x75, 0x77, 0x79, 0x7b, 0x7c, 0x7c, 0x80, 0x84, 0x86, 0x8a, 0x8f, 0x8f, 0x8e, 0x8d, 0x8a, 0x84, 0x82, 0x81, 0x7e, 0x79, 0x75, 0x75, 0x77, 0x76, 0x74, 0x77, 0x7d, 0x7e, 0x7b, 0x7b, 0x7e, 0x80, 0x80, 0x7e, 0x7f, 0x82, 0x84, 0x84, 0x85, 0x87, 0x87, 0x85, 0x83, 0x81, 0x7d, 0x78, 0x73, 0x71, 0x70, 0x6f, 0x6e, 0x6c, 0x6b, 0x6c, 0x70, 0x72, 0x71, 0x77, 0x7d, 0x7f, 0x80, 0x83, 0x86, 0x87, 0x85, 0x7f, 0x7b, 0x7e, 0x81, 0x80, 0x7f, 0x82, 0x84, 0x85, 0x85, 0x84, 0x82, 0x7f, 0x7c, 0x7a, 0x79, 0x7a, 0x7c, 0x7e, 0x80, 0x82, 0x83, 0x81, 0x81, 0x83, 0x85, 0x83, 0x80, 0x7e, 0x7e, 0x7d, 0x7a, 0x79, 0x7c, 0x7f, 0x7d, 0x7a, 0x7b, 0x7d, 0x7d, 0x7f, 0x7d, 0x7b, 0x7d, 0x7d, 0x7c, 0x7d, 0x81, 0x81, 0x81, 0x85, 0x87, 0x85, 0x85, 0x86, 0x87, 0x89, 0x8a, 0x87, 0x81, 0x7e, 0x7b, 0x75, 0x6f, 0x6c, 0x6e, 0x70, 0x6e, 0x6b, 0x6c, 0x6e, 0x6e, 0x6a, 0x68, 0x6a, 0x6e, 0x6f, 0x70, 0x74, 0x7b, 0x80, 0x85, 0x8b, 0x90, 0x92, 0x94, 0x94, 0x92, 0x91, 0x8f, 0x8b, 0x88, 0x85, 0x82, 0x7f, 0x7b, 0x77, 0x77, 0x7a, 0x78 }
const uint32_t farmSoundSize = sizeof(farmSound);
char menu[][64] = {"Change Open Time", "Change Close Time", "Change Current Time", "Factory Reset",
                   "Change Status", "Device Information", "Door Sound", "Last Chance",
                   "Change Language", "Test Mode", "Reboot", "Power Off"
                  };
char status_menu[][64] = {"PAUSE", "AUTOMATIC", "MANUAL"};
char device_menu[][64] = {"Firmware Version", "Serial Number"};
char yes_no[][64] = {"Yes", "No"};
char chance_menu[][64] = {"Turn on", "Turn off"};
char mins_menu[][64] = {"5 mins", "10 mins", "15 mins"};
char* selected_status = "AUTOMATIC";


void print_screen(uint8_t screens);
void update_time(uint8_t* time_to_print);
void draw_bg(uint8_t screens);
void draw_text(uint8_t*  time_to_print);
void display_time(uint8_t* time_to_print);
void NEXT_SCREEN();
void BACK_SCREEN();
void UP_SCREEN();
void DOWN_SCREEN();
void welcome_screen();
void print_menu();
void print_status_menu();
void print_devce_information();
void print_yes_no();
void print_last_chance_menu();
void print_mins_menu();
uint8_t adjust_menu_index();
void setRTCTime(uint8_t temp_screen);
void adjust_12_hours_format(uint8_t temp_screen);
void print_battery_icon();
void print_battery_pc();
void clear_battery_icon();
void clear_battery_pc();
void get_battery_status();
void convert_to_hh_mm(uint8_t* temp_mins, uint8_t* temp_hours);

void setup() {
  Serial.begin(115200);
  pinPeripheral(22, PIO_SERCOM);
  pinPeripheral(38, PIO_SERCOM);
  rtc.begin();

  DACSetup(16000, 4);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS, false,false)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
    {
      Serial.println(F("SSD1306 allocation failed"));
    }
  }

  pinMode(NEXT, INPUT_PULLUP);
  pinMode(BACK, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(NEXT), NEXT_SCREEN, FALLING);
  attachInterrupt(digitalPinToInterrupt(BACK), BACK_SCREEN, FALLING);
  attachInterrupt(digitalPinToInterrupt(UP), UP_SCREEN, FALLING);
  attachInterrupt(digitalPinToInterrupt(DOWN), DOWN_SCREEN, FALLING);

  display.clearDisplay();
  display.display();

  welcome_screen();

  delay(5000);

  display.clearDisplay();
  display.display();

  print_screen(FIRST);
}

void loop() {

  // Update battery percentage if changed
  if ((last_battery_pc != battery_pc))
  {
    last_battery_pc = battery_pc;
    print_battery_pc();
  }

  // Update battery icon if changed
  if ((last_battery_icon != battery_icon))
  {
    last_battery_icon = battery_icon;
    print_battery_icon();
  }

  if (switched == true)
  {
    if (state == ENTERED)
    {
      if (((screen < FOURTH) && (screen >= FIRST)) || ((screen >= (MENU + 0)) && (screen < (MENU + 3))) || (screen == LAST_CHANCE_TIMER_SET_TIME))
      {
        uint8_t temp_screen = 0;
        if (screen < FOURTH)
        {
          temp_screen = screen;
        }
        else if ((screen >= (MENU + 0)) && (screen < (MENU + 3)))
        {
          switch (screen)
          {
            case (MENU + 2):
              temp_screen = CURRENT_TIME;
              break;

            case (MENU):
              temp_screen = OPEN_TIME;
              break;

            case (MENU + 1):
              temp_screen = CLOSE_TIME;
              break;
          }
        }
        else if ((screen >= LAST_CHANCE_TIMER))
        {
          switch (screen)
          {
            case LAST_CHANCE_TIMER_SET_TIME:
              temp_screen = OPEN_TIME_LAST_CHANCE;
              break;
          }
        }

        Serial.println("temp_screen");
        Serial.println(temp_screen);
        update_time(time_keep[temp_screen]);
      }

      switched = false;
    }

    if ((state == OUTSIDE) || (entered_scroll_page_main) || (entered_scroll_page_status) || (entered_scroll_page_device) || (entered_scroll_page_lastChance) || (entered_scroll_page_reboot) || (entered_scroll_page_power_off) || (entered_scroll_page_mins))
    {
      if (screen <= FIFTH)
      {
        display.clearDisplay();
        display.display();
      }

      print_screen(screen);

      switched = false;
    }
  }

  // Used for returning back to normal screen for time screens
  if (screen < FOURTH)
  {
    if (state != previous_state)
    {
      if (state == OUTSIDE)
      {
        display.clearDisplay();
        display.display();

        print_screen(screen);
      }

      previous_state = state;
    }
  }

  // Print diagnostic messages
  if ((millis() - print_tick) > 2000)
  {
    get_battery_status();

    Serial.println("Hours");
    Serial.println(rtc.getHours());

    Serial.println("Minutes");
    Serial.println(rtc.getMinutes());

    Serial.println("Open  Hours");
    Serial.println(time_keep[OPEN_TIME][HOURS]);

    Serial.println("Open Minutes");
    Serial.println(time_keep[OPEN_TIME][MINUTES]);

    Serial.println("Close Hours");
    Serial.println(time_keep[CLOSE_TIME][HOURS]);

    Serial.println("Close Minutes");
    Serial.println(time_keep[CLOSE_TIME][MINUTES]);

    Serial.println("State");
    Serial.println(state);

    Serial.println("Scroll page main");
    Serial.println(entered_scroll_page_main);

    Serial.println("Scroll page status");
    Serial.println(entered_scroll_page_status);

    Serial.println("Scroll page device");
    Serial.println(entered_scroll_page_device);

    Serial.println("Scroll page last chance");
    Serial.println(entered_scroll_page_lastChance);

    Serial.println("Scroll page Reboot");
    Serial.println(entered_scroll_page_reboot);

    Serial.println("Scroll page last power off");
    Serial.println(entered_scroll_page_power_off);

    Serial.println("Scroll page mins");
    Serial.println(entered_scroll_page_mins);

    Serial.println("Current screen");
    Serial.println(screen);

    // Open/Close door checking regular open/close time
    if ((rtc.getHours() > time_keep[OPEN_TIME][HOURS]) && (rtc.getHours() < time_keep[CLOSE_TIME][HOURS]))
    {
      door_state =  true;
      //      Serial.println("Door is opened");

    } else if ((rtc.getHours() == time_keep[OPEN_TIME][HOURS]) || (rtc.getHours() == time_keep[CLOSE_TIME][HOURS]))
    {
      if ((rtc.getMinutes() >= time_keep[OPEN_TIME][MINUTES]) && (rtc.getMinutes() < time_keep[CLOSE_TIME][MINUTES]))
      {
        door_state = true;
        //        Serial.println("Door is opened");
      } else if (rtc.getMinutes() >= time_keep[CLOSE_TIME][MINUTES])
      {
        door_state = false;
        //        Serial.println("Door is closed");
      }
      else if (rtc.getMinutes() < time_keep[OPEN_TIME][MINUTES])
      {
        door_state = false;
        //        Serial.println("Door is closed");
      }
    }
    else
    {
      door_state = false;
      //      Serial.println("Door is closed");
    }

    uint8_t new_mins, new_hours = 0;
    // Convert Last chance open time window to HH:MM
    convert_to_hh_mm(&new_mins, &new_hours);

    Serial.println("New hours");
    Serial.println(new_hours);

    Serial.println("New mins");
    Serial.println(new_mins);

    Serial.println("lat chance hours");
    Serial.println(time_keep[OPEN_TIME_LAST_CHANCE][HOURS]);

    Serial.println("last chance mins");
    Serial.println(time_keep[OPEN_TIME_LAST_CHANCE][MINUTES]);

    // Open/Close door checking last chance open time
    if ((rtc.getHours() > time_keep[OPEN_TIME_LAST_CHANCE][HOURS]) && (rtc.getHours() < new_hours))
    {
      door_state =  true;
      //      Serial.println("Door is opened");

    } else if ((rtc.getHours() == time_keep[OPEN_TIME_LAST_CHANCE][HOURS]))
    {
      if ((rtc.getMinutes() >= time_keep[OPEN_TIME_LAST_CHANCE][MINUTES]) && (rtc.getMinutes() < new_mins))
      {
        door_state =  true;
        //        Serial.println("Door is opened");
      } else if (rtc.getMinutes() >= new_mins)
      {
        door_state =  false;
        //        Serial.println("Door is closed");
      }
    }

    Serial.println("Door state");
    Serial.println(door_state ? "Opened" : "Closed");

    print_tick = millis();

  }

  // Test mode
  if (screen == TEST_MODE)
  {
    if ((millis() - test_mode_time_tracking) > 10000)
    {
      door_state_test_mode = !door_state_test_mode;
      Serial.print("Test mode door state: ");
      Serial.println(door_state_test_mode ? "Opened" : "Closed");

      test_mode_time_tracking = millis();
    }
  }
}

void convert_to_hh_mm(uint8_t* temp_mins, uint8_t* temp_hours)
{
  uint8_t add_mins = 0;
  //  uint8_t temp_mins, temp_hours = 0;

  switch (last_chance_mins)
  {
    case MINS_5:
      add_mins = 5;
      break;

    case MINS_10:
      add_mins = 10;
      break;

    case MINS_15:
      add_mins = 15;
      break;
  }

  *temp_mins = time_keep[OPEN_TIME_LAST_CHANCE][MINUTES] + add_mins;

  if (*temp_mins >= 60)
  {
    *temp_mins -= 60;
    *temp_hours = time_keep[OPEN_TIME_LAST_CHANCE][HOURS] + 1;
  } else
  {
    *temp_hours = time_keep[OPEN_TIME_LAST_CHANCE][HOURS];
  }

  // Reset hours to 1 in case of greater than 12/24
  if (time_keep[OPEN_TIME_LAST_CHANCE][AM_PM] == 0)
  {
    if (*temp_hours > 12)
    {
      *temp_hours = 1;
    }
  }
  else if (time_keep[OPEN_TIME_LAST_CHANCE][AM_PM] == 1)
  {
    if (*temp_hours > 24)
    {
      *temp_hours = 1;
    }
  }

}

void print_screen(uint8_t screens)
{
  uint8_t draw_text_screen = 0;

  if (screens > OPEN_TIME_LAST_CHANCE)
  {
    draw_text_screen = 0;
  }
  else {
    draw_text_screen = screens;
  }

  print_battery_pc();
  print_battery_icon();
  draw_bg(screens);
  draw_text(time_keep[draw_text_screen]);
}

void print_battery_pc()
{
  clear_battery_pc();

  display.setTextColor(WHITE);
  display.setFont(&FreeSans5pt7b);
  display.setCursor(display.width() - BATTERY_WIDTH - 25, 7);

  // Pad extra zero if battery < 10
  if (battery_pc >= 10)
  {
    display.print(String(battery_pc)); display.print("%");
  } else {
    display.print(0); display.print(battery_pc); display.print("%");
  }

  display.display();
}

void print_battery_icon()
{
  clear_battery_icon();

  display.drawBitmap(display.width() - BATTERY_WIDTH,
                     -1,
                     battery_icons[battery_icon] , BATTERY_WIDTH, BATTERY_HEIGHT, 1);
  display.display();

}

void get_battery_status()
{
  // random number generated for batterty percentage
  battery_pc = random(0, 100);

  if (battery_pc <= 10)
  {
    battery_icon =  BATTERY_DEAD;
  } else if (battery_pc <= 50)
  {
    battery_icon = BATTERY_HALF;
  } else if (battery_pc <= 100)
  {
    battery_icon = BATTERY_FULL;
  }
}

void clear_battery_pc()
{
  display.fillRect(display.width() - BATTERY_WIDTH - 25, -1, 25, BATTERY_HEIGHT - 2, BLACK);
}

void clear_battery_icon()
{
  display.fillRect(display.width() - BATTERY_WIDTH, -1, BATTERY_WIDTH, BATTERY_HEIGHT - 2, BLACK);
}

void update_time(uint8_t* time_to_print)
{
  String hours, minutes, am_pm = "";
  uint8_t temp_pm_hours = 0;

  display.fillRect(0, 32, 128, HIGHLIGHTER_LEN, BLACK);
  display.display();

  if (time_position == 0)
  {
    display.fillRect(25, 32, HIGHLIGHTER_LEN, 14, WHITE);
    display.display();
  } else if (time_position == 1)
  {
    display.fillRect(25 + HIGHLIGHTER_LEN + SPACING, 32, HIGHLIGHTER_LEN, 14, WHITE);
    display.display();
  } else if (time_position == 2)
  {
    display.fillRect(25 + 2 * HIGHLIGHTER_LEN + 2 * SPACING + 5, 32, HIGHLIGHTER_LEN + 5, 14, WHITE);
    display.display();
  }



  // case PM
  if (time_to_print[AM_PM] == 1)
  {
    temp_pm_hours = time_to_print[HOURS] - 12;
  }
  else if (time_to_print[AM_PM] == 0)
  {
    temp_pm_hours = time_to_print[HOURS];
  }

  // 00:00 AM
  display.setFont(&FreeSans9pt7b);
  display.setCursor(25, 45);

  if (temp_pm_hours < 10)
  {
    hours = "0";
    hours += String(temp_pm_hours);
  } else
  {
    hours += String(temp_pm_hours);
  }

  if (time_to_print[MINUTES] < 10)
  {
    minutes = "0";
    minutes += String(time_to_print[MINUTES]);
  } else
  {
    minutes += String(time_to_print[MINUTES]);
  }

  if (time_to_print[AM_PM] == 0)
  {
    am_pm = "AM";
  } else
  {
    am_pm = "PM";
  }

  display.setTextColor(time_position == 0 ? BLACK : WHITE);
  display.print(hours);
  display.display();

  display.setTextColor(WHITE);
  display.print(":");
  display.display();

  display.setTextColor(time_position == 1 ? BLACK : WHITE);
  display.print(minutes);
  display.display();

  display.setTextColor(WHITE);
  display.print(" ");
  display.display();

  display.setTextColor(time_position == 2 ? BLACK : WHITE);
  display.println(am_pm);
  display.display();

  display.setTextColor(WHITE);

}

void draw_bg(uint8_t screens)
{
  if (screens == FIRST)
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      epd_bitmap_Set_Current_Time_01, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  } else if (screens == SECOND)
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      epd_bitmap_Set_Open_Time_01, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  } else if (screens == THIRD)
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      epd_bitmap_Set_Close_Time_01, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  } else if (screens == FOURTH)
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      epd_bitmap_time_summary_01, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  } else if (screens == FIFTH)
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      epd_bitmap_Settings___Page_2_01, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  } else if ((screens >= MENU) && (screen < SUBMENU))
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      options[screens - MENU], LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  }
  else if (screens == FIRMWARE_VERSION)
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      epd_bitmap_Firmware_Version_01, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  } else if (screens == SERIAL_NUMBER)
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      epd_bitmap_Serial_Number_01, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  }
  else if ((screens == LAST_CHANCE_SUBMENU) || (screens == LAST_CHANCE_TIMER_MINS))
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      epd_bitmap_Last_Chance___1_01, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  }
  else if (screens == LAST_CHANCE_TIMER_SET_TIME)
  {
    display.drawBitmap(
      (display.width()  - LOGO_WIDTH ) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      epd_bitmap_LastChanceSetOpenTime, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
  }
}

void draw_text(uint8_t*  time_to_print)
{
  String hours, minutes, am_pm = "";
  display.setTextColor(WHITE);

  if (screen == FOURTH)
  {
    display.setFont(&FreeSans3pt7b);
    display.setCursor(30, 18);
    display.print(selected_status);

    display.setFont(&FreeSans6pt7b);
    display.setCursor(35, 30);
    display_time(time_keep[FIRST]);

    display.setFont(&FreeSans5pt7b);

    display.setCursor(60, 40);
    display_time(time_keep[SECOND]);

    display.setCursor(60, 50);
    display_time(time_keep[THIRD]);

  }

  if ((screen >= FIFTH) && (screen < MENU))
  {
    print_menu();
  }

  if (screen == FACTORY_RESET)
  {
    display.fillRect(6, 12, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();

    display.setFont(&FreeSans5pt7b);

    display.setCursor(6, 18);
    display.setTextColor(BLACK);
    display.print("Reset Device");
    display.display();

    state = OUTSIDE;
    screen = FACTORY_RESET_SUBMENU;
  }

  else if ((screen == CHANGE_STATUS) || (screen == CHANGE_STATUS_SUBMENU))
  {
    print_status_menu();
    screen = CHANGE_STATUS_SUBMENU;
    entered_scroll_page_status = true;
  }

  else if ((screen == DEVICE_INFORMATION) || (screen == DEVICE_INFORMATION_SUBMENU))
  {
    screen = DEVICE_INFORMATION_SUBMENU;
    entered_scroll_page_device = true;
    print_devce_information();
  }

  else if (screen == DOOR_SOUND)
  {
    display.fillRect(6, 12, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();

    display.setFont(&FreeSans5pt7b);

    display.setCursor(6, 18);
    display.setTextColor(BLACK);
    display.print("Song1");
    display.display();

    state = OUTSIDE;
    screen = DOOR_SOUND_SUBMENU;
  }
  else if ((screen == LAST_CHANCE) || (screen == LAST_CHANCE_SUBMENU))
  {
    screen = LAST_CHANCE_SUBMENU;
    entered_scroll_page_lastChance = true;
    print_last_chance_menu();
  }
  else if (screen == LAST_CHANCE_TIMER_MINS)
  {
    entered_scroll_page_mins = true;
    print_mins_menu();
  }

  else if (screen == CHANGE_LANGUAGE)
  {
    display.fillRect(6, 12, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();

    display.setFont(&FreeSans5pt7b);

    display.setCursor(6, 18);
    display.setTextColor(BLACK);
    display.print("English (UK)");
    display.display();
  }
  else if (screen == FIRMWARE_VERSION)
  {
    display.setFont(&FreeSans9pt7b);

    display.setCursor(10, 40);
    display.setTextColor(WHITE);
    display.print("15.4.1");
    display.display();
  }
  else if (screen == SERIAL_NUMBER)
  {
    display.setFont(&FreeSans9pt7b);

    display.setCursor(10, 40);
    display.setTextColor(WHITE);
    display.print("5055-3010a");
    display.display();
  }
  else if (screen == TEST_MODE)
  {
    display.setFont(&FreeSans5pt7b);

    display.setCursor(25, 40);
    display.setTextColor(WHITE);
    display.print("Test Mode Running");
    display.display();

    test_mode_time_tracking = millis();
  }
  else if ((screen == REBOOT) || (screen == REBOOT_SUBMENU))
  {
    screen = REBOOT_SUBMENU;
    entered_scroll_page_reboot = true;
    print_yes_no();
  }
  else if (screen == POWER_OFF || (screen == POWER_OFF_SUBMENU))
  {
    screen = POWER_OFF_SUBMENU;
    entered_scroll_page_power_off = true;
    print_yes_no();
  }

  if ((screen < 3) && (screen >= 0))
  {
    display.setFont(&FreeSans9pt7b);
    display.setCursor(25, 45);
    display_time(time_to_print);
  }
}

void print_last_chance_menu()
{
  display.fillRect(0, 12, 128, 43, BLACK);
  display.display();

  if (chance_position == 0)
  {
    display.fillRect(6, 12, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  } else if (chance_position == 1)
  {
    display.fillRect(6, 22, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  }

  display.setFont(&FreeSans5pt7b);
  display.setTextColor(WHITE);

  display.setCursor(6, 18);
  display.setTextColor(chance_position == 0 ? BLACK : WHITE);
  display.print(chance_menu[0]);
  display.display();

  display.setCursor(6, 30);
  display.setTextColor(chance_position == 1 ? BLACK : WHITE);
  display.print(chance_menu[1]);
  display.display();
}

void print_yes_no()
{
  display.fillRect(0, 22, 128, 33, BLACK);
  display.display();

  if (yes_no_position == 0)
  {
    display.fillRect(6, 22, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  } else if (yes_no_position == 1)
  {
    display.fillRect(6, 32, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  }

  display.setFont(&FreeSans5pt7b);
  display.setTextColor(WHITE);

  display.setCursor(6, 30);
  display.setTextColor(yes_no_position == 0 ? BLACK : WHITE);
  display.print(yes_no[0]);
  display.display();

  display.setCursor(6, 42);
  display.setTextColor(yes_no_position == 1 ? BLACK : WHITE);
  display.print(yes_no[1]);
  display.display();
}

void print_devce_information()
{
  display.fillRect(0, 12, 128, 43, BLACK);
  display.display();

  if (device_position == 0)
  {
    display.fillRect(6, 12, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  } else if (device_position == 1)
  {
    display.fillRect(6, 22, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  }

  display.setFont(&FreeSans5pt7b);
  display.setTextColor(WHITE);

  display.setCursor(6, 18);
  display.setTextColor(device_position == 0 ? BLACK : WHITE);
  display.print(device_menu[0]);
  display.display();

  display.setCursor(6, 30);
  display.setTextColor(device_position == 1 ? BLACK : WHITE);
  display.print(device_menu[1]);
  display.display();
}

void print_status_menu()
{
  display.fillRect(0, 12, 128, 43, BLACK);
  display.display();

  if (status_position == 0)
  {
    display.fillRect(6, 12, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  } else if (status_position == 1)
  {
    display.fillRect(6, 22, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  } else if (status_position == 2)
  {
    display.fillRect(6, 34, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  }

  display.setFont(&FreeSans5pt7b);
  display.setTextColor(WHITE);

  display.setCursor(6, 18);
  display.setTextColor(status_position == 0 ? BLACK : WHITE);
  display.print(status_menu[0]);
  display.display();

  display.setCursor(6, 30);
  display.setTextColor(status_position == 1 ? BLACK : WHITE);
  display.print(status_menu[1]);
  display.display();

  display.setCursor(6, 42);
  display.setTextColor(status_position == 2 ? BLACK : WHITE);
  display.print(status_menu[2]);
  display.display();
}

void print_mins_menu()
{
  display.fillRect(0, 12, 128, 43, BLACK);
  display.display();

  if (mins_position == 0)
  {
    display.fillRect(6, 12, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  }
  else if (mins_position == 1)
  {
    display.fillRect(6, 22, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  }
  else if (mins_position == 2)
  {
    display.fillRect(6, 34, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  }

  display.setFont(&FreeSans5pt7b);
  display.setTextColor(WHITE);

  display.setCursor(6, 18);
  display.setTextColor(mins_position == 0 ? BLACK : WHITE);
  display.print(mins_menu[0]);
  display.display();

  display.setCursor(6, 30);
  display.setTextColor(mins_position == 1 ? BLACK : WHITE);
  display.print(mins_menu[1]);
  display.display();

  display.setCursor(6, 42);
  display.setTextColor(mins_position == 2 ? BLACK : WHITE);
  display.print(mins_menu[2]);
  display.display();

}

void print_menu()
{
  display.fillRect(0, 12, 128, 43, BLACK);
  display.display();

  if ((position == 0) || (position == 4) || (position == 8))
  {
    display.fillRect(6, 12, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  } else if ((position == 1) || (position == 5) || (position == 9))
  {
    display.fillRect(6, 22, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  } else if ((position == 2) || (position == 6) || (position == 10))
  {
    display.fillRect(6, 34, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  } else if ((position == 3) || (position == 7) || (position == 11))
  {
    display.fillRect(6, 46, HIGHLIGHTER_LEN_MENU, HIGHLIGHTER_HEIGHT_MENU, WHITE);
    display.display();
  }

  display.setFont(&FreeSans5pt7b);
  display.setTextColor(WHITE);

  display.setCursor(6, 18);
  display.setTextColor(position == 0 ? BLACK : position == 4 ? BLACK : position == 8 ? BLACK : WHITE);
  display.print(menu[0 + 4 * menu_count]);
  display.display();

  display.setCursor(6, 30);
  display.setTextColor(position == 1 ? BLACK : position == 5 ? BLACK : position == 9 ? BLACK : WHITE);
  display.print(menu[1 + 4 * menu_count]);
  display.display();

  display.setCursor(6, 42);
  display.setTextColor(position == 2 ? BLACK : position == 6 ? BLACK : position == 10 ? BLACK : WHITE);
  display.print(menu[2 + 4 * menu_count]);
  display.display();

  display.setCursor(6, 54);
  display.setTextColor(position == 3 ? BLACK : position == 7 ? BLACK : position == 11 ? BLACK : WHITE);
  display.print(menu[3 + 4 * menu_count]);
  display.display();
}

uint8_t adjust_menu_index()
{
  uint8_t temp_screen = 0;

  // Correct array index if screen >= MENU
  if (screen < FOURTH)
  {
    temp_screen = screen;
  } else if (screen >= MENU)
  {
    switch (screen)
    {
      case (MENU + 2):
        temp_screen = CURRENT_TIME;
        break;

      case (MENU):
        temp_screen = OPEN_TIME;
        break;

      case (MENU + 1):
        temp_screen = CLOSE_TIME;
        break;

      case (LAST_CHANCE_TIMER_SET_TIME):
        temp_screen = OPEN_TIME_LAST_CHANCE;
        break;
    }
  }

  return temp_screen;
}

void display_time(uint8_t* time_to_print)
{
  uint8_t temp_hours;
  String hours, minutes, am_pm = "";

  if (time_to_print[AM_PM] == 1)
  {
    temp_hours = time_to_print[HOURS] - 12;
  }
  else if (time_to_print[AM_PM] == 0)
  {
    temp_hours = time_to_print[HOURS];
  }


  if (temp_hours < 10)
  {
    hours = "0";
    hours += String(temp_hours);
  } else
  {
    hours += String(temp_hours);
  }

  if (time_to_print[MINUTES] < 10)
  {
    minutes = "0";
    minutes += String(time_to_print[MINUTES]);
  } else
  {
    minutes += String(time_to_print[MINUTES]);
  }

  if (time_to_print[AM_PM] == 0)
  {
    am_pm = "AM";
  } else
  {
    am_pm = "PM";
  }

  display.print(hours); display.print(":"); display.print(minutes); display.print(" "); display.println(am_pm);
  display.display();
}

void adjust_12_hours_format(uint8_t temp_screen)
{

  if (time_keep[temp_screen][AM_PM] >= 2)
  {
    time_keep[temp_screen][AM_PM] = 0;
  }

  switch (time_keep[temp_screen][AM_PM])
  {

    // case AM
    case 0:

      if (time_keep[temp_screen][HOURS] > 12)
      {
        time_keep[temp_screen][HOURS] -= 12;
      }

      if (time_keep[temp_screen][HOURS] > 12)
      {
        time_keep[temp_screen][HOURS] = 1;
      }
      break;

    // case PM
    case 1:
      if (time_keep[temp_screen][HOURS] < 12)
      {
        time_keep[temp_screen][HOURS] += 12;
      }

      if (time_keep[temp_screen][HOURS] > 24)
      {
        time_keep[temp_screen][HOURS] = 13;
      }

      break;
  }

  if (time_keep[temp_screen][MINUTES] >= 60)
  {
    time_keep[temp_screen][MINUTES] = 0;
  }
}

void setRTCTime(uint8_t temp_screen)
{
  // Set RTC TIME
  rtc.setHours(time_keep[temp_screen][HOURS]);


  // Set RTC Minutes
  rtc.setMinutes(time_keep[temp_screen][MINUTES]);
}

void change_menu_screen()
{
  if ((position >= 0 && position < 4))
  {
    menu_count = 0;
  } else if ((position >= 4 && position < 8))
  {
    menu_count = 1;
  } else if ((position >= 8 && position < 12))
  {
    menu_count = 2;
  }
}

void NEXT_SCREEN()
{
  static uint32_t ticks = 0;

  if ((millis() - ticks) > DEBOUNCE)
  {
    if (state == ENTERED)
      time_position++;

    if ((time_position >= 3) && (screen != LAST_CHANCE_TIMER_SET_TIME))
    {
      time_position = 0;
    }

    // Print new screen, include main and menu screens
    if (((screen == FOURTH) && (!entered_scroll_page_main)) || ((screen >= FIFTH) && (screen <  POWER_OFF_SUBMENU)) || (screen >= LAST_CHANCE_TIMER))
    {

      // Main menu print
      if ((screen >= FIFTH) && (screen < SUBMENU))
      {
        display.clearDisplay();
        display.display();

        screen = position + MENU;
        print_screen(screen);
      }
      else if (((screen >= SUBMENU) && (screen < POWER_OFF_SUBMENU)) || (screen >= LAST_CHANCE_TIMER))
      {
        switch (screen)
        {
          case LAST_CHANCE_SUBMENU:
            time_position = 0;
            entered_scroll_page_lastChance = false;

            if (chance_position == 0)
            {
              display.clearDisplay();
              display.display();

              screen = LAST_CHANCE_TIMER_SET_TIME;
              print_screen(screen);
            }
            else if (chance_position == 1)
            {
              last_chance_open_state =  false;

              display.clearDisplay();
              display.display();

              screen = FIFTH;
              print_screen(screen);

            }
            break;

          case LAST_CHANCE_TIMER_SET_TIME:
            Serial.println(time_position);
            if (time_position > AM_PM)
            {
              screen = LAST_CHANCE_TIMER_MINS;
              state = OUTSIDE;

              display.clearDisplay();
              display.display();

              print_screen(screen);
            }
            break;
        }
      }
      else if (screen < FIFTH)
      {
        display.clearDisplay();
        display.display();

        print_screen(++screen);
      }

      // Set flag to indicate that scroll page has entered
      entered_scroll_page_main = (screen == FIFTH) ? true : false;

    }

    // Respond to each button click for each menu/submenu
    if ((screen < FOURTH) && (screen >= FIRST))
    {
      update_time(time_keep[screen]);
    }
    else if ((screen >= MENU) && (screen < (MENU + 3)))
    {
      switch (screen)
      {
        case (MENU + 2):
          update_time(time_keep[CURRENT_TIME]);
          break;

        case (MENU):
          update_time(time_keep[OPEN_TIME]);
          break;

        case (MENU + 1):
          update_time(time_keep[CLOSE_TIME]);
          break;
      }
    }
    else if ((screen == FACTORY_RESET_SUBMENU) && (state == ENTERED))
    {
      rtc.begin();
      NVIC_SystemReset();
    }
    else if ((screen == DOOR_SOUND_SUBMENU) && (state == ENTERED))
    {
      Serial.println("Playing Sound");
      playSample(farmSound, farmSoundSize);
    }
    else if ((screen == LAST_CHANCE_TIMER_SET_TIME) && (state == ENTERED))
    {
      update_time(time_keep[OPEN_TIME_LAST_CHANCE]);
    }
    else if ((screen == CHANGE_STATUS_SUBMENU) && (state == ENTERED))
    {
      selected_status = status_menu[status_position];
    }
    else if ((screen == DEVICE_INFORMATION_SUBMENU) && (state == ENTERED))
    {
      // Clear display contents before
      display.clearDisplay();
      display.display();

      if (device_position == 0)
      {
        screen = FIRMWARE_VERSION;
        print_screen(screen);
      }
      else if (device_position == 1)
      {
        screen = SERIAL_NUMBER;
        print_screen(screen);
      }
    }
    else if ((screen == REBOOT_SUBMENU) && (state == ENTERED))
    {
      if (yes_no_position == 0)
      {
        NVIC_SystemReset();
      }
    }
    else if ((screen == POWER_OFF_SUBMENU) && (state == ENTERED))
    {
      if (yes_no_position == 0)
      {
        display.clearDisplay();
        display.display();

        LowPower.deepSleep();
      }
    }
    else if ((screen == LAST_CHANCE_TIMER_MINS) && (state == ENTERED))
    {
      last_chance_mins = mins_position;
      last_chance_open_state = true;
      entered_scroll_page_mins = false;
      entered_scroll_page_main = true;

      display.clearDisplay();
      display.display();

      screen = FIFTH;
      print_screen(screen);
    }

    Serial.println("NEXT");

    if ((!entered_scroll_page_main))
    {
      state = ENTERED;
    }

    ticks = millis();
  }
}

void BACK_SCREEN()
{
  static uint32_t ticks = 0;

  if ((millis() - ticks) > DEBOUNCE)
  {

    time_position = 0;
    Serial.println("BACK");


    if ((screen >= FIFTH) && (screen < MENU))
    {
      display.clearDisplay();
      display.display();

      screen = FOURTH;
      entered_scroll_page_main = false;
      print_screen(screen);
    }
    else if ((screen >= MENU) && (screen < SUBMENU))
    {
      display.clearDisplay();
      display.display();

      screen = FIFTH;
      entered_scroll_page_main = true;
      print_screen(screen);
    }
    else if (screen == FACTORY_RESET_SUBMENU)
    {
      display.clearDisplay();
      display.display();

      screen = FIFTH;
      entered_scroll_page_main = true;
      print_screen(screen);
    }
    else if (screen == CHANGE_STATUS_SUBMENU)
    {
      display.clearDisplay();
      display.display();

      screen = FIFTH;
      entered_scroll_page_status = false;
      entered_scroll_page_main = true;
      print_screen(screen);
    }
    else if (screen == DEVICE_INFORMATION_SUBMENU)
    {
      display.clearDisplay();
      display.display();

      screen = FIFTH;
      entered_scroll_page_device = false;
      entered_scroll_page_main = true;
      print_screen(screen);
    }
    else if (screen == DOOR_SOUND_SUBMENU)
    {
      display.clearDisplay();
      display.display();

      screen = FIFTH;
      entered_scroll_page_main = true;
      print_screen(screen);
    }
    else if (screen == LAST_CHANCE_SUBMENU)
    {
      display.clearDisplay();
      display.display();

      screen = FIFTH;
      entered_scroll_page_lastChance = false;
      entered_scroll_page_main = true;
      print_screen(screen);
    }
    else if (screen ==  LAST_CHANCE_TIMER_SET_TIME)
    {
      display.clearDisplay();
      display.display();

      screen = LAST_CHANCE_SUBMENU;
      entered_scroll_page_lastChance = true;
      print_screen(screen);
    } else if (screen ==  LAST_CHANCE_TIMER_MINS)
    {
      display.clearDisplay();
      display.display();

      screen = LAST_CHANCE_TIMER_SET_TIME;
      entered_scroll_page_mins = false;
      print_screen(screen);
      update_time(time_keep[OPEN_TIME_LAST_CHANCE]);
    }
    else if (screen == REBOOT_SUBMENU)
    {
      yes_no_position = 0;
      display.clearDisplay();
      display.display();

      screen = FIFTH;
      entered_scroll_page_reboot = false;
      entered_scroll_page_main = true;
      print_screen(screen);
    }
    else if (screen == POWER_OFF_SUBMENU)
    {
      yes_no_position = 0;
      display.clearDisplay();
      display.display();

      screen = FIFTH;
      entered_scroll_page_power_off = false;
      entered_scroll_page_main = true;
      print_screen(screen);
    }
    else if (screen == FIRMWARE_VERSION)
    {
      display.clearDisplay();
      display.display();

      screen = DEVICE_INFORMATION;
      entered_scroll_page_device = true;
      entered_scroll_page_main = false;
      print_screen(screen);
    }
    else if (screen == SERIAL_NUMBER)
    {
      display.clearDisplay();
      display.display();

      screen = DEVICE_INFORMATION;
      entered_scroll_page_device = true;
      entered_scroll_page_main = false;
      print_screen(screen);
    }

    if (!entered_scroll_page_device && !entered_scroll_page_lastChance)
    {
      state = OUTSIDE;
    }

    ticks = millis();

  }
}

void UP_SCREEN()
{
  static uint32_t ticks = 0;
  uint8_t temp_screen = 0;

  if ((millis() - ticks) > DEBOUNCE)
  {

    // Correct array index if screen >= MENU
    temp_screen = adjust_menu_index();

    if (!entered_scroll_page_main && !entered_scroll_page_status && !entered_scroll_page_device && !entered_scroll_page_lastChance && !entered_scroll_page_reboot && !entered_scroll_page_power_off && !entered_scroll_page_mins)
    {
      if (state == ENTERED)
      {
        time_keep[temp_screen][time_position]++;

        adjust_12_hours_format(temp_screen);

        // set Current RTC Time
        if (screen == CURRENT_TIME)
        {
          setRTCTime(temp_screen);
        }

      } else if (state == OUTSIDE)
      {
        screen++;

        if (screen > FOURTH)screen = FOURTH;
      }
    }

    else if (entered_scroll_page_main)
    {
      screen++;
      position++;

      // Display new options if selector crossed first/last option of page
      change_menu_screen();

      if (screen > EIGHTH)screen = EIGHTH;
      if (position >= 12)position = 11;

    }
    else if (entered_scroll_page_status)
    {
      status_position++;
      if (status_position >= 3) status_position = 2;
    }
    else if (entered_scroll_page_device)
    {
      device_position++;
      if (device_position >= 2) device_position = 1;
    }
    else if (entered_scroll_page_lastChance)
    {
      chance_position++;
      if (chance_position >= 2) chance_position = 1;
    }
    else if (entered_scroll_page_reboot)
    {
      yes_no_position++;
      if (yes_no_position >= 2) yes_no_position = 1;
    }
    else if (entered_scroll_page_power_off)
    {
      yes_no_position++;
      if (yes_no_position >= 2) yes_no_position = 1;
    } else if (entered_scroll_page_mins)
    {
      mins_position++;
      if (mins_position >= 3) mins_position = 2;
    }

    // Indicate pressing of up/down button
    switched = true;


    // Used for button debounce
    ticks = millis();
  }
}

void DOWN_SCREEN()
{
  uint8_t temp_screen = 0;
  static uint32_t ticks = 0;

  if ((millis() - ticks) > DEBOUNCE)
  {

    // Correct array index if screen >= MENU
    temp_screen = adjust_menu_index();

    if (!entered_scroll_page_main && !entered_scroll_page_status && !entered_scroll_page_device && !entered_scroll_page_lastChance && !entered_scroll_page_reboot && !entered_scroll_page_power_off && !entered_scroll_page_mins)
    {
      if (state == ENTERED)
      {
        time_keep[temp_screen][time_position]--;

        adjust_12_hours_format(temp_screen);

        // set Current RTC Time
        if (screen == CURRENT_TIME)
        {
          setRTCTime(temp_screen);
        }

      } else if (state == OUTSIDE)
      {
        screen--;

        if (screen > FOURTH)screen = FIRST;
      }
    }

    else if (entered_scroll_page_main)
    {
      screen--;

      position--;

      change_menu_screen();

      if (screen < FIFTH)screen = FIFTH;

      if (position >= 12)position = 0;
    }
    else if (entered_scroll_page_status)
    {
      status_position--;
      if (status_position >= 3) status_position = 0;
    }
    else if (entered_scroll_page_device)
    {
      device_position--;
      if (device_position >= 2) device_position = 0;
    }
    else if (entered_scroll_page_lastChance)
    {
      chance_position--;
      if (chance_position >= 2) chance_position = 0;
    }
    else if (entered_scroll_page_reboot)
    {
      yes_no_position--;
      if (yes_no_position >= 2) yes_no_position = 0;
    }
    else if (entered_scroll_page_power_off)
    {
      yes_no_position--;
      if (yes_no_position >= 2) yes_no_position = 0;
    } else if (entered_scroll_page_mins)
    {
      mins_position--;
      if (mins_position >= 3) mins_position = 0;
    }

    // Indicate pressign of up/down button
    switched = true;

    // Used for button debounce
    ticks = millis();
  }
}

void welcome_screen()
{
  display.clearDisplay();
  display.display();

  display.drawBitmap(
    (display.width()  - 125 ) / 2,
    (display.height() - 100) / 2,
    epd_bitmap_welcome_cropped, 128, 64, 1);
  display.display();
}
