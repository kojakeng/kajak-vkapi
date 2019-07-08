#include "vkapi_types.h"


/*
 * vkapi_opt_obj - VK API method option structure contains option name and
 *                 it's value
 */
struct vkapi_opt {
	char  *opt_name;
	char  *opt_value;
};

/*
 * vkapi_opts - VK API method options structure contains list of options
 *              and it's length
 */
struct vkapi_opts {
	enum vkapi_method_types   method_type;
	struct vkapi_opt        **lst;
	int                       num;
};


/*
 * vkapi_add_opt - adds an option to vkapi_opts struct
 */
struct vkapi_opts		*vkapi_add_opt(struct vkapi_opts *, char *, char *);

/*
 * vkapi_gen_opts_obj - generates vkapi_opts struct object
 */
struct vkapi_opts		*vkapi_gen_opts_obj(char *, char *);

/*
 * vkapi_free_opts_obj - frees allocated vkapi_opts struct
 */
void  vkapi_free_opts_obj(struct vkapi_opts *);
