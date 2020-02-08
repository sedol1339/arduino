#include "commandHandler.h"

char* commandBuffer;
char* outputBuffer;

void (*_cmdHandler)(int argc, char** argv) = NULL;

void initCommandHandler() {
  commandBuffer = new char[COMMAND_BUFFER_SIZE];
  outputBuffer = new char[OUTPUT_BUFFER_SIZE];
  commandBuffer[0] = 0; //end of string
  outputBuffer[0] = 0; //end of string
}

void Msg(const char* format, ...) {
  va_list argptr; //macro for varargs from stdio
  va_start(argptr, format);
  int prevBufferLen = strlen(outputBuffer);
  //writing bytes to outputBuffer (then \n)
  vsnprintf(outputBuffer + prevBufferLen, OUTPUT_BUFFER_SIZE - prevBufferLen - 1, format, argptr);
  va_end(argptr);
  strcat(outputBuffer, "\n");
  int nextBufferLen = strlen(outputBuffer);
  if (nextBufferLen == OUTPUT_BUFFER_SIZE - 1) {
    const char* errMsg = "\nWARNING: Output buffer overflow\n";
    int errMsgChars = strlen(errMsg) + 1; //including \0
    strncpy(outputBuffer + OUTPUT_BUFFER_SIZE - errMsgChars - 1, errMsg, errMsgChars);
  }
}

void setCommandHandler(void (*func)(int argc, char** argv)) {
  _cmdHandler = func;
}

void parseCommandsFromBuffer() {
  while (true) {
    //get next command and args from command buffer
    int cmdBufferLen = strlen(commandBuffer);
    char* linebreakPtr = strchr(commandBuffer, '\n');
    if (!linebreakPtr) break; //no command yet, still reveiving chars
    int linebreakIndex = (int)(linebreakPtr - commandBuffer);
    //executing command
    if (linebreakIndex != 0) {
      //there is some command before linebreak
      char* args[MAX_ARGS];
      int arg_index = 0; //index to write next argument (current args count)
      *linebreakPtr = 0; //replacing \n by \0
      for (int i = 0; i < linebreakIndex; i++) {
        //iterating over all command chars
        //we suppose that args may be divided by more than one whitespace
        char currentChar = commandBuffer[i];
        char prevChar = (i > 0) ? commandBuffer[i - 1] : ' ';
        if (currentChar != ' ' && (prevChar == ' ' || prevChar == 0)) {
          //currentChar is the first char of а new argument
          if (arg_index < MAX_ARGS) {
            args[arg_index++] = commandBuffer + i;
          }
        } else if (currentChar == ' ' && prevChar != ' ' && prevChar != 0) {
          //prevChar was the last char of the argument
          //replacing current char by \0, so we generated zero-terminated string
          commandBuffer[i] = 0;
        }
      }
      //args parsing finished, processing command
      if (_cmdHandler) {
        _cmdHandler(arg_index, args);
      } else {
        handle_exit("EXIT: no command handler");
        return;
      }
    }
    //removing parsed command from buffer
    for (int i = linebreakIndex + 1; i <= cmdBufferLen; i++) {
      commandBuffer[i - linebreakIndex - 1] = commandBuffer[i];
    }
  }
}

void Command(const char* cmd, ...) {
  va_list argptr; //macro for varargs from stdio
  va_start(argptr, cmd);
  int prevBufferLen = strlen(commandBuffer);
  //writing bytes to commandBuffer (then \n)
  vsnprintf(commandBuffer + prevBufferLen, COMMAND_BUFFER_SIZE - prevBufferLen - 1, cmd, argptr);
  va_end(argptr);
  strcat(commandBuffer, "\n");
  int nextBufferLen = strlen(commandBuffer);
  if (nextBufferLen == COMMAND_BUFFER_SIZE - 1) {
    handle_exit("EXIT: command buffer overflow");
  }
  parseCommandsFromBuffer();
}

void sendAllBlocking() {
  Serial.write(outputBuffer, strlen(outputBuffer));
}

void processCommandsAndOutput() {
  //writing from output buffer to serial
  int maxNonBlockingBytes = Serial.availableForWrite();
  int bufferLen = strlen(outputBuffer);
  if (maxNonBlockingBytes >= bufferLen) {
    //enough place in serial buffer
    Serial.write(outputBuffer, bufferLen);
    outputBuffer[0] = 0; //end of string
  } else {
    //not enough place in serial buffer
    Serial.write(outputBuffer, maxNonBlockingBytes);
    //removing written bytes from output buffer
    for (int i = maxNonBlockingBytes; i <= bufferLen; i++) {
      outputBuffer[i - maxNonBlockingBytes] = outputBuffer[i];
    }
  }
  //reading from serial port
  int bytes = Serial.available();
  if (!bytes) return;
  int prevBufferLen = strlen(commandBuffer);
  int nextBufferLen = prevBufferLen + bytes;
  if (nextBufferLen > COMMAND_BUFFER_SIZE - 1) {
    handle_exit("EXIT: command buffer overflow");
    return;
  }
  Serial.readBytes(commandBuffer + prevBufferLen, bytes);
  commandBuffer[nextBufferLen] = 0; //appending \0
  //replacing semicolons with line breaks
  for (int i = prevBufferLen; i < nextBufferLen; i++) {
    if (commandBuffer[i] == ';') commandBuffer[i] = '\n';
    if (commandBuffer[i] == '\r') commandBuffer[i] = '\n';
  }
  //parsing command buffer
  parseCommandsFromBuffer();
}
