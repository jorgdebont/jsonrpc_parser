#include "json-rpc.h"

LIST( function_list);
MEMB(function_mem, function_t, 16);

void function_list_init() {
	memb_init(&function_mem);
	list_init(function_list);
}

int add_function(char function_name[], void (*fp)) {
	function_t *func = memb_alloc(&function_mem);
	if (func) {
		return -1;
	}
	strcpy(func->function_name, function_name);
	func->function = fp;

	list_add(function_list, func);
	return 1;
}

int delete_function(void (*fp)) {
	function_t *f;
	for (f = list_head(function_list); f != NULL; f = list_item_next(f)) {
		if (f->function == fp) {
			list_remove(function_list, f);
			memb_free(&function_mem, f);
			return 1;
		}
	}
	return -1;
}

int method_caller(char method_name[], char params[]) {
	function_t *f;
	for (f = list_head(function_list); f != NULL; f = list_item_next(f)) {
		if (compare_string(method_name, f->function_name)) {
			void (*function)(char[]);
			function = f->function;
			function(params);
			return 1;
		}
	}
	return -1;
}

uint8_t jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (uint8_t) strlen(s) == tok->end - tok->start
			&& strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void json_rpc_handler(call_t* json_call) {

}

void encode_json_rpc(struct tuple *json_tuple, char *json_string, char *ip) {
	char buff[150];
	strcpy(json_string, "\{\"jsonrpc\": \"2.0\", ");
	switch (json_tuple->a) {
	case JSON_RPC_NOT_ASSIGNED:
		printf("encode_json_rpc(): Error: no response or call");
		break;
	case JSON_RPC_CALL:
		call_to_string(&json_tuple->call, json_string);
		break;
	case JSON_RPC_RESPONSE:
		response_to_string(&json_tuple->response, json_string);
		break;
	}
	strcat(json_string, " , \"id\": ");
	sprintf(buff, "\"%d\" , \"ip\" : \"%s\"}", json_tuple->id, ip);
	strcat(json_string, buff);
}

void response_to_string(response_t* json_response, char* json_string) {
#ifdef _DEBUG
	printf("response_to_string(): encoding json_string");
#endif // _DEBUG
	strcat(json_string, "\"result\": ");
	strcat(json_string, json_response->result);
	return;
}

void call_to_string(call_t *json_call, char *json_string) {
	char *str_token;
	char temp_string[256];
#ifdef _DEBUG
	printf("call_to_string(): encoding json_string");
	printf("call_to_string(): method:%s, params:%s, id:%d \n\r", json_call->method, json_call->params, json_call->id);
#endif // _DEBUG
	strcat(json_string, "\"method\": ");
	strcat(json_string, "\"");
	strcat(json_string, json_call->method);
	strcat(json_string, "\", ");

	strcat(json_string, "\"params\": [");
	strcpy(temp_string, json_call->params);
	str_token = strtok(temp_string, ",");
	if (str_token != NULL) {
		strcat(json_string, "\"");
		strcat(json_string, str_token);
		strcat(json_string, "\"");
		str_token = strtok(NULL, ",");
	}
	while (str_token != NULL) {
		strcat(json_string, ", ");
		strcat(json_string, "\"");
		strcat(json_string, str_token);
		strcat(json_string, "\"");
		str_token = strtok(NULL, ",");
	}
	strcat(json_string, "]");
	return;
}

