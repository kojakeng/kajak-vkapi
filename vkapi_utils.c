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

struct vkapi_request_parts *
vkapi_gen_request(struct vkapi_sess_obj *sess_obj,
                  struct vkapi_opts     *opts)
{
	/* iteration variables declaration */
	int i;

	/* temporary variables declaration */
	char *opt_name;
	char *opt_value;

	/* normal variables declaration */
	struct vkapi_request_parts *request_parts;
	char *method_str;
	char *token;

	request_parts = vkapi_emalloc(sizeof(struct vkapi_request_parts *));
	request_parts->method_url = NULL;
	request_parts->opts_str = NULL;

	method_str = vkapi_get_method_str(opts->method_type);
	if (method_str == NULL) {
		fprintf(stderr, "FATAL ERROR vkapi_utils.c:");
		fprintf(stderr, "can't get method string in vkapi_gen_request");
		exit(EXIT_FAILURE);
	}


	/* VK_API_URL + method_str + '/' + '?' + '\0' */
	request_parts->method_url = vkapi_emalloc(sizeof(char) * 
	                                          (strlen(VK_API_URL) +
	                                           strlen(method_str) + 4));
	strcpy(request_parts->method_url, VK_API_URL);
	strcat(request_parts->method_url, "/");
	strcat(request_parts->method_url, method_str);

	for (i = 0; i < opts->num; ++i) {
		opt_name = opts->lst[i]->opt_name;
		opt_value = opts->lst[i]->opt_value;

		if (i == 0) {
			request_parts->opts_str = vkapi_emalloc(sizeof(char) *
																							(strlen(opt_name) +
			                                         strlen(opt_value) + 3));
		} else {
			request_parts->opts_str = vkapi_erealloc(request_parts->opts_str,
			                                         sizeof(char) * 
			                                         (strlen(opt_name) +
			                                          strlen(opt_value) + 3));

		}

		if (i == 0) {
			if (opts->request_type == GET_REQUEST) {
				strcpy(request_parts->opts_str, "?");
				strcat(request_parts->opts_str, opt_name);
				strcat(request_parts->opts_str, "=");
				strcat(request_parts->opts_str, opt_value);
			} else {
				strcpy(request_parts->opts_str, opt_name);
				strcat(request_parts->opts_str, "=");
				strcat(request_parts->opts_str, opt_value);
			}
		} else {
			strcat(request_parts->opts_str, "&");
			strcat(request_parts->opts_str, opt_name);
			strcat(request_parts->opts_str, "=");
			strcat(request_parts->opts_str, opt_value);
		}
	}
	
	token = sess_obj->token;
	request_parts->opts_str = vkapi_erealloc(request_parts->opts_str, sizeof(char) * (
	                                          strlen(request_parts->opts_str) +
	                                          strlen("access_token=") +
	                                          strlen(token) + 5));
	if (i == 0) {
		if (opts->request_type == GET_REQUEST) {
			strcpy(request_parts->opts_str, "?");
		}
	} else {
		strcat(request_parts->opts_str, "&");
	}
	strcat(request_parts->opts_str, "access_token=");
	strcat(request_parts->opts_str, token);

	return request_parts;
}

void vkapi_free_request(struct vkapi_request_parts *request_parts)
{
	free(request_parts->method_url);
	free(request_parts->opts_str);
	free(request_parts);
}

char  *
vkapi_get_method_str(enum vkapi_method_types method_type) 
{
	if (method_type == MESSAGES_SEND) {
		return "messages.send";
	} else if (method_type == DOCS_GETMESSAGESUPLOADSERVER) {
		return "docs.getMessagesUploadServer";
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
