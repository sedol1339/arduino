#include "multitasking.h"
#include "commandHandler.h"
#include "LinkedList.h"

/*
Move 100 clockwise
Move 100 counter
Reset
Exit [message]
Hold
Release
Wait 100
Then [command]
SetInterrupt 3 off
SetInterrupt 3 rising [cmd(s)]
Speed 0.5
Cmove coef -2.3
Cmove 4
Alias cmd [cmd(s)]
AttachInterrupt 3 high
Listen 3 rising 10 cmd
Pause
Unpause
Pause; wait 100; then unpause
*/

int PUL = 10; //define Pulse pin
int DIR = 6; //define Direction pin
int ENA = 5; //define Enable Pin

int COUNTER_CLOCKWISE = LOW;
int CLOCKWISE = HIGH;

unsigned long finishTime = 0;
float tickIntervalMsec = 1;
float turnSpeed = 1; //turns per second
float ticks_per_turn = 400;

unsigned long turns_to_msec(float turns) {
  float ticks = turns * ticks_per_turn;
  return (unsigned long)(ticks * tickIntervalMsec);
}

int last_tick_value = LOW;
void engineTick() {
  if (millis() - finishTime < 1000000) { //probably it's correct
    remove_loop();
    digitalWrite(ENA, HIGH);
  }
  last_tick_value = (last_tick_value == HIGH) ? LOW : HIGH;
  digitalWrite(PUL, last_tick_value);
}

void setDirection(int dir) {
  digitalWrite(DIR, dir);
}

void startEngine() {
  digitalWrite(ENA, LOW);
  register_loop("engine", tickIntervalMsec, engineTick);
}

void interruptEngine() {
  remove_loop("engine");
  digitalWrite(ENA, HIGH);
}

char* to_lower(char* str) {
  for (int i = 0; i < strlen(str); i++) {
    str[i] = tolower(str[i]);
  }
  return str;
}

bool starts_with(const char* str, const char* substr) {
  return strstr(str, substr) == str;
}

void runCommand(int argc, char** argv) {
  char* cmd = argv[0];
  to_lower(cmd);
  if (!strcmp(cmd, "move")) {
    if (argc == 3) {
      float turns = atof(argv[1]);
      int turn_direction;
      to_lower(argv[2]);
      if (starts_with("clockwise", argv[2])) {
        turn_direction = CLOCKWISE;
      } else if (starts_with("counter", argv[2])) {
        turn_direction = COUNTER_CLOCKWISE;
      } else {
        Msg("Wrong direction argument");
        return;
      }
      finishTime = millis() + turns_to_msec(turns);
      setDirection(turn_direction);
      startEngine();
    } else {
      Msg("Wrong syntax");
    }
  } else {
    Msg("Unknown command");
  }
}

void setup() {
  initCommandHandler();
  register_loop("cmds", 0, processCommandsAndOutput);
  setCommandHandler(runCommand);
  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);
  Serial.begin(9600);
  while (!Serial) {} // wait for serial port to connect
  Msg("Serial port available");
}