void decode_json_rpc(char *json_string, struct tuple *tup) {
#ifdef _DEBUG
#ifdef USING_PRINTF
	printf("decode_json_rpc(): Message: Decoding json string\n\r");
#endif // USING_PRINTF
#endif // _DEBUG

	uint16_t length, index;
	jsmn_parser temp_jsmn_parser;
	jsmntok_t jsmn_tokens[50];
	char temp_string[100];
	char *str_token;

	tup->a = JSON_RPC_NOT_ASSIGNED;
	jsmn_init(&temp_jsmn_parser);
	length = jsmn_parse(&temp_jsmn_parser, json_string, strlen(json_string),
			jsmn_tokens, sizeof(jsmn_tokens) / sizeof(jsmn_tokens[0]));

	for (index = 0; index < length; index++) {
		if (jsoneq(json_string, &jsmn_tokens[index], "jsonrpc") == 0) {
			index++;
		} else if (jsoneq(json_string, &jsmn_tokens[index], "method") == 0) {
			tup->a = JSON_RPC_CALL;
			strncpy(tup->call.method,
					json_string + jsmn_tokens[index + 1].start,
					(jsmn_tokens[index + 1].end - jsmn_tokens[index + 1].start));
			index++;
		} else if (jsoneq(json_string, &jsmn_tokens[index], "result") == 0) {
			tup->a = JSON_RPC_RESPONSE;
			strncpy(tup->response.result,
					json_string + jsmn_tokens[index + 1].start,
					(jsmn_tokens[index + 1].end - jsmn_tokens[index + 1].start));
			index++;
		} else if (jsoneq(json_string, &jsmn_tokens[index], "params") == 0) {
			if (tup->a == JSON_RPC_NOT_ASSIGNED) {
#ifdef USING_PRINTF
				printf("decode_json_rpc(): Error: JSON_RPC is not assigned \n\r");
#else
				//ERROR_METHOD
#endif // USING_PRINTF
			} else if (tup->a == JSON_RPC_RESPONSE) {
#ifdef USING_PRINTF
				printf("decode_json_rpc(): Error: JSON_RPC is a response, therefore it can't have parameters \n\r");
#else
				//ERROR_METHOD
#endif // USING_PRINTF
			} else if (tup->a == JSON_RPC_CALL) {
				strncpy(temp_string, json_string + jsmn_tokens[index + 1].start,
						(jsmn_tokens[index + 1].end
								- jsmn_tokens[index + 1].start));
				str_token = strtok(temp_string, "[\"] ");
				while (str_token != NULL) {
					strcat(tup->call.params, str_token);
					str_token = strtok(NULL, "[\"] ");
				}
			}
			index++;
		} else if (jsoneq(json_string, &jsmn_tokens[index], "id") == 0) {
			if (tup->a == JSON_RPC_NOT_ASSIGNED) {
#ifdef USING_PRINTF
				printf("decode_json_rpc(): Error: JSON_RPC is not assigned \n\r");
#else
				//ERROR_METHOD
#endif // USING_PRINTF
			} else if ((tup->a == JSON_RPC_CALL)
					|| (tup->a == JSON_RPC_RESPONSE)) {
				tup->id = atoi(json_string + jsmn_tokens[index + 1].start);
			}
			index++;
		} else if (jsoneq(json_string, &jsmn_tokens[index], "ip") == 0) {
			strncpy(tup->ip, json_string + jsmn_tokens[index + 1].start,
					(jsmn_tokens[index + 1].end - jsmn_tokens[index + 1].start));
			index++;
		}

	}
	return;
}

void get_ip_addr_from_json(char *ipaddr, char *json_string) {
	uint16_t length, index;
	jsmn_parser temp_jsmn_parser;
	jsmntok_t jsmn_tokens[50];

	jsmn_init(&temp_jsmn_parser);
	length = jsmn_parse(&temp_jsmn_parser, json_string, strlen(json_string),
			jsmn_tokens, sizeof(jsmn_tokens) / sizeof(jsmn_tokens[0]));

	for (index = 0; index < length; index++) {
		if (jsoneq(json_string, &jsmn_tokens[index], "ip") == 0) {
			strncpy(ipaddr, json_string + jsmn_tokens[index + 1].start,
					(jsmn_tokens[index + 1].end - jsmn_tokens[index + 1].start));
			index++;
		}
	}
}

void get_id_from_json(char *id, char *json_string) {
	uint16_t length, index;
	jsmn_parser temp_jsmn_parser;
	jsmntok_t jsmn_tokens[50];

	jsmn_init(&temp_jsmn_parser);
	length = jsmn_parse(&temp_jsmn_parser, json_string, strlen(json_string),
			jsmn_tokens, sizeof(jsmn_tokens) / sizeof(jsmn_tokens[0]));

	for (index = 0; index < length; index++) {
		if (jsoneq(json_string, &jsmn_tokens[index], "id") == 0) {
			strncpy(id, json_string + jsmn_tokens[index + 1].start,
					(jsmn_tokens[index + 1].end - jsmn_tokens[index + 1].start));
			index++;
		}
	}
}

void get_tag_value_from_jason(char *value, char *tag, char *json_string) {
	uint16_t length, index;
	jsmn_parser temp_jsmn_parser;
	jsmntok_t jsmn_tokens[50];

	jsmn_init(&temp_jsmn_parser);
	length = jsmn_parse(&temp_jsmn_parser, json_string, strlen(json_string),
			jsmn_tokens, sizeof(jsmn_tokens) / sizeof(jsmn_tokens[0]));

	for (index = 0; index < length; index++) {
		if (jsoneq(json_string, &jsmn_tokens[index], tag) == 0) {
			strncpy(value, json_string + jsmn_tokens[index + 1].start,
					(jsmn_tokens[index + 1].end - jsmn_tokens[index + 1].start));
			index++;
		}
	}
}
