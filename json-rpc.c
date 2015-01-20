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
    char buffer[50];
    strcpy(json_string,"\{\"jsonrpc\": \"2.0\", ");
    switch(json_tuple->a)
    {
    case JSON_RPC_NOT_ASSIGNED:
#ifdef USING_PRINTF
        printf("encode_json_rpc(): Error: no response or call\n\r");
#else
        ERROR_METHOD
#endif // USING_PRINTF
        break;
    case JSON_RPC_CALL:
        call_to_string(&json_tuple->call, json_string);
        break;
    case JSON_RPC_RESPONSE:
        response_to_string(&json_tuple->response, json_string);
        break;
    }
    sprintf(buffer, " , \"id\": \"%d\"}",json_tuple->id);
    strcat(json_string, buffer);
    return;
}

void response_to_string(response_t* json_response, char* json_string)
{
#ifdef _DEBUG
#ifdef USING_PRINTF
    printf("response_to_string(): encoding json_string\n\r");
#endif // USING_PRINTF
#endif // _DEBUG
	char buffer[100];
	sprintf(buffer, "\"result\": \"%s\"", json_response->result);
    strcat(json_string, buffer);
    return;
}

void call_to_string(call_t *json_call, char *json_string)
{
#ifdef _DEBUG
#ifdef USING_PRINTF
    printf("call_to_string(): encoding json_string\n\r");
    printf("call_to_string(): method:%s, params:%s, id:%d \n\r", json_call->method, json_call->params, json_call->id);
#endif // USING_PRINTF
#endif // _DEBUG
    char *str_token = malloc(sizeof(char)*50);
    if(str_token == NULL)
    {
#ifdef USING_PRINTF
        printf("call_to_string(): Error: Failed to allocate memory");
#else
        ERROR_METHOD
#endif // USING_PRINTF
		return;
    }
    char temp_string[256];
    char buffer[100];
    sprintf(buffer,"\"method\": \"%s\", \"params\": [", json_call->method);
    strcat(json_string, buffer);
    strcpy(temp_string, json_call->params);
    str_token = strtok(temp_string, ", ");
    if(str_token != NULL)
    {
        sprintf(buffer, "\"%s\"", str_token);
        strcat(json_string, buffer);
        str_token = strtok(NULL, ", ");
    }
    while(str_token != NULL)
    {
        sprintf(buffer, ", \"%s\"", str_token);
        strcat(json_string, buffer);
        str_token = strtok(NULL, ", ");
    }
    strcat(json_string, "]");
    free(str_token);
    return;
}

void decode_json_rpc(char *json_string, struct tuple *tup)
{
#ifdef _DEBUG
#ifdef USING_PRINTF
    printf("decode_json_rpc(): Message: Decoding json string\n\r");
#endif // USING_PRINTF
#endif // _DEBUG

    uint16_t	    length, index;
    jsmn_parser		temp_jsmn_parser;
    jsmntok_t		jsmn_tokens[50];
    uint8_t			jsonrpc_found = FALSE;
    char			*temp_string = malloc(sizeof(char)*256);
    char			*str_token = malloc(sizeof(char)*50);

	if((str_token == NULL) || (temp_string == NULL))
    {
#ifdef USING_PRINTF
        printf("decode_json_rpc(): Error: Failed to allocate memory");
#else
        ERROR_METHOD
#endif // USING_PRINTF
		if(str_token != NULL) 			free(str_token);
		else if(temp_string != NULL) 	free(temp_string);
		return;
    }
    if(tup->a == JSON_RPC_CALL) strcpy(tup->call.params, "");

    tup->a = JSON_RPC_NOT_ASSIGNED;
    jsmn_init(&temp_jsmn_parser);
    length = jsmn_parse(&temp_jsmn_parser, json_string, strlen(json_string), jsmn_tokens, sizeof(jsmn_tokens)/sizeof(jsmn_tokens[0]));

    for(index = 0; index < length; index++)
    {
        if((jsonrpc_found == FALSE) &&(jsoneq(json_string, &jsmn_tokens[index], "jsonrpc") == 0))
        {
            jsonrpc_found = TRUE;
            index++;
        }
        else if(tup->a == JSON_RPC_NOT_ASSIGNED)
        {
            if(jsoneq(json_string, &jsmn_tokens[index], "method") == 0)
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
        }
        else if(jsoneq(json_string, &jsmn_tokens[index], "params") == 0)
        {
            if(tup->a==JSON_RPC_CALL)
            {
                strncpy(temp_string, json_string+jsmn_tokens[index+1].start, (jsmn_tokens[index+1].end - jsmn_tokens[index+1].start));
                str_token = strtok(temp_string, "[\"] ");
                while(str_token != NULL)
                {
                    strcat(tup->call.params, str_token);
                    str_token = strtok(NULL, "[\"] ");
                }
            }
            else if(tup->a==JSON_RPC_NOT_ASSIGNED)
            {
#ifdef USING_PRINTF
                printf("decode_json_rpc(): Error: JSON_RPC is not assigned \n\r");
#else
                ERROR_METHOD
#endif // USING_PRINTF
            }
            else if(tup->a==JSON_RPC_RESPONSE)
            {
#ifdef USING_PRINTF
                printf("decode_json_rpc(): Error: JSON_RPC is a response, therefore it can't have parameters \n\r");
#else
                ERROR_METHOD
#endif // USING_PRINTF
            }
            index++;
        }
        else if(jsoneq(json_string, &jsmn_tokens[index], "id") == 0)
        {
            if(tup->a==JSON_RPC_NOT_ASSIGNED)
            {
#ifdef USING_PRINTF
                printf("decode_json_rpc(): Error: JSON_RPC is not assigned \n\r");
#else
                ERROR_METHOD
#endif // USING_PRINTF
            }
            else if((tup->a==JSON_RPC_CALL) ||(tup->a==JSON_RPC_RESPONSE))
            {
                tup->id= atoi(json_string+jsmn_tokens[index+1].start);
            }
            index++;
        }
    }
    free(str_token);
    free(temp_string);
    return;
}

