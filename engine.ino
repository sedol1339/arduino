/*
   ВЗАИМОДЕЙСТВИЕ С ПЛАТОЙ
   Плата презназначена для управления двигателем и подключается к драйверу двигателя
   DC:9-42VDC и компьютеру. Драйвер двигателя имеет выводы PUL (+/-), DIR (+/-),
   ENA (+/-). Подключение выводов платы:
   9 -> PUL+
   7 -> DIR+
   5 -> ENA+
   GND -> PUL-, DIR-, ENA-
   2 -> прерывание (опционально)
   USB -> компьютер (питание, ввод/вывод)

   Плата принимает команды через последовательный порт, работающий на скорости 9600 бод.
   Вывод платы передается в этот же порт. Большая часть строк вывода предназначена для
   чтения человеком, однако предусмотрены также строки, получателем которых является
   управляющая программа (начинаются с "!").

   ОТПРАВКА КОМАНД
   Каждая принимаемая платой команда представляет собой последовательность слов или чисел,
   разделенных пробелами. Первое слово - команда, следующие слова или числа - аргументы.
   Символы точка с запятой, перенос строки (\n), возврат каретки (\r) интерпретируются
   как разделители команд. Команда, поступающая через порт, не будет выполнена, пока не
   будет передан любой из символов-разделителей команд. До, после команды или между ее
   аргументами допустимо произвольное количество пробелов. Между командами допустимы
   множественные разделители (например, ;\n). Регистр (размер буковок) не имеет значения.

   БУФЕРЫ ВВОДА И ВЫВОДА
   Буфер приема команда имеет размер 256 байт, при превышении работа платы будет завершена
   с сообщением об ошибке. Однако учитывая, что команды обрабатываются с большей скоростью,
   чем символы принимаются через порт, можно пересылать последовательности команд
   произвольной длины. Буфер вывода имеет размер 256 байт, при превышении плата осуществит
   попытку вывести предупреждение, однако это не завершит работу платы. Если некая программная
   ошибка или некорректная команда приведет к тому, что плата будет записывать что-либо в буфер
   вывода быстрее, чем данные пересылаются через порт, пользователь вероятно увидит в окне
   приема данных из порта множество символов W (начало слова Warning).

   КОМАНДA MOVE
   Синтаксис: move <turns> <direction>
   Осуществляет вращение двигателя. Параметры: turns - количество полных оборотов, direction
   - направление, "clockwise", "counter" или начало одного из этих слов. Пример:
   move 10 cl;

   КОМАНДА SPEED
   Синтаксис: speed <turns_per_sec>
   Устанавливает скорость вращения двигателя в оборотах в секунду. Значения около 5 и выше
   приводят к некорректной работе двигателя. Также у используемого на данный момент двигателя
   Handpose 17HS4401 (или у дайвера) имеется погрешность: при указанном соотношении 400 тактов
   на оборот реальное значение несколько больше (около 405) даже при низкой скорости (тактов
   в секунду). Эта потрешность возможно зависит от скорости и на данный момент не учитывается
   в программе, потому что неизвестно, какая конфигурация будет использоваться на практике.
   Поэтому при команде на 1 полный оборот реальный угол поворота будет немного меньше. То же
   замечание относится и к команде pos, о которой ниже. Также нужно отметить, что текущая
   версия программы не поддерживает изменение скорости в процессе выполнения команды move.

   КОМАНДЫ HOLD и WAIT
   Синтаксис: hold <msec>, wait <msec>
   Обе команды означают ожидание в течение заданного количества миллисекунд, при этом команда
   hold ждет в заблокированном состоянии двигателя (не вращается вручную), команда wait ждет
   в разблокированном состоянии. Команду wait не имеет смысла использовать без команды
   then, о которой далее.

   КОМАНДА THEN
   Синтаксис: then <cmd> [args]
   Позволяет выстраивать очередь из команд. Следующая команда в очереди будет выполнена тогда,
   когда закончит выполняться предыдущая команда. Например, чтобы повернуть двигатель на один
   оборот по часовой стрелке, подождать 1 секунду и повернуть обратно, нужно отправить такую
   последовательность команд:
   move 1 cl; then hold 1000; then move 1 co;
   Технически возможно начать первую команду с "then", результат будет идентичным. Очередь
   команд не ограничивается в размерах и при переполнении памяти может вызвать сбой.
   

   ПРОГРАММНО СЧИТЫВАЕМЫЕ КОМАНДЫ
   При начале работы с двигателем (move, hold, wait) плата пересылает в порт строку !START X,
   где X - оставшееся время до конца выполнения данной команды в миллисекундах. При переходе
   на следующую команду в очереди плата пересылает строку !NEXT. При завершении очереди и
   переходе в состояние простоя и ожидания новых команд плата пересылает строку !STOP.

   СБРОС И ВЫХОД
   Прекращение выполнения текущей команды и очистка очереди команд осуществляется командой
   reset. Выход из программы (остановка работы платы до перезагрузки) осуществляется командой
   exit. При выходе или сбросе двигатель всегда разблокируется. Также выход может быть
   осуществлен с помощью прерывания подачей сигнала +5V на цифровой порт 2, в этом случае
   в выводе порта не появится сообщения о выходе (защита от программных сбоев).

   КОМАНДА POS
   Синтаксис: pos init <start_pos> <delta_per_turn>; pos <position>
   Данная команда предлагается в качестве замены команды move в том случае, когда двигатель
   используется для позиционирования объекта. При инициализация (pos init) задается стартовая
   позиция объекта на шкале и изменение позиции на шкале в пересчете на один оборот двигателя
   (числа с плавающей запятой). После инициализации при командах move и pos будет расчитываться
   координата по шкале с учетом скорости, времени и направления работы двигателя. Команда pos
   позволяет перейти на заданную позицию на шкале. Она будет корректно работать после
   инициализации и произвольного количества команд pos, move, hold, wait.
*/

