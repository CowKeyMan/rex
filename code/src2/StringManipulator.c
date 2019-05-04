#include <string.h>
#include <stdbool.h>

#include "StringManipulator.h"

void splitStringBy(char *delims, char* splitString, char** returnArgs, int bufferAmount) {
  char *token;
  token = strtok(delims, splitString);

  int tokenIndex = 0;
  while (token != NULL && tokenIndex < bufferAmount) {
    returnArgs[tokenIndex] = token;
    token = strtok(NULL, splitString);
    tokenIndex++;
  }

  //Set last returnArgs as NULL
  returnArgs[tokenIndex] = NULL;
}

void splitStringByString(char *string, char* splitString, char** returnArgs, int bufferAmount, int bufferSize) {
  //Copy string into a buffer
  char stringBuf[bufferSize];
  strncpy(stringBuf, string, bufferSize);

  //Create a bufer for the returnArgs
  char returnArgsBuf[bufferAmount][bufferSize];
  returnArgsBuf[0][0] = '\0';

  int currentString = 0; //The current string being added to
  int currentStringIndex = 0; //The position in the string

  //current char is the current character in the string being split
  for(int currentChar = 0; currentChar < strlen(stringBuf); currentChar++, currentStringIndex++){
    //Add current character to the argument
    returnArgsBuf[currentString][currentStringIndex] = stringBuf[currentChar];

    //go through the string from the current position, until the string does not match the parsing string
    for(int i2 = 0; i2+currentChar < strlen(stringBuf) && stringBuf[i2+currentChar] == splitString[i2]; i2++){
      //If the string is found, then split it at this point
      if(i2 == strlen(splitString)-1){
        if(returnArgsBuf[currentString][0] != '\0'){
          returnArgsBuf[currentString][currentStringIndex] = '\0';
          currentString++;
          returnArgsBuf[currentString][0] = '\0';
          currentStringIndex = -1;
        }
        currentChar+=i2;
        continue;
      }
    }
  }

  returnArgsBuf[currentString][currentStringIndex] = '\0';
  //Set final arg as null
  returnArgsBuf[currentString+1][0] = '\0';



  for(int i = 0; returnArgsBuf[i][0] != '\0'; i++){
    removeSpaces(returnArgsBuf[i], bufferSize);
    returnArgs[i] = returnArgsBuf[i];
  }
}

void removeSpaces(char* string, int bufferSize){
  char buf[bufferSize];

  //Traverse the string, ignoring spaces in the beginning
  int counter = 0;
  int j = 0;
  while(string[j] == ' '){
    j++;
  }

  while (j < strlen(string) && j<bufferSize) {
    buf[counter] = string[j];
    counter++;
    j++;
  }

  //Then set spaces at the end of the string as '\0'
  buf[counter] = '\0';
  for (int j2 = counter-1; j2 >= 0; j2--) {
    if(buf[j2] == ' '){
      buf[j2] = '\0';
    }else{
      break;
    }
  }
  //At the end, change the string to the buffer
  strncpy(string, buf, bufferSize);
}

bool isEmptyString(char* string, int bufferSize){
  char _line[bufferSize];
  strncpy(_line, string, bufferSize);
  removeSpaces(_line, bufferSize);
  
  return  strlen(_line) <= 0;
}

void shiftStrings(char **args){
  int i = 0;
  while(args[i] != NULL){
    args[i] = args[i+1];
    i++;
  }
}

void concatenteStrings(char **strings, char *buffer, int bufferSize){
  strncpy(buffer, strings[0], bufferSize);
  for (int i = 1; strings[i] != NULL; ++i) {
    strncat(buffer, " ", bufferSize);
    strncat(buffer, strings[i], bufferSize);
  }
}