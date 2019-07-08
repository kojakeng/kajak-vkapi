#include "vkapi_alloc.h"
#include "vkapi_session.h"


struct vkapi_sess_obj *
vkapi_gen_group_sessobj(char *token)
{
	struct vkapi_sess_obj *sess_obj;

	sess_obj = vkapi_emalloc(sizeof(struct vkapi_sess_obj));
	sess_obj->token = token;

	return sess_obj;
}
