#ifndef SRC_JSON_RPC_H_
#define SRC_JSON_RPC_H_

#include "jsmn.h"
#include "util.h"
#include "lib/list.h"
#include "lib/memb.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define JSON_RPC_NOT_ASSIGNED 	0
#define JSON_RPC_CALL 			1
#define JSON_RPC_RESPONSE 		2
static char ipaddress[128];

typedef struct {
	char function_name[32];
	void (*function);
} function_t;

typedef struct {
	char *method;
	char *params;
	char *params_format;
} call_t;

typedef struct {
	char *result;
} response_t;

struct tuple {
	uint8_t a;
	uint16_t id;
	response_t response;
	call_t call;
	char ip[128];
};

void function_list_init();
int add_function(char function_name[], void (*fp));
int delete_function(void (*fp));
int method_caller(char method_name[], char params[]);
uint8_t jsoneq(const char *json, jsmntok_t *tok, const char *s);
void json_rpc_handler(call_t *json_call);
void encode_json_rpc(struct tuple *json_tuple, char *jsonString, char*);
void response_to_string(response_t *json_response, char *json_string);
void call_to_string(call_t *json_call, char *json_string);
void decode_json_rpc(char *json_string, struct tuple *tup);
void get_array_from_tuple(struct tuple *json_tuple, char output_array[][50],
		uint8_t amount_of_parameters);
char *strtok_two(char *s, const char *delim);
void get_tag_value_from_jason(char *, char *, char *);

#endif /* SRC_JSON_RPC_H_ */
