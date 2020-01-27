int PUL=10; //define Pulse pin
int DIR=6; //define Direction pin
int ENA=5; //define Enable Pin

int COUNTER_CLOCKWISE = LOW;
int CLOCKWISE = HIGH;

bool interrupted = false;

void step(int microdelay, int microsteps, int direction) {
  digitalWrite(DIR,direction);
  for (int i=0; i<microsteps; i++) {
    if (interrupted) return;
    digitalWrite(PUL,HIGH);
    delayMicroseconds(microdelay);
    digitalWrite(PUL,LOW);
    delayMicroseconds(microdelay);
  }
}

void interrupt() {
  interrupted = true;
}

void setup() {
  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);
  attachInterrupt(1, interrupt, RISING); //1 means port 3
  //Serial.begin(9600);
  digitalWrite(ENA,LOW);
  for (int i = 0; i < 15; i++) {
    step(600, 100, CLOCKWISE);
    if (interrupted) return;
    delay(200);
    if (interrupted) return;
  }
  digitalWrite(ENA,HIGH);
}

void loop() {
  
}
