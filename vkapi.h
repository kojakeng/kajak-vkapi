#include <stddef.h>      /* size_t */

#include "vkapi_utils.h"

void   vkapi_check_response(char *response);
int    vkapi_send_message(struct vkapi_sess_obj *,
                          struct vkapi_opts *);
