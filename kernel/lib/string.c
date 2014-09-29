#include <string.h>

/*
 * Separates string s by delim, returning either NULL if there is no more 
 * strings that can be split out, or the next split if there is any.
 */
char* strtok(char *s, const char *delim) {
	char *spanp;
	int c, sc;
	char *tok;
	static char *last;

	if (s == NULL && (s = last) == NULL)
		return NULL;

	// Skip (span) leading delimiters (s += strspn(s, delim), sort of).
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) { // no non-delimiter characters
		last = NULL;
		return NULL;
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *) delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return tok;
			}
		} while (sc != 0);
	}
}

/*
 * Returns a pointer to the first occurrence of character c in string s.
 */
char* strchr(const char *s, int c) {
	const char ch = c;

	for ( ; *s != ch; s++)
		if (*s == '\0')
			return NULL;
	return (char *) s;
}

/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.  
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no more tokens).
 *
 * If *stringp is NULL, strsep returns NULL.
 */
char* strsep(char **stringp, const char *delim) {
	char *s;
	const char *spanp;
	int c, sc;
	char *tok;

	if ((s = *stringp) == NULL) {
		return NULL;
	}

	for (tok = s;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0) {
					s = NULL;
				} else {
					s[-1] = 0;
				}

				*stringp = s;
				return tok;
			}
		} while (sc != 0);
	}
}




/*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison. The mappings are
 * based upon ASCII character sequences.
 */
static const unsigned char strcasecmp_charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

/*
 * Performs a case-insensitive comparison between the two strings. Functions
 * otherwise identically to strcmp.
 */
int strcasecmp(const char *s1, const char *s2) {
	const unsigned char *cm = strcasecmp_charmap,
			*us1 = (const unsigned char *) s1,
			*us2 = (const unsigned char *) s2;

	while (cm[*us1] == cm[*us2++]) {
		if (*us1++ == '\0') {
			return 0;
		}
	}

	return (cm[*us1] - cm[*--us2]);
}

/*
 * Performs a case-insensitive comparison between the two strings. Functions
 * otherwise identically to strcmp.
 *
 * However, only n bytes are checked.
 */
int strncasecmp(const char *s1, const char *s2, size_t n) {
	if (n != 0) {
		const unsigned char *cm = strcasecmp_charmap,
				*us1 = (const unsigned char *) s1,
				*us2 = (const unsigned char *) s2;

		do {
			if (cm[*us1] != cm[*us2++]) { 
				return (cm[*us1] - cm[*--us2]);
			}

			if (*us1++ == '\0') {
				break;
			}
		} while (--n != 0);
	}

	return 0;
}

/*
 * Compares the first num bytes in two blocks of memory.
 * Returns 0 if equal, a value greater than 0 if the first byte in ptr1 is
 * greater than the first byte in ptr2; and a value less than zero if the
 * opposite. Note that these comparisons are performed on uint8_t types.
 */
int memcmp(const void* ptr1, const void* ptr2, size_t num) {
	uint8_t *read1 = (uint8_t *) ptr1;
	uint8_t *read2 = (uint8_t *) ptr2;

	for(size_t i = 0; i < num; i++) {
		if(read1[i] != read2[i]) {
			if(read1[i] > read2[i]) return 1;
			else return -1;
		}
	}

	return 0;
}


int strcmp(const char* s1, const char* s2) {
	for (; (*s1 == *s2) && (*s1 != '\0'); s1++, s2++);
	return (*s1 == *s2) ? 0 : *s1 - *s2;
}

void *memcpy(void *dest,const void *src,size_t n) {
	volatile uint32_t num_dwords = n/4;
	volatile uint32_t num_bytes = n%4;
	volatile uint32_t *dest32 = (uint32_t*)dest;
	volatile uint32_t *src32 = (uint32_t*)src;
	volatile uint8_t *dest8 = ((uint8_t*)dest)+num_dwords*4;
	volatile uint8_t *src8 = ((uint8_t*)src)+num_dwords*4;
	uint32_t i;

	for (i=0;i<num_dwords;i++) {
		dest32[i] = src32[i];
	}
	for (i=0;i<num_bytes;i++) {
		dest8[i] = src8[i];
	}
	return dest;
}
void *memset(void *dest,int val,size_t n) {
	volatile uint32_t num_dwords = n/4;
	volatile uint32_t num_bytes = n%4;
	volatile uint32_t *dest32 = (uint32_t*)dest;
	volatile uint8_t *dest8 = ((uint8_t*)dest)+num_dwords*4;
	volatile uint8_t val8 = (uint8_t)val;
	volatile uint32_t val32 = val|(val<<8)|(val<<16)|(val<<24);
	uint32_t i;

	for (i=0;i<num_dwords;i++) {
		dest32[i] = val32;
	}
	for (i=0;i<num_bytes;i++) {
		dest8[i] = val8;
	}
  return dest;
}

void memmove(void *dest, const void *src, size_t n)
{
	size_t i;
	unsigned char *a = dest;
	const unsigned char *b = src;

	if(src < dest)
	{
		for (i = n; i > 0; --i)
		{
			a[i-1] = b[i-1];
		}
	}
	else
	{
		for (i = 0; i < n; ++i)
		{
			a[i] = b[i];
		}
	}
}
char * strcpy(char * dest, const char * src) {
	int len = strlen(src);
	memcpy(dest, src, len+1);
	return dest;
}
size_t strlen(const char *str)
{
	const char *wrk=str;
	int retval;
	for(retval = 0; *wrk != '\0'; wrk++) retval++;
	return retval;
}
char *strdup (char *s) {
	/**
    char *d = malloc (strlen (s) + 1);   // Space for length plus nul
    if (d == NULL) return NULL;          // No memory
    strcpy (d,s);                        // Copy the characters
    return d;                            // Return the new string
    **/
    return NULL;
}
char * strtok_r(char * str, const char * delim, char ** saveptr)
{
	char * token;
	if (str == NULL) {
		str = *saveptr;
	}
	str += strspn(str, delim);
	if (*str == '\0') {
		*saveptr = str;
		return NULL;
	}
	token = str;
	str = strpbrk(token, delim);
	if (str == NULL) {
		*saveptr = (char *)lfind(token, '\0');
	} else {
		*str = '\0';
		*saveptr = str + 1;
	}
	return token;
}
size_t strspn(const char * str, const char * accept) {
	const char * ptr = str;
	const char * acc;

	while (*str) {
		for (acc = accept; *acc; ++acc) {
			if (*str == *acc) {
				break;
			}
		}
		if (*acc == '\0') {
			break;
		}

		str++;
	}

	return str - ptr;
}

char * strpbrk(const char * str, const char * accept) {
	const char *acc = accept;

	if (!*str) {
		return NULL;
	}

	while (*str) {
		for (acc = accept; *acc; ++acc) {
			if (*str == *acc) {
				break;
			}
		}
		if (*acc) {
			break;
		}
		++str;
	}

	if (*acc == '\0') {
		return NULL;
	}

	return (char *)str;
}
size_t lfind(const char * str, const char accept)
{
	size_t i = 0;
	while ( str[i] != accept) {
		i++;
	}
	return (size_t)(str) + i;
}
