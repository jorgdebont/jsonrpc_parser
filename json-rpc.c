/*
 * json-rpc.c
 *
 *  Created on: Dec 15, 2014
 *      Author: tackticaldude, jazula
 */

#include "json-rpc.h"




uint8_t jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (uint8_t) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}


void json_rpc_handler(call_t* json_call)
{

}

void encode_json_rpc(struct tuple *json_tuple, char *json_string)
{
	char temp[7];
    strcpy(json_string,"\{\"jsonrpc\": \"2.0\", ");
    switch(json_tuple->a){
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
	custom_itoa(json_tuple->id, temp);
    strcat(json_string, " , \"id\": \"");
    strcat(json_string, temp);
    strcat(json_string, "\"}");
    return;
}

void response_to_string(response_t* json_response, char* json_string)
{
#ifdef _DEBUG
    printf("response_to_string(): encoding json_string");
#endif // _DEBUG
    strcat(json_string, "\"result\": ");
    strcat(json_string, json_response->result);
    return;
}

void call_to_string(call_t *json_call, char *json_string)
{
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
    if(str_token != NULL)
    {
        strcat(json_string, "\"");
        strcat(json_string, str_token);
        strcat(json_string, "\"");
        str_token = strtok(NULL, ",");
    }
    while(str_token != NULL)
    {
        strcat(json_string, ", ");
        strcat(json_string, "\"");
        strcat(json_string, str_token);
        strcat(json_string, "\"");
        str_token = strtok(NULL, ",");
    }
    strcat(json_string, "]");
    return;
}

struct tuple *decode_json_rpc_(char *json_string)
{
#ifdef _DEBUG
    printf("decode_json_rpc(): Message: Decoding json string\n\r");
#endif // _DEBUG

    uint16_t	    length, index;
    struct tuple	*tup;
    jsmn_parser		temp_jsmn_parser;
    jsmntok_t		jsmn_tokens[50];

    tup = malloc(sizeof(struct tuple));
    if(tup != NULL)
    {
        tup->a = JSON_RPC_NOT_ASSIGNED;
        jsmn_init(&temp_jsmn_parser);
        length = jsmn_parse(&temp_jsmn_parser, json_string, strlen(json_string), jsmn_tokens, sizeof(jsmn_tokens)/sizeof(jsmn_tokens[0]));

        for(index = 0; index < length; index++)
        {
            if(jsoneq(json_string, &jsmn_tokens[index], "jsonrpc") == 0)
            {
                index++;
            }
            else if(jsoneq(json_string, &jsmn_tokens[index], "method") == 0)
            {
                tup->a = JSON_RPC_CALL;
                strncpy(tup->call.method, json_string+jsmn_tokens[index+1].start, (jsmn_tokens[index+1].end - jsmn_tokens[index+1].start));
                index++;
            }
            else if(jsoneq(json_string, &jsmn_tokens[index], "result") == 0)
            {
                tup->a = JSON_RPC_RESPONSE;
                strncpy(tup->response.result, json_string+jsmn_tokens[index+1].start, (jsmn_tokens[index+1].end - jsmn_tokens[index+1].start));
                index++;
            }
            else if(jsoneq(json_string, &jsmn_tokens[index], "params") == 0)
            {
                if(tup->a==JSON_RPC_NOT_ASSIGNED)
                {
                    printf("decode_json_rpc(): Error: JSON_RPC is not assigned \n\r");
                }
                else if(tup->a==JSON_RPC_RESPONSE)
                {
                    printf("decode_json_rpc(): Error: JSON_RPC is a response, therefore it can't have parameters \n\r");
                }
                else if(tup->a==JSON_RPC_CALL)
                {
                    strncpy(tup->call.params, json_string+jsmn_tokens[index+1].start, (jsmn_tokens[index+1].end - jsmn_tokens[index+1].start));
                }
                index++;
            }
            else if(jsoneq(json_string, &jsmn_tokens[index], "id") == 0)
            {
                if(tup->a==JSON_RPC_NOT_ASSIGNED)
                {
                    printf("decode_json_rpc(): Error: JSON_RPC is not assigned \n\r");
                }
                else if(tup->a==JSON_RPC_CALL)
                {
                    tup->id= atoi(json_string+jsmn_tokens[index+1].start);
                }
                else if(tup->a==JSON_RPC_RESPONSE)
                {
                    tup->id = atoi(json_string+jsmn_tokens[index+1].start);
                }
                index++;
            }
        }
    }
    else
    {
        printf("decode_json_rpc(): Error: failed to allocate mem\n\r");
    }
    return tup;
}