#include "multitasking.h"
#include "commandHandler.h"
#include "LinkedList.h"

int PUL = 9; //define Pulse pin
int DIR = 7; //define Direction pin
int ENA = 5; //define Enable Pin

int COUNTER_CLOCKWISE = LOW;
int CLOCKWISE = HIGH;

const int MODE_MOVE = 0;
const int MODE_HOLD = 1;
const int MODE_WAIT = 2;

bool isRunningTask = false;
bool pendingNext = false;
int mode = MODE_MOVE;
int dir = COUNTER_CLOCKWISE;
unsigned long finishTime = 0;
unsigned long nextRunTime = 0; //used only for com port output
float tickIntervalMsec = 1;
float ticks_per_turn = 400;

bool engine_pos_init = false;
float engine_pos = 0;
float engine_delta_per_tick = 0;

void calcTimeAndDirection(float desired_pos) {
  float delta_pos = desired_pos - engine_pos;
  if (delta_pos > 0) {
    dir = CLOCKWISE;
  } else {
    dir = COUNTER_CLOCKWISE;
    delta_pos = -delta_pos;
  }
  float required_ticks = delta_pos / engine_delta_per_tick;
  unsigned long required_msec = (unsigned long)(required_ticks * tickIntervalMsec);
  nextRunTime = required_msec;
  finishTime = millis() + required_msec;
}

unsigned long turns_to_msec(float turns) {
  float ticks = turns * ticks_per_turn;
  return (unsigned long)(ticks * tickIntervalMsec);
}

void setEngineSpeed(float turns_per_sec) {
  float ticks_per_sec = ticks_per_turn * turns_per_sec;
  tickIntervalMsec = 1000 / ticks_per_sec;
  Msg("setEngineSpeed(): speed = %f", turns_per_sec);
}

LinkedList<char*> commandQueue;

void addToQueue(char* cmd) {
  //int size = strlen(cmd) + 1;
  //char* copy = new char[size];
  //memcpy(copy, cmd, size);
  Msg("Add to queue: %s", cmd);
  commandQueue.add(cmd);
}

void clearQueue() {
  while (commandQueue.size() > 0) {
    char* nextCmd = commandQueue.pop();
    delete[] nextCmd;
  }
}

void next() {
  pendingNext = false;
  //takes command from queue
  int size = commandQueue.size();
  if (size == 0) {
    Msg("Command queue finished");
    Msg("!STOP");
  } else {
    char* nextCmd = commandQueue.remove(0);
    Msg("next(): sending command \"%s\", %d commands in queue", nextCmd, size - 1);
    Msg("!NEXT");
    //debugPrintLoops();
    Command(nextCmd);
    delete[] nextCmd;
  }
}

int first_run = true;
int last_tick_value = LOW;
unsigned long last_millis = 0;
void engineTick() {
  if (first_run) {
    Msg("engineTick(): first run");
    first_run = false;
  }
  if (millis() != last_millis && millis() % 100 == 0) Msg("engineTick(): millis = %u", millis());
  if (millis() - finishTime < 1000000) { //probably it's correct
    Msg("engineTick(): finishing");
    remove_loop("engine");
    digitalWrite(ENA, HIGH);
    isRunningTask = false;
    last_millis = millis();
    delayed_call(0, next);
    return;
  }
  switch (mode) {
    case MODE_MOVE:
      digitalWrite(ENA, LOW);
      digitalWrite(DIR, dir);
      last_tick_value = (last_tick_value == HIGH) ? LOW : HIGH;
      digitalWrite(PUL, last_tick_value);
      if (engine_pos_init) engine_pos += engine_delta_per_tick * ((dir == CLOCKWISE) ? 1.0 : -1.0);
      break;
    case MODE_HOLD:
      digitalWrite(ENA, LOW);
      break;
    case MODE_WAIT:
      digitalWrite(ENA, HIGH);
      break;
  }
  last_millis = millis();
}

