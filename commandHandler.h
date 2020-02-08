/*
commandHandler.cpp

Данный файл определяет неблокирующую функцию processCommandsAndOutput(),
которую следует вызывать циклически.

Буферы приема команд и печати сообщений:
Буферы приема и передачи имеют размер 256 бейт. Для инициализации буферов
следует вызвать следующую функцию:
initCommandHandler()

Прием команд:
Команды принимаются через последовательный порт (Serial), разделенные
переносом строки и/или точкой с запятой. Каждая команда состоит из набора
аргументов, разделенных произвольным количеством пробелов. Имя команды
являяется нулевым аргументом, следующие аргументы команды нумеруются
начиная с первого. Перед началом приема первых команд необходимо указать
функцию-обработчик команд с помощью следующей функции:
void setCommandHandler(void (*func)(int argc, char** argv))

Вывод в последовательный порт:
Отправка теста, завершаемого переносом строки, осуществляется функцией:
void Msg(const char* format, ...)
В функцию Msg можно передать либо только строку, либо строку с данными
для форматирования.

Отправка команд:
Команды могут быть приняты не только через последовательный порт, но
и вызовом следующей функции:
void Command(const char* cmd, ...)
В функцию Command можно передать либо только строку, либо строку с
данными для форматирования.
*/

#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_

#include <Arduino.h>

#include <stdio.h>

#include "multitasking.h"

#define COMMAND_BUFFER_SIZE 256
#define OUTPUT_BUFFER_SIZE 256
#define MAX_ARGS 16

void initCommandHandler();

void Msg(const char* format, ...);

void setCommandHandler(void (*func)(int argc, char** argv));

void parseCommandsFromBuffer();

void Command(const char* cmd, ...);

void processCommandsAndOutput();

void sendAllBlocking();

#endif
