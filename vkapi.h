#include <stddef.h>      /* size_t */

#include "vkapi_utils.h"

struct vkapi_error {
	int err_num;
	char *err_msg;
};

struct vkapi_error  *vkapi_check_error(char *response);
struct vkapi_error  *vkapi_send_message(struct vkapi_sess_obj *,
                                        struct vkapi_opts *);