void get_array_from_tuple(struct tuple *json_tuple, char output_array[][50],uint8_t amount_of_parameters)
{
#ifdef _DEBUG
#ifdef USING_PRINTF
    printf("get_array_from_tuple(): Message: getting array from json_tuple params\n\r");
#endif // USING_PRINTF
#endif // _DEBUG
    uint8_t index;
    char *str_token = malloc(sizeof(char)*50);
    char *temp_result = malloc(sizeof(char)*256);
    if((str_token == NULL) || (temp_result == NULL))
    {
#ifdef USING_PRINTF
        printf("get_array_from_tuple(): Error: Failed to allocate memory");
#else
        ERROR_METHOD
#endif // USING_PRINTF
		if(str_token != NULL) 			free(str_token);
		else if(temp_result != NULL) 	free(temp_result);
		return;
    }
    strcpy(temp_result, json_tuple->response.result);
    str_token = strtok(temp_result, "[\"] ");
    if(str_token == NULL)
    {
#ifdef USING_PRINTF
        printf("get_array_from_tuple(): Error: amount_of_parameters not right");
#else
        ERROR_METHOD
#endif // USING_PRINTF
		free(temp_result);
		free(str_token);
        return;
    }
    for(index = 0; index < amount_of_parameters; index++)
    {
        strncpy(output_array[index], str_token, 50);
        printf("output token: %s", output_array[index]);
        str_token = strtok(NULL, "[\"] ");
        if(str_token == NULL)
        {
#ifdef USING_PRINTF
            printf("get_array_from_tuple(): Error: amount_of_parameters not right");
#else
            ERROR_METHOD
#endif // USING_PRINTF
            break;
        }
    }
    free(temp_result);
    free(str_token);
}

char *strtok_two(char *s, const char *delim)
{
    static char *last;

    return strtok_r(s, delim, &last);
}

void call_to_string_V2(call_t *json_call, char *json_string, char *params)
{
#ifdef _DEBUG
#ifdef USING_PRINTF
    printf("call_to_string_V2(): encoding json_string\n\r");
    printf("call_to_string_V2(): method:%s, params:%s, id:%d \n\r", json_call->method, json_call->params, json_call->id);
#endif // USING_PRINTF
#endif // _DEBUG
    char *str_token;
    char *temp_param = malloc(sizeof(char)*strlen(params));
    char *param_token;
    if(temp_param == NULL)
    {
#ifdef USING_PRINTF
        printf("call_to_string_V2(): Error: Failed to allocate memory");
#else
        ERROR_METHOD
#endif // USING_PRINTF
		return;
    }
    char temp_string[256];
    char buffer[100];
    strcpy(temp_param, params);
	param_token = strtok_two(temp_param, "%, ");

    sprintf(buffer,"\"method\": \"%s\", \"params\": [", json_call->method);
    strcat(json_string, buffer);
    strcpy(temp_string, json_call->params);
    str_token = strtok(temp_string, ", ");

    if(str_token != NULL)
    {
		if(strcmp(param_token, "s") == 0)
		{
			sprintf(buffer, "\"%s\"", str_token);
		}
		else
		{
			sprintf(buffer, "%s", str_token);
		}
        strcat(json_string, buffer);
        str_token = strtok(NULL, ", ");
        param_token = strtok_two(NULL, "%, ");
    }
    while(str_token != NULL)
    {
        sprintf(buffer, ", \"%s\"", str_token);
        strcat(json_string, buffer);
        str_token = strtok(NULL, ", ");
        param_token = strtok_two(NULL, "%, ");
    }
    strcat(json_string, "]");

    free(temp_param);
    return;
}













