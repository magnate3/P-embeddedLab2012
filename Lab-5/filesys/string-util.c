#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)                                                                      
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

#define SS (sizeof(size_t))
void *memset(void *dest, int c, size_t n)
{
	unsigned char *s = dest;
	c = (unsigned char)c;
	for (; ((uintptr_t)s & ALIGN) && n; n--) *s++ = c;			// to aligned address
	if (n) {
		size_t *w, k = ONES * c;								// k = cccc....
		for (w = (void *)s; n>=SS; n-=SS, w++) *w = k;			// fill larget size SS each time
		for (s = (void *)w; n; n--, s++) *s = c;				// remain
	}
	return dest;
}

char *strcpy(char *dest, const char *src)
{
	const unsigned char *s = src;
	unsigned char *d = dest;
	while ((*d++ = *s++));
	return dest;
}


void* memcpy(void* dst, void* src, size_t n){
//{{{	
	unsigned char *d = dst;
	unsigned char *s = src;

	if( ((uintptr_t)d & ALIGN) == ((uintptr_t)s & ALIGN) ){
		for (; ((uintptr_t)d & ALIGN) && n; n--) *d++ = *s++;			// to aligned address
	}
	if (n) {
		if( !( ((uintptr_t)d & ALIGN) | ((uintptr_t)s & ALIGN) ) ){		// both aligned
			size_t *wd = (void*) d;
			size_t *ws = (void*) s;
			for(; n>=SS; n-=SS) *wd++ = *ws++;							// max copy
		}
		for(; n; n--) *d++ = *s++;										// remain
	}
	return dst;
//}}}	
}

/*
Copies at most count characters of the byte string pointed to by src (including the terminating null character) to character array pointed to by dest.

If count is reached before the entire string src was copied, the resulting character array is not null-terminated.

If, after copying the terminating null character from src, count is not reached, additional null characters are written to dest until the total of count characters have been written. 
*/
char *strncpy(char *dest, const char *src, size_t n)
{
	char *d = dest;
	while( n && (*src!='\0') ){
		*d++ = *src++;
		n--;
	}
	while(n){
		*d++ = '\0';
		n--;
	}
	return dest;
}

/*
Finds the first occurrence of the character ch in the byte string pointed to by str.

The terminating null character is considered to be a part of the string.
*/
const char* strchr(const char* str, int ch ){
	const char* s = str;
	while( *s != (char)ch ){
		if(*s == '\0') return NULL;
		s++;
	}
	return s;
}
	
