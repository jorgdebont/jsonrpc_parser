#ifndef SRC_JSON_RPC_H_
#define SRC_JSON_RPC_H_

#include "jsmn.h"
#include "util.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef USING_CONTIKI
MEMB(tuples, struct tuple, 16);
#include "lib/memb.h"
#define USING_PRINTF
#endif // USING_CONTIKI

#define ERROR_METHOD

#define JSON_RPC_NOT_ASSIGNED 	0
#define JSON_RPC_CALL 			1
#define JSON_RPC_RESPONSE 		2

typedef struct
{
    char *method;
    char *params;
} call_t;

typedef struct
{
    char *result;
} response_t;

struct tuple
{
    uint8_t a;
    uint16_t id;
    response_t response;
    call_t call;
};
uint8_t jsoneq(const char *json, jsmntok_t *tok, const char *s);
void json_rpc_handler(call_t *json_call);
void encode_json_rpc(struct tuple *json_tuple, char *jsonString);
void response_to_string(response_t *json_response, char *json_string);
void call_to_string(call_t *json_call, char *json_string);
void decode_json_rpc(char *json_string, struct tuple *tup);
void get_array_from_tuple(struct tuple *json_tuple, char output_array[][50],uint8_t amount_of_parameters);
void call_to_string_V2(call_t *json_call, char *json_string, char *params);

#endif /* SRC_JSON_RPC_H_ */
