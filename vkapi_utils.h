#include "vkapi_opts.h"
#include "vkapi_session.h"

struct vkapi_request_parts {
	char *method_url;
	char *opts_str;
};

char  *vkapi_create_mention(char *, char *);

/*
 * vkapi_gen_request - generates url string of VK API request
 */
struct vkapi_request_parts  *vkapi_gen_request(struct vkapi_sess_obj *,
                                               struct vkapi_opts *);

char  *vkapi_get_method_str(enum vkapi_method_types);

char  *vkapi_get_outer_id(char *);

void   vkapi_free_request(struct vkapi_request_parts *);
