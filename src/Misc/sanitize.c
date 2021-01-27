#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "misc.h"

int sanitized(char *str, char const *except)
{
	size_t i;
	size_t len = strlen(str);

	/* empty strings are not valid */
	if (!len)
		return 0;

	for (i = 0; i < len; i++) {

		/* removing new line */
		if (str[i] == '\n')
			str[i] = '\0';
		else if (!((isalpha(str[i]) && islower(str[i])) ||
				(strchr(except, str[i]))))
			return 0;

	}

	/* check if string is now empty */
	len = strlen(str);
	if (!len)
		return 0;

	return 1;
}
