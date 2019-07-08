#include <stddef.h>      /* size_t */

#include "vkapi_utils.h"

void   vkapi_check_response(void *ptr, size_t size, size_t nmemb, void *stream);
int    vkapi_send_message(struct vkapi_sess_obj *,
                          struct vkapi_opts *);
