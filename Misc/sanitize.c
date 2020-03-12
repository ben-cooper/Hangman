#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "misc.h"

/* return false if string contains non alphabetical characters
otherwise convert characters to lowercase and return true */
int sanitize(char *str, char const *exceptions)
{
	size_t i;
	size_t len = strlen(str);
	
	/* empty strings are not valid */
	if (!len)
		return 0;

	for (i = 0; i < len; i++) {

		/* removing new line */
		if (str[i] == '\n') {
			str[i] = '\0';
		} else if (!((isalpha(str[i]) && islower(str[i])) ||
			     (index(exceptions, str[i])))) {
			/* illegal character found */
			return 0;
		}
	}
	
	/* check if string is now empty */
	len = strlen(str);
	if (!len)
		return 0;

	return 1;
}
