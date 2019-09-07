#include <stdio.h>

#include "vkapi.h"

int main(void)
{
	/* iteration variables declaration */
	int i;

	/* normal variable declaration */
	struct vkapi_response *resp;
	char                  *result;

	resp = vkapi_gen_resp_obj();

	vkapi_add_resp_item(resp, "hey", "one");
	vkapi_add_resp_item(resp, "test", "value");

	printf("\nresp object before deleting item:\n\n");
	for (i=0; i < resp->obj->num; ++i) {
		printf("item #%d:\n", i);
		printf("name: %s\n", resp->obj->lst[i]->name);
		printf("value: %s\n\n", resp->obj->lst[i]->value);
	}

	vkapi_del_resp_item(resp, "hey");

	printf("\nresp object after deleting item:\n\n");
	for (i=0; i < resp->obj->num; ++i) {
		printf("item #%d:\n", i);
		printf("name: %s\n", resp->obj->lst[i]->name);
		printf("value: %s\n\n", resp->obj->lst[i]->value);
	}


	result = vkapi_get_resp_item(resp, "test");

	if (result == NULL) {
		printf("result is NULL!");
		return 1;
	}

	printf("\"test\" item value: %s", result);

	return 0;
}
