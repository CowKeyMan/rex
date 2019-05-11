#include <stdbool.h>

//Changes the contents of returnArgs to the string split by the delimeters delims
//Return args is an array of <bufferAmount> strings
void splitStringBy(char *delims, char* splitString, char** returnArgs, int bufferAmount);

//Parser with an entire string as delimeter
void splitStringByString(char *string, char* splitString, char** returnArgs, int bufferAmount, int bufferSize);

//Method to remove spaces from the beginning and end of a string
void removeSpaces(char* string, int bufferSize);

// check if string is just spaces
bool isEmptyString(char* string, int bufferSize);

// shift an array of strings by 1 to the left
void shiftStrings(char **args);

// concatinate the array of strings to one string separated by spaces
void concatenteStrings(char **strings, char *buffer, int bufferSize);
