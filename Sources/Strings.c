
#include "header.h"

/* Проблемы:
*	CopyString() - а если строка, куда копируем меньше по размеру, чем та, из которой
*					происходит копирование? Нужно обработать такой случай.

/* CopyString():	Copies no more than "n" symbols from one string to another. 
*					If n == 0, then copies all symbols.
*
*/
void CopyString(char *to, char *from, int n)
{
	if (n == 0)
		while (*to++ = *from++)
			;
	else
		while ((*to++ = *from++) && n-- > 0 )
			;
}

/* ReadString():	Reads no more than "limit" symbols ('\0' is included) from the INPUT and writes them into the "line" as C-string.
*
*  Comment:			1) А что делать с пустой строкой?
*/
int ReadString(char *line, int limit)
{
	char c;
	int length;

	length = 0;
	while (--limit > 0 && (c = getchar()) != EOF) {
		*line++ = c;
		length++;
		if (c == '\n')
			break;
	}
	*line = '\0';

	return length;
}

/* CompareStrings():	Compares strings and returns "1" if they are equal. In another case returns "0".
*						If (ignore_reg == true), then the register is ignored.
*/
int CompareStrings(char *first_str, char *second_str, int ignore_reg) 
{
	char f, s;

	do {
		f = *first_str++;
		s = *second_str++;
		if (ignore_reg) {
			f = mcr__ToLow(f);
			s = mcr__ToLow(s);
		}
		
		if (f == '\0' && s == '\0')
			return 1;
	} while (f == s);

	return 0;
}

