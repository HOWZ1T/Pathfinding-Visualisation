#include "string_utils.h"

// concatenates two char arrays and returns the resulting char array
// returns NULL if malloc failed
// the resultant pointer should be freed when no longer needed
char* concat(const char* s1, const char* s2)
{
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);
	char* result = malloc(len1 + len2 + 1); // +1 for null terminator

	if (result == NULL)
	{
		return NULL;
	}

	// memcpy(destination, source, number of bytes to be copied
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1); // +1 for null terminator

	return result;
}
