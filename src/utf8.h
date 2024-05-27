/*
 * utf8.h
 */

#ifndef EMULOS_UTF8_H_
#define EMULOS_UTF8_H_

void utf8_upper(char *s);
void utf8_lower(char *s);

char *utf8_upperc(char *s);
char *utf8_lowerc(char *s);

int utf8_match(char *str, char *sub);
void utf8_unrepeat(char *s);

#endif
