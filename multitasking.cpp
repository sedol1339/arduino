#include "multitasking.h"

String exit_reason = "";
bool exited = false;
void (*run_on_exit)() = NULL;

Loop *loops[MAX_LOOPS + 1]; //ends with null

int current_index;

void on_exit(void (*func)()) {
  run_on_exit = func;
}

void loop() {
  if (exited) return;
  unsigned long time = micros();
  for(current_index = 0 ;; current_index++) {
    Loop *loop = loops[current_index];
    if (loop == NULL) break;
    //https://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover
    if (loop->last_called + loop->call_interval < time) {
      loop->last_called = time;
      loop->func();
      if (loop->run_once) remove_loop(); //removes loop with current index
    }
  }
}

void clear_loops() {
  for(int i = 0 ;; i++) {
    Loop *loop = loops[i];
    if (loop == NULL) break;
    loop = NULL;
  }
}

void remove_loop_internal(int index) {
  if (&loops[index] == NULL) return;
  delete loops[index];
  for(int i = index ;; i++) {
    Loop *tmp = loops[index + 1];
    loops[index] = tmp;
    if (tmp == NULL) break;
  }
}

void remove_loop() {
  remove_loop_internal(current_index);
}

void remove_loop(String name) {
  for(int i = 0 ;; i++) {
    Loop *loop = loops[i];
    if (loop == NULL) break;
    if (name.equals(*(loop->name))) {
      remove_loop_internal(i);
      break;
    }
  }
}

void remove_loop(const char* name) {
  remove_loop(String(name));
}

void handle_exit(const char* reason) {
  if (exited) return;
  exit_reason = reason;
  if (Serial) Serial.write(reason);
  clear_loops();
  if (run_on_exit != NULL) run_on_exit();
  exited = true;
}

void register_loop_internal(String *name, unsigned long interval, void (*func)()) {
  int index = 0;
  //looking for first null loop
  for( ;; index++) {
    Loop *loop = loops[index];
    if (loop == NULL) break; //adding new loop
    if (loop->name != NULL && loop->name->equals(*name)) break; //overwriting
  }
  //checking index
  if (index == MAX_LOOPS) {
    handle_exit("Exceeding max number of loops");
    return;
  }
  //writing new loop data
  Loop *loop = new Loop;
  loops[index] = loop;
  loop->name = name;
  loop->func = func;
  loop->last_called = micros();
  loop->call_interval = interval;
  loop->run_once = false;
}

void register_loop_internal(unsigned long interval, void (*func)()) {
  register_loop_internal(NULL, interval, func);
}

void register_loop(String name, float millisec, void (*func)()) {
  unsigned long microsec = millisec * 1000;
  register_loop_internal(&name, microsec, func);
}

void register_loop(float millisec, void (*func)()) {
  unsigned long microsec = millisec * 1000;
  register_loop_internal(microsec, func);
}

void delayed_call_internal(unsigned long interval, void (*func)()) {
  int index = 0;
  //looking for first null loop
  for( ; loops[index] != NULL; index++);
  //checking index
  if (index == MAX_LOOPS) {
    handle_exit("Exceeding max number of loops");
    return;
  }
  //writing new loop data
  Loop *loop = new Loop;
  loops[index] = loop;
  loop->name = NULL;
  loop->func = func;
  loop->last_called = (interval < 4) ? micros() - 4 : micros();
  loop->call_interval = interval;
  loop->run_once = true;
}

void delayed_call(float millisec, void (*func)()) {
  unsigned long microsec = millisec * 1000;
  delayed_call_internal(microsec, func);
}

//test

/*int test_i = 1;
void test() {
  test_i++;
  bool on = test_i % 2 == 0;
  digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
}

void stop() {
  //remove_loop("test");
  register_loop("test", 900, test);
  digitalWrite(LED_BUILTIN, LOW);
}
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  register_loop("test", 19.9, test);
  delayed_call(2000, stop);
}*/