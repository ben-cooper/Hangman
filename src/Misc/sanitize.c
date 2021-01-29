#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "misc.h"

int sanitized(char const *str, char const *except)
{
	size_t i;
	size_t len = strlen(str);

	/* empty strings are not valid */
	if (!len)
		return 0;

	for (i = 0; i < len; i++)
		if (!((isalpha(str[i]) && islower(str[i])) ||(strchr(except, str[i]))))
			return 0;

	return 1;
}
