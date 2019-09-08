#include <stddef.h>      /* size_t */

#include "vkapi_utils.h"

struct vkapi_response {
	int                       err_num;
	char                     *err_msg;
	struct vkapi_resp_items *obj;
};

struct vkapi_resp_item {
	char *name;
	char *value;
};

struct vkapi_resp_items {
	int  num;
	struct vkapi_resp_item **lst;
};


/* 
 * *add*, *del*, *free*, *gen*, *get* functions 
 * */ 

void                    vkapi_free_resp_obj(struct vkapi_response *);

struct vkapi_response  *vkapi_gen_resp_obj(void);

struct vkapi_response  *vkapi_add_resp_item(struct vkapi_response *,
                                            const char *, const char *);

struct vkapi_response  *vkapi_del_resp_item(struct vkapi_response *,
                                            char *);

char                   *vkapi_get_resp_item(struct vkapi_response *,
                                            char *);


/* VK API methods functions */
struct vkapi_response  *vkapi_messages_send(struct vkapi_sess_obj *,
                                            struct vkapi_opts *);
struct vkapi_response  *vkapi_docs_getMessagesUploadServer(struct vkapi_sess_obj *,
                                                           struct vkapi_opts *);
/* other helper functions */
struct vkapi_response  *vkapi_check_response(struct vkapi_response *,
                                             char *response);