void startEngine() {
  /*const char* mode_str = "none";
    switch (mode) {
    case MODE_MOVE: mode_str = "MODE_MOVE"; break;
    case MODE_HOLD: mode_str = "MODE_HOLD"; break;
    case MODE_WAIT: mode_str = "MODE_WAIT"; break;
    }*/
  Msg("startEngine(): starting, finish time %u", finishTime);
  Msg("!START %u", nextRunTime);
  first_run = true;
  isRunningTask = true;
  register_loop("engine", tickIntervalMsec, engineTick);
}

void interruptEngine() {
  remove_loop("engine");
  isRunningTask = false;
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

void reset() {
  Msg("reset");
  interruptEngine();
  clearQueue();
}

void runCommand(int argc, char** argv) {
  char* cmd = argv[0];
  to_lower(cmd);
  //Msg("Running command %s", cmd);
  if (!strcmp(cmd, "move")) {
    if (argc == 3) {
      float turns = atof(argv[1]);
      to_lower(argv[2]);
      if (starts_with("clockwise", argv[2])) {
        dir = CLOCKWISE;
      } else if (starts_with("counter", argv[2])) {
        dir = COUNTER_CLOCKWISE;
      } else {
        Msg("Wrong direction argument");
        return;
      }
      int required_time = turns_to_msec(turns);
      nextRunTime = required_time;
      finishTime = millis() + required_time;
      mode = MODE_MOVE;
      startEngine();
    } else {
      Msg("Wrong syntax, arg count is %d", argc);
    }
  } else if (!strcmp(cmd, "reset")) {
    reset();
  } else if (!strcmp(cmd, "then")) {
    int totalStrLen = 0;
    for (int i = 1; i < argc; i++) totalStrLen += strlen(argv[i]);
    totalStrLen += argc - 2; //whitespaces
    char* cmd2 = new char[totalStrLen + 1];
    cmd2[0] = 0; //end of string
    for (int i = 1; i < argc; i++) {
      strcat(cmd2, argv[i]);
      if (i != argc - 1) strcat(cmd2, " ");
    }
    addToQueue(cmd2);
    //can't add to buffer now, "then" command is still in buffer
    if (!isRunningTask && !pendingNext) {
      pendingNext = true;
      delayed_call(0, next);
    }
  } else if (!strcmp(cmd, "exit")) {
    handle_exit("Exit");
  } else if (!strcmp(cmd, "hold")) {
    if (argc == 2) {
      unsigned long msec = atoi(argv[1]);
      nextRunTime = msec;
      finishTime = millis() + msec;
      mode = MODE_HOLD;
      startEngine();
    } else {
      Msg("Wrong syntax, arg count is %d", argc);
    }
  } else if (!strcmp(cmd, "wait")) {
    if (argc == 2) {
      unsigned long msec = atoi(argv[1]);
      nextRunTime = msec;
      finishTime = millis() + msec;
      mode = MODE_WAIT;
      startEngine();
    } else {
      Msg("Wrong syntax, arg count is %d", argc);
    }
  } else if (!strcmp(cmd, "speed")) {
    if (argc == 2) {
      float turns_per_sec = atof(argv[1]);
      setEngineSpeed(turns_per_sec);
    } else {
      Msg("Wrong syntax, arg count is %d", argc);
    }
  }  else if (!strcmp(cmd, "pos")) {
    if (argc == 2) {
      //positioning
      if (!strcmp(argv[1], "init")) {
        Msg("Use \"pos init X D\"");
      } else if (engine_pos_init) {
        calcTimeAndDirection(atof(argv[1]));
        mode = MODE_MOVE;
        startEngine();
      } else {
        Msg("Use \"pos init X D\" first");
      }
    } else if (argc == 4) {
      //initialization
      if (!strcmp(argv[1], "init")) {
        engine_pos = atof(argv[2]);
        engine_delta_per_tick = atof(argv[3]) / ticks_per_turn; //delta per turn
        engine_pos_init = true;
        Msg("Engine pos control initialized");
      } else {
        Msg("Wrong syntax, use pos X or pos init X D");
      }
    } else {
      Msg("Wrong syntax, arg count is %d", argc);
    }
  } else {
    Msg("Unknown command \"%s\"", cmd);
  }
}

void engine_on_exit() {
  digitalWrite(ENA, HIGH);
}

void safeExit() {
  engine_on_exit();
  while (1);
}

void setup() {
  attachInterrupt(0, safeExit, RISING);
  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);
  digitalWrite(ENA, HIGH);
  on_exit(engine_on_exit);
  Serial.begin(9600);
  while (!Serial) {} // wait for serial port to connect
  initCommandHandler();
  setCommandHandler(runCommand);
  commandQueue = LinkedList<char*>();
  register_loop("cmds", 0, processCommandsAndOutput);
  Msg("Serial port available");
}
