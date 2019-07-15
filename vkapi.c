#include <curl/curl.h>
#include <json-c/json.h>
#include <stdio.h>       /* printf */
#include <stdlib.h>      /* atoi exit  */
#include <string.h>      /* strcpy strstr */

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

struct vkapi_error *
vkapi_check_error(char *response)
{	
	/* iteration variables declaration */
	int i;

	/* normal variables declaration */
	struct json_object *jsn_response;
	struct json_object *jsn_err;
	struct json_object *jsn_err_num;
	struct json_object *jsn_err_msg;
	struct vkapi_error *err_obj;


	jsn_response = json_tokener_parse(response);

	jsn_err = json_object_object_get(jsn_response, "error");
	
	err_obj = vkapi_emalloc(sizeof(struct vkapi_error));
	err_obj->err_msg = NULL;

	if (jsn_response != NULL) {
		jsn_err_num = json_object_object_get(jsn_err, "error_code");
		err_obj->err_num = json_object_get_int(jsn_err_num);

		jsn_err_msg = json_object_object_get(jsn_err, "error_msg");
		err_obj->err_msg = vkapi_emalloc(sizeof(char) *
		                                 strlen(json_object_get_string(
		                                                       jsn_err_msg)));
		strcpy(err_obj->err_msg, json_object_get_string(jsn_err_msg));
	} else {
		err_obj->err_num = 0;
	}

	return err_obj;
}

struct vkapi_error *
vkapi_send_message(struct vkapi_sess_obj *sess_obj,
                   struct vkapi_opts *opts)
{
	/* iteration variables declaration */
	int                 i;

	/* bool variables declaration */
	char                have_message_opt;

	/* kajak-vkapi variables declaration */
	char               *request_url;
	struct vkapi_error *vk_error;

	/* libCURL variables declaration */
	CURL               *curl;
	CURLcode            res;
	struct curl_memory *vk_response;
	
	/* other variables declaration */
	char               *msg_txt;


	vk_error = vkapi_emalloc(sizeof(struct vkapi_error));
	vk_error->err_num = 0;
	vk_error->err_msg = NULL;

	if (opts->method_type != MESSAGES_SEND){
		vk_error->err_num = -1;
		vk_error->err_msg = vkapi_emalloc(strlen(
		                                   "wrong opts->method_type"));
		strcpy(vk_error->err_msg, "wrong opts->method_type");

		return vk_error;
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

		vk_error = vkapi_check_error(vk_response->mem);
		if (vk_error != 0) {
			return vk_error;
		}

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


	return vk_error;
}
