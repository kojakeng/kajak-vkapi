#include <stddef.h>             /* size_t */
#include <stdlib.h>							/* malloc realloc */
#include <stdio.h>
#include <string.h>             /* strcpy strcat strlen */

#include "vkapi_alloc.h"
#include "vkapi_config.h"
#include "vkapi_utils.h"


char*
vkapi_create_mention(char *user_id, char *user_name)
{
	char *user_mention;
	char *user_outer_id;

	user_outer_id         = vkapi_get_outer_id(user_id);
	user_mention = vkapi_emalloc(sizeof(char) * (
	                             strlen(user_id) +
	                             strlen(user_name) + 4));

	strcpy(user_mention, "[");
	strcat(user_mention, user_outer_id);
	strcat(user_mention, "|");
	strcat(user_mention, user_name);
	strcat(user_mention, "]");

	return user_mention;
}

char *
vkapi_gen_request(struct vkapi_sess_obj *sess_obj,
                  struct vkapi_opts     *opts)
{
	/* iteration variables declaration */
	int i;

	/* temporary variables declaration */
	char *opt_name;
	char *opt_value;

	/* normal variables declaration */
	char *request_url;
	char *method_str;
	char *token;

	method_str = vkapi_get_method_str(opts->method_type);
	if (method_str == NULL) {
		fprintf(stderr, "FATAL ERROR vkapi_utils.c:");
		fprintf(stderr, "can't get method string in vkapi_gen_request");
		exit(EXIT_FAILURE);
	}

	/* VK_API_URL + method_str + '/' + '?' + '\0' */
	request_url = vkapi_emalloc(sizeof(char) * 
	                            (strlen(VK_API_URL) +
															 strlen(method_str) + 4));

	strcpy(request_url, VK_API_URL);
	strcat(request_url, "/");
	strcat(request_url, method_str);

	for (i = 0; i < opts->num; ++i) {
		opt_name = opts->lst[i]->opt_name;
		opt_value = opts->lst[i]->opt_value;

		request_url = vkapi_erealloc(request_url, sizeof(char) *
																							(strlen(request_url) +
																							 strlen(opt_name) +
																							 strlen(opt_value) + 3));
		if (i == 0) {
			strcat(request_url, "?");
		} else {
			strcat(request_url, "&");
		}
		strcat(request_url, opt_name);
		strcat(request_url, "=");
		strcat(request_url, opt_value);
	}
	
	token = sess_obj->token;
	request_url = vkapi_erealloc(request_url, sizeof(char) * (
	                                          strlen(request_url) +
	                                          strlen("access_token=") +
	                                          strlen(token) + 2));
	if (i == 0) {
		strcat(request_url, "?");
	} else {
		strcat(request_url, "&");
	}
	strcat(request_url, "access_token=");
	strcat(request_url, token);

	return request_url;
}

char  *
vkapi_get_method_str(enum vkapi_method_types method_type) 
{
	if (method_type == MESSAGES_SEND) {
		return "messages.send";
	} else {
		return NULL;
	}
}

char  *
vkapi_get_outer_id(char *user_id)
{
	/* iteration variables */
	int i;
	int c;

	char *outer_id;

	if (user_id[0] == '-') {
		outer_id = vkapi_emalloc(sizeof(char) * strlen(user_id) +
		                                        strlen("club") + 1);
		sprintf(outer_id, "club");
		i = 4;

		for (c = 1; user_id[c] != '\0'; ++c) {
			outer_id[i] = user_id[c];
			i += 1;
		}

		outer_id[i] = '\0';
	} else {
		outer_id = vkapi_emalloc(sizeof(char) * strlen(user_id) +
		                                        strlen("id") + 1);
		sprintf(outer_id, "id");
		i = 2;

		for (c = 0; user_id[c] != '\0'; ++c) {
			outer_id[i] = user_id[c];
			i += 1;
		}

		outer_id[i] = '\0';
	}

	return outer_id;
}
