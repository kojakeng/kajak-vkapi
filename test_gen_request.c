#include <stdio.h>
#include <stdlib.h>

#include "vkapi_utils.h"

int
main(void)
{
	struct vkapi_sess_obj      *sess_obj;
	struct vkapi_opts          *opts;
	struct vkapi_request_parts *request_parts;

	char *message_text = "Howdy ho! I'm Jack the dog and this is a test message for testing POST request making.";

	sess_obj = vkapi_gen_group_sessobj("testtoken");

	opts = vkapi_gen_opts_obj("v", "5.95");

	opts->request_type = POST_REQUEST;
	opts->method_type = MESSAGES_SEND;

	vkapi_add_opt(opts, "peer_id", "1488");
	vkapi_add_opt(opts, "hey", "228");
	vkapi_add_opt(opts, "message", message_text);

	request_parts = vkapi_gen_request(sess_obj, opts);
	printf("request_parts->method_url: %s\n",
	       request_parts->method_url);
	printf("request_parts->opts_str: %s\n",
	       request_parts->opts_str);

	vkapi_free_opts_obj(opts);
	vkapi_free_request(request_parts);
	exit(EXIT_SUCCESS);
}
