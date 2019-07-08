#include <stdio.h>
#include <stdlib.h>

#include "vkapi_utils.h"

int
main(void)
{
	struct vkapi_sess_obj *sess_obj;
	struct vkapi_opts *opts;
	char *request_url;


	sess_obj = vkapi_gen_group_sessobj("testtoken");

	opts = vkapi_gen_opts_obj("v", "5.95");
	opts->method_type = MESSAGES_SEND;
	opts = vkapi_add_opt(opts, "peer_id", "1488");
	opts = vkapi_add_opt(opts, "hey", "228");

	request_url = vkapi_gen_request(sess_obj,
	                                opts);
	printf("%s", request_url);

	vkapi_free_opts_obj(opts);
	exit(EXIT_SUCCESS);
}
