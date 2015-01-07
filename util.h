/*
 * util.h
 *
 *  Created on: Dec 18, 2014
 *      Author: tackticaldude
 */

#define TRUE 	1
#define FALSE 	0

#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <string.h>
#include <stdio.h>

char* custom_itoa(int, char[]);
int custom_atoi( char *);
int compare_string(char *, char *);
void custom_strcat(char *,  char *);

#endif /* SRC_UTIL_H_ */
