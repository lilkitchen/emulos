/*
 * utf8.c
 */

#include "utf8.h"

void utf8_upper(char *s)
{
	while (*s)
		s = utf8_upperc(s);
}

void utf8_lower(char *s)
{
	while (*s)
		s = utf8_lowerc(s);
}

char *utf8_upperc(char *s)
{
	unsigned char *str = (unsigned char *)s;

	if (*str == '\0')
		return (char *)str;

	/* Latin */
	if (*str <= 0x7f) {
		if (*str >= 0x61 && *str <= 0x7a)
			*str -= 0x20;

	/* Cyrillic */
	} else if (*str == 0xd0) {
		str++;
		/* Ё */
		if (*str == 0x81) {
			*str = 0x95;

		/* а - п */
		} else if (*str >= 0xb0)
			*str -= 0x20;

	} else if (*str == 0xd1) {
		str++;
		if (*str <= 0x8f) {
			*(str - 1) = 0xd0;
			*str += 0x20;

		/* ё */
		} else if (*str == 0x91) {
			*(str - 1) = 0xd0;
			*str = 0xb5;
		}
	}

	return (char *)str++;
}

char *utf8_lowerc(char *s)
{
	unsigned char *str = (unsigned char *)s;

	if (*str == '\0')
		return (char *)str;

	/* Latin */
	if (*str <= 0x7f) {
		if (*str >= 0x41 && *str <= 0x5a)
			*str += 0x20;

	 /* Cyrillic */
	} else if (*str == 0xd0) {
		str++;
		/* Ё */
		if (*str == 0x81) {
			*str = 0xb5;

		/* А - П */
		} else if (*str <= 0x9f) {
			*str += 0x20;

		} else if (*str <= 0xaf) {
			*(str - 1) = 0xd1;
			*str -= 0x20;
		}
	/* ё */
	} else if (*str == 0xd1 && *(++str) == 0x91) {
		*(str - 1) = 0xd0;
		*str = 0xb5;
	}

	return (char *)++str;
}

int utf8_match(char *str, char *sub)
{
	while (*str && *sub) {
		if (*str != *sub)
			return 0;
		str++;
		sub++;
	}
	return 1;
}

void utf8_unrepeat(char *s) {
	unsigned char *str = (unsigned char *)s;
	int i = 0;
	while (str[i]) {
		if (str[i] <= 0x7f) {
			if (str[i] == str[i + 1]) {
				for (int k = i; str[k]; k++)
					str[k] = str[k + 1];
				continue;
			}
			i++;
		} else {
			if (str[i] == str[i + 2] && str[i + 1] == str[i + 3]) {
				for (int k = i; str[k]; k += 2) {
					str[k] = str[k + 2];
					str[k + 1] = str[k + 3];
				}
				continue;
			}
			i += 2;
		}
	}
}
