#include <stdio.h>          /* printf */
#include <stdlib.h>					/* free */
#include <string.h>

#include "vkapi_alloc.h"
#include "vkapi_opts.h"


struct vkapi_opts *
vkapi_add_opt(struct vkapi_opts *opts,
              char *opt_name,
              char *opt_value)
{
	opts->lst = vkapi_erealloc(opts->lst, sizeof(struct vkapi_opts *) *
	                                      (opts->num + 1));
	
	opts->num += 1;

	opts->lst[opts->num - 1] = vkapi_emalloc(sizeof(struct vkapi_opt));

	opts->lst[opts->num - 1]->opt_name = vkapi_emalloc(sizeof(char) *
	                                                   strlen(opt_name));
	opts->lst[opts->num - 1]->opt_value = vkapi_emalloc(sizeof(char) *
	                                                    strlen(opt_value));

	strcpy(opts->lst[opts->num - 1]->opt_name, opt_name);
	strcpy(opts->lst[opts->num - 1]->opt_value, opt_value);

	return opts;
}

void
vkapi_free_opts_obj(struct vkapi_opts *opts)
{
	/* iteration variables */
	int i;

	for (i = 0; i < opts->num; ++i) {
		free(opts->lst[i]);
	}

	free(opts->lst);
	free(opts);
}

struct vkapi_opts *
vkapi_gen_opts_obj(char *opt_name,
                   char *opt_value)
{
	struct vkapi_opts		 *opts;

	opts = vkapi_emalloc(sizeof(struct vkapi_opts));
	opts->num = 0;
	opts->lst = vkapi_emalloc(sizeof(struct vkapi_opt *));
	opts->lst[0] = vkapi_emalloc(sizeof(struct vkapi_opt));
	
	opts->lst[0]->opt_name = vkapi_emalloc(sizeof(char) *
	                                       strlen(opt_name));
	opts->lst[0]->opt_value = vkapi_emalloc(sizeof(char) *
	                                        strlen(opt_value));

	strcpy(opts->lst[0]->opt_name, opt_name);
	strcpy(opts->lst[0]->opt_value, opt_value);

	opts->num += 1;

	return opts;
}
