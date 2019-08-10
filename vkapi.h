#include <stddef.h>      /* size_t */

#include "vkapi_utils.h"

struct vkapi_response {
	int     err_num;
	char   *err_msg;
	struct vkapi_resp_items *obj;
};

struct vkapi_resp_item {
	char *name;
	char *value;
};

struct vkapi_resp_items {
	int  num;
	struct vkapi_resp_item *lst;
};


struct vkapi_response  *vkapi_handle_response(char *response);
struct vkapi_response  *vkapi_messages_send(struct vkapi_sess_obj *,
                                         struct vkapi_opts *);
struct vkapi_response  *vkapi_docs_getMessagesUploadServer(struct vkapi_sess_obj *,
                                                           struct vkapi_opts *);
