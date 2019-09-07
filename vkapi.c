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


void
vkapi_free_resp_obj(struct vkapi_response *resp) 
{
	/* iteration variables */
	int i;

	free(resp->err_msg);
	if (resp->obj != NULL) {
		for (i=0; i < resp->obj->num; ++i) {
			free(resp->obj->lst[i]->name);
			free(resp->obj->lst[i]->value);
		}
	}
}


struct vkapi_response *
vkapi_gen_resp_obj(void)
{
	struct vkapi_response *resp;

	resp = vkapi_emalloc(sizeof(struct vkapi_response));
	resp->err_num = 0;
	resp->err_msg = NULL;
	resp->obj = vkapi_emalloc(sizeof(struct vkapi_resp_items));

	resp->obj->num = 0;
	resp->obj->lst = NULL;

	return resp;
}


struct vkapi_response *
vkapi_add_resp_item(struct vkapi_response *resp,
                    char *name, char *value)
{
	int                     item_count;
	struct vkapi_resp_item *resp_item;

	item_count = resp->obj->num;
	
	if (item_count == 0) {
		resp->obj->lst = vkapi_emalloc(sizeof(struct vkapi_resp_item));
	} else {
		resp->obj->lst = vkapi_erealloc(resp->obj->lst,
		                                sizeof(struct vkapi_resp_item) *
		                                (item_count + 1));
	}

	resp_item = vkapi_emalloc(sizeof(struct vkapi_resp_item));
	resp_item->name = vkapi_emalloc(sizeof(char) * strlen(name));
	resp_item->value = vkapi_emalloc(sizeof(char) * strlen(value));

	strcpy(resp_item->name, name);
	strcpy(resp_item->value, value);

	resp->obj->lst[item_count] = resp_item;

	++resp->obj->num;

	return resp;
}

struct vkapi_response *
vkapi_del_resp_item(struct vkapi_response *resp,
                    char  *name)
{
	/* iteration variables declaration */
	int i;

	/* bool variables declaration */
	char is_item_found;

	/* normal variables declaration */
	struct vkapi_resp_item *resp_item;
	int                     founded_item_num;
	int                     item_count;

	item_count = resp->obj->num;

	is_item_found = 0;
	for(i=0; i < item_count; ++i) {
		resp_item = resp->obj->lst[i];

		if (is_item_found) {
			if ((founded_item_num + 1) == item_count || (i + 1) == item_count) {
				--resp->obj->num;
				
				resp->obj->lst = vkapi_erealloc(resp->obj->lst,
				                                sizeof(struct vkapi_resp_items) *
				                                resp->obj->num);

				break;

			} else {
				resp->obj->lst[i] = resp->obj->lst[i + 1];
			}

		} else if (strcmp(resp_item->name, name) == 0) {
			founded_item_num = i;
			free(resp_item->name);
			free(resp_item->value);
			free(resp_item);

			resp->obj->lst[i] = resp->obj->lst[i + 1];
			
			is_item_found = 1;
		}
	}

	return resp;
}


char *
vkapi_get_resp_item(struct vkapi_response *resp,
                    char *item_name)
{
	/* iteration variables declaration */
	int i;

	/* normal variables declaration */
	int                     items_count;
	struct vkapi_resp_item *resp_item;

	items_count = resp->obj->num;

	for (i=0; i < items_count; ++i) {
		resp_item = resp->obj->lst[i];

		if (strcmp(resp_item->name, item_name) == 0) {
			return resp_item->value;
		}
	}
	
	return NULL;
}

struct vkapi_response *
vkapi_check_response(char *response)
{	
	/* iteration variables declaration */
	int i;

	/* other variables declaration */
	struct json_object    *jsn_response;
	struct json_object    *jsn_err;
	struct json_object    *jsn_err_num;
	struct json_object    *jsn_err_msg;
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

	return resp;
}


struct vkapi_response *
vkapi_messages_send(struct vkapi_sess_obj *sess_obj,
                    struct vkapi_opts *opts)
{
	/* iteration variables declaration */
	int                             i;

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

		resp = vkapi_check_response(vk_response->mem);
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
	char                         *request_url;
	struct vkapi_request_parts   *request_parts;
	struct vkapi_response *resp;

	/* libCURL variables declaration */
	CURL               *curl;
	CURLcode            res;
	struct curl_memory *vk_response;

	/* json variables */
	struct json_object    *jsn_response;
	struct json_object    *jsn_resp_obj;

	if (opts->request_type == DEFAULT) {
		opts->request_type = GET_REQUEST;
	}

	resp = vkapi_emalloc(sizeof(struct vkapi_response));
	resp->err_num = 0;
	resp->err_msg = NULL;
	resp->obj = NULL;

	request_parts = vkapi_gen_request(sess_obj, opts);

	curl = curl_easy_init();

	if (curl) {		
		vk_response = vkapi_emalloc(sizeof(struct curl_memory));
		vk_response->mem = vkapi_emalloc(sizeof(char));
		vk_response->size = 0;

		if (opts->request_type == GET_REQUEST) {
			request_url = vkapi_emalloc(sizeof(char) * (
			                            strlen(request_parts->method_url) +
																	strlen(request_parts->opts_str)) + 1);

			strcpy(request_url, request_parts->method_url);
			strcat(request_url, request_parts->opts_str);

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
			resp->err_num = 200;
		}

		resp = vkapi_check_response(vk_response->mem);
		if (resp->err_num != 0) {
			return resp;
		}

		jsn_response = json_tokener_parse(vk_response->mem);

		jsn_resp_obj = json_object_object_get(jsn_response, "response");

		if (jsn_resp_obj == NULL) {
			resp->err_num = 1;

			return resp;
		} else {
			
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
