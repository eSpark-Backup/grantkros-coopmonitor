#define OPEN_TIME 0
#define CLOSE_TIME 1
#define CURRENT_TIME 2
#define OPEN_TIME_LAST_CHANCE 3

#define HOURS 0
#define SECONDS 1
#define AM_PM 2

#define TIME_DIMENSIONS 3
#define TIME_TYPES 4

#define NEXT 7
#define BACK 6
#define UP 5
#define DOWN 3

#define DEBOUNCE 100

#define Serial SerialUSB

uint8_t time_keep[TIME_TYPES][TIME_DIMENSIONS] = {0};
uint8_t time_position = 0;
uint8_t screen = 0;
uint32_t tick = 0;

void NEXT_SCREEN()
{

  time_position++;

  Serial.println("NEXT");
}

void BACK_SCREEN()
{

  time_position--;

  Serial.println("BACK");
}

void UP_SCREEN()
{
  //  uint32_t tick = millis();
  //  while ((millis() - tick) < DEBOUNCE);
  //
  //  time_keep[screen][time_position]++;
  //
  //  if (time_keep[screen][time_position] >= 60)
  //  {
  //    time_keep[screen][time_position] = 0;
  //  }

  Serial.println("UP");
}

void DOWN_SCREEN()
{
  //  uint32_t tick = millis();
  //  while ((millis() - tick) < DEBOUNCE);
  //
  //  time_keep[screen][time_position]--;
  //
  //  if (time_keep[screen][time_position] < 0)
  //  {
  //    time_keep[screen][time_position] = 0;
  //  }


  Serial.println("DOWN");
}

void setup() {
  // put your setup code here, to run once:
  pinMode(NEXT, INPUT_PULLUP);
  pinMode(BACK, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);

  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(NEXT), NEXT_SCREEN, FALLING);
  attachInterrupt(digitalPinToInterrupt(BACK), BACK_SCREEN, FALLING);
  attachInterrupt(digitalPinToInterrupt(UP), UP_SCREEN, FALLING);
  attachInterrupt(digitalPinToInterrupt(DOWN), DOWN_SCREEN, FALLING);

  tick = millis();

}

void loop() {
  // put your main code here, to run repeatedly:

  if (time_position >= 3)
  {
    time_position = 0;
    screen++;
  }

  if (time_position < 0)
  {
    time_position = 0;
    screen--;
  }

  if ((screen >= TIME_TYPES) || (screen < 0))
  {
    screen = 0;
  }
}
