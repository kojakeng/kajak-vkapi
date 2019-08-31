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

struct vkapi_response *
vkapi_handle_response(char *response)
{	
	/* iteration variables declaration */
	int i;

	/* other variables declaration */
	struct json_object    *jsn_response;
	struct json_object    *jsn_err;
	struct json_object    *jsn_err_num;
	struct json_object    *jsn_err_msg;
	struct json_object    *jsn_resp_obj;
	struct vkapi_response *resp;


	jsn_response = json_tokener_parse(response);

	jsn_err = json_object_object_get(jsn_response, "error");
	
	resp = vkapi_emalloc(sizeof(struct vkapi_response));
	resp->err_msg = NULL;
	resp->obj = NULL;

	if (jsn_err != NULL) {
		jsn_err_num = json_object_object_get(jsn_err, "error_code");
		resp->err_num = json_object_get_int(jsn_err_num);

		jsn_err_msg = json_object_object_get(jsn_err, "error_msg");
		resp->err_msg = vkapi_emalloc(sizeof(char) *
		                                 strlen(json_object_get_string(
		                                          jsn_err_msg)));
		strcpy(resp->err_msg, json_object_get_string(jsn_err_msg));

	} else {
		resp->err_num = 0;
	}

	jsn_resp_obj = json_object_object_get(jsn_response, "response");

	return resp;
}

struct vkapi_response *
vkapi_messages_send(struct vkapi_sess_obj *sess_obj,
                    struct vkapi_opts *opts)
{
	/* iteration variables declaration */
	int                 i;

	/* kajak-vkapi variables declaration */
	struct vkapi_request_parts     *request_parts;
	char                           *request_url;
	struct vkapi_response          *resp;

	/* libCURL variables declaration */
	CURL               *curl;
	CURLcode            res;
	struct curl_memory *vk_response;
	
	/* other variables declaration */
	char               *msg_txt;


	resp = vkapi_emalloc(sizeof(struct vkapi_response));
	resp->err_num = 0;
	resp->err_msg = NULL;
	resp->obj = NULL;

	if (opts->request_type == DEFAULT) {
		opts->request_type = POST_REQUEST;
	}

	if (opts->method_type != MESSAGES_SEND){
		resp->err_num = -1;
		resp->err_msg = vkapi_emalloc(strlen("wrong opts->method_type"));
		strcpy(resp->err_msg, "wrong opts->method_type");

		return resp;
	}

	curl = curl_easy_init();
	if (curl) {		
		for (i = 0; i < opts->num; ++i) {
			if (strcmp(opts->lst[i]->opt_name, "message") == 0) {
				msg_txt = opts->lst[i]->opt_value;
				opts->lst[i]->opt_value = curl_easy_escape(curl,
				                                           msg_txt, 0);
				break;
			}
		}

		request_parts = vkapi_gen_request(sess_obj, opts);

		vk_response = vkapi_emalloc(sizeof(struct curl_memory));
		vk_response->mem = vkapi_emalloc(sizeof(char));
		vk_response->size = 0;

		if (opts->request_type == GET_REQUEST) {
			request_url = vkapi_emalloc(sizeof(char) * (
			                            strlen(request_parts->method_url) +
																	strlen(request_parts->opts_str)) + 1);

			curl_easy_setopt(curl, CURLOPT_URL, request_url);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writedata_curl);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) vk_response);
			free(request_url);
		} else {
			curl_easy_setopt(curl, CURLOPT_URL, request_parts->method_url);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_parts->opts_str);
		}

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed:\n");
			fprintf(stderr, "%s\n", curl_easy_strerror(res));
			exit(EXIT_FAILURE);
		}

		resp = vkapi_handle_response(vk_response->mem);
		if (resp->err_num != 0) {
			free(vk_response->mem);
			free(vk_response);
			curl_easy_cleanup(curl);

			return resp;
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

	vkapi_free_request(request_parts);
	free(vk_response->mem);
	free(vk_response);
	curl_easy_cleanup(curl);

	return resp;
}

struct vkapi_response *
vkapi_docs_getMessagesUploadServer(struct vkapi_sess_obj *sess_obj,
                                   struct vkapi_opts *opts)
{
	/* kajak-vkapi variables declaration */
	struct vkapi_request_parts   *request_url;
	struct vkapi_response *resp;

	/* libCURL variables declaration */
	CURL               *curl;
	CURLcode            res;
	struct curl_memory *vk_response;

	resp = vkapi_emalloc(sizeof(struct vkapi_response));
	resp->err_num = 0;
	resp->err_msg = NULL;
	resp->obj = NULL;

	request_url = vkapi_gen_request(sess_obj, opts);

	curl = curl_easy_init();

	if (curl) {		
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

		resp = vkapi_handle_response(vk_response->mem);
		if (resp->err_num != 0) {
			return resp;
		}
	} else {
		fprintf(stderr, "CURL error.");
		exit(EXIT_FAILURE);
	}

	free(request_url);
	free(vk_response->mem);
	free(vk_response);
	curl_easy_cleanup(curl);

	return resp;
}
