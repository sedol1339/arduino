/*
multitasking.cpp

Данный файл определяет функцию void loop(), которая
будет вызываться из Arduiuno автоматически, и предоставляет
возможность управления параллельными циклами с помощью
следующих функций:
void register_loop(String name, float millisec, void (*func)())
  Регистрация нового цикла, который затем может быть удален
  или перезаписан используя его имя. На Arduino Uno (16 MHz)
  минимальное значение millisec равно 0.004 (4 микросекунды),
  меньшие значения будут аналогичны 0.004. Функция цикла func
  будет вызвана первый раз не сразу, а через millisec миллисекунд.
  Всего может быть не больше 32 циклов + отложенных заданий.
void register_loop(float millisec, void (*func)())
  Аналогичная функция без использования имени. В таком случае
  данный цикл не может быть перезаписан или удален, кроме как
  использованием функции remove_loop() изнутри функции func.
void delayed_call(float millisec, void (*func)())
  Регистрация отложенного вызова функции func, который будет
  выполнен один раз через указанное время.  Минимальное время
  задержки равно 0, в таком случае вызов будет выполнен на
  следующей итерации основного цикла loop. Отложенные вызовы
  нельзя перезаписывать или удалять. Всего может быть не
  больше 32 циклов + отложенных заданий.
void clear_loops()
  Удаление всех зарегистрированных циклов. Гарантируется, что
  после выполнения этой функции ни один из прежде
  зарегистрированных циклов не будет вызван, если только функция
  clear_loops вызвана не из прерывания. В противном случае
  существует небольшая вероятность выполнения одного из прежде
  зарегистрированных циклов один раз.
void remove_loop()
  Удаление текущего цикла. Данная функция может быть вызвана только
  изнутри одного из работающих циклов. Вызов функции remove_loop()
  из отложенного вызова влечет неопределенное поведение.
void remove_loop(String name)
  Удаление цикла по его имени.
void remove_loop(const char* name)
  Удаление цикла по его имени.
void handle_exit(String reason)
  Завершение всех циклов без возможности запуска новых циклов с
  печатью сообщения в com-порт. Функция handle_exit вызывается
  либо вручную, либо при превышении максимального количества
  одновременных циклов + отложенных вызовов (32).
void on_exit(void (*func)())
  Регистрация функции, которая будет вызывана при вызове функции
  handle_exit. Такая функция может быть только одна, при повторной
  регистрации функция будет перезаписана.
*/

#ifndef MULTITASKING_H_
#define MULTITASKING_H_

#include <Arduino.h>

#include "commandHandler.h"

#define MAX_LOOPS 32

struct Loop {
  const char* name;
  void (*func)(); //loop function
  unsigned long last_called;
  unsigned long call_interval;
  bool run_once;
};

void on_exit(void (*func)());

void loop();

void clear_loops();

void remove_loop_internal(int index);

void remove_loop();

void remove_loop(const char* name);

void remove_loop(const char* name);

void handle_exit(const char* reason);

void register_loop_internal(const char* name, unsigned long interval, void (*func)());

void register_loop_internal(unsigned long interval, void (*func)());

void register_loop(const char* name, float millisec, void (*func)());

void register_loop(float millisec, void (*func)());

void delayed_call_internal(unsigned long interval, void (*func)());

void delayed_call(float millisec, void (*func)());

void debugPrintLoops();

#endif
