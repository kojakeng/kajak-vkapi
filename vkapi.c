#include <curl/curl.h>
#include <stdio.h>       /* printf */
#include <stdlib.h>      /* atoi exit  */
#include <string.h>      /* strstr */

#include "vkapi.h"
#include "vkapi_alloc.h"
#include "vkapi_config.h"

struct curl_memory {
	char *mem;
	int size;
};

static size_t
writedata_curl(void *contents, size_t size,
               size_t nmemb, void *stream)
{
	int realsize;
	struct curl_memory *cmem; 
	realsize = size * nmemb;
	cmem = (struct curl_memory *) stream;
	cmem->mem = vkapi_erealloc(cmem->mem,
	                           cmem->size + realsize + 1);

	memcpy(&(cmem->mem[cmem->size]), contents, realsize);
	cmem->size += realsize;
	cmem->mem[cmem->size] = 0;

	return realsize;
}


void
vkapi_check_response(char *response)
{	
	/* iteration variables declaration */
	int i;

	/* normal variables declaration */
	char *err_code;
	char *err_msg;
	char *err_msg_block;
	char *err_msg_txt;
	char *err_str;


	err_code       = NULL;
	err_msg        = NULL;
	err_msg_block  = NULL;
	err_msg_txt    = NULL;
	err_str        = NULL;

	err_str = strstr(response, "error_code");
	if (err_str) {
		for (i = 0; err_str[i + 13] != ',' || 
		            err_str[i + 13] != '}' ||
		            err_str[i + 13] != ' '   ; ++i) {
			if (i == 0) {
				err_code = vkapi_emalloc(sizeof(char) * 2);
			} else {
				err_code = vkapi_erealloc(err_code, 
				                          sizeof(char) *
				                          (strlen(err_code) + 2));
			}
			err_code[i] = err_str[i + 13];
		}

		fprintf(stderr, "error code: %s", err_code);

		err_msg_block = strstr(response, "error_msg");
		if (err_msg_block) {
			err_msg_txt = strchr(err_msg, '\"');
			
			for (i = 0; err_msg_txt[i + 1] != '\"'; ++i) {
				if (i == 0) {
					err_msg = vkapi_emalloc(sizeof(char) * 2);
				} else {
					err_msg = vkapi_emalloc(sizeof(char) *
																	(strlen(err_msg) + 2));
				}

				err_msg[i] = err_msg_txt[i + 1];
			}

			fprintf(stderr, "error msg:");
			fprintf(stderr, "%s", err_msg);
		}

		free(err_msg);
		free(err_msg_block);
		free(err_msg_txt);
		free(err_str);

		if (atoi(err_code) > 0) {
			free(err_code);
			exit(EXIT_FAILURE);
		}

		free(err_code);
	}
}

int
vkapi_send_message(struct vkapi_sess_obj *sess_obj,
                   struct vkapi_opts *opts)
{
	/* iteration variables declaration */
	int                 i;

	/* bool variables declaration */
	char                have_message_opt;

	/* kajak-vkapi variables declaration */
	char               *request_url;

	/* libCURL variables declaration */
	CURL               *curl;
	CURLcode            res;
	struct curl_memory *vk_response;
	
	/* other variables declaration */
	char               *msg_txt;


	if (opts->method_type != MESSAGES_SEND){
		fprintf(stderr, "kajak-vkapi error:\n");
		fprintf(stderr, "incorrect method type\n");
		exit(EXIT_FAILURE);
	}

	curl = curl_easy_init();
	if (curl) {		
		have_message_opt = 0;
		for (i = 0; i < opts->num; ++i) {
			if (strcmp(opts->lst[i]->opt_name, "message") == 0) {
				have_message_opt = 1;
				msg_txt = opts->lst[i]->opt_value;
				opts->lst[i]->opt_value = curl_easy_escape(curl,
				                                           msg_txt, 0);
				break;
			}
		}

		if (!have_message_opt) {
			fprintf(stderr, "error in vkapi_send_message:");
			fprintf(stderr, "opts doesn't have \'message\' option.");
			exit(EXIT_FAILURE);
		}

		request_url = vkapi_gen_request(sess_obj, opts);

		vk_response = vkapi_emalloc(sizeof(struct curl_memory));
		vk_response->mem = vkapi_emalloc(sizeof(char));
		vk_response->size = 0;

		curl_easy_setopt(curl, CURLOPT_URL, request_url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writedata_curl);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) vk_response);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed:\n");
			fprintf(stderr, "%s\n", curl_easy_strerror(res));
			exit(EXIT_FAILURE);
		}

		vkapi_check_response(vk_response->mem);

		for (i = 0; i < opts->num; ++i) {
			if (strcmp(opts->lst[i]->opt_name, "message") == 0) {
				msg_txt = opts->lst[i]->opt_value;
				opts->lst[i]->opt_value = curl_easy_unescape(curl,
				                                             msg_txt, 0, NULL);

				break;
			}
		}
	} else {
		fprintf(stderr, "CURL error.");
		exit(EXIT_FAILURE);
	}

	free(request_url);
	free(vk_response->mem);
	free(vk_response);
	curl_easy_cleanup(curl);

	return 0;
}
