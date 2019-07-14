#include <stdio.h>       /* printf */

#include "vkapi.h"

int main(void)
{
	char               *jsn_err;
	struct vkapi_error *err_obj;


	jsn_err = "{\"error\": {\"error_code\": 100, \"error_msg\": \"One of the parameters specified was missing or invalid: random_id is a required parameter\"}}";

	err_obj = vkapi_check_error(jsn_err);

	printf("err_num: %d\n", err_obj->err_num);

	if (err_obj->err_msg != NULL) {
		printf("err_msg: %s\n", err_obj->err_msg);
	} else {
		printf("err_msg is NULL\n");
	}

	return 0;
}
