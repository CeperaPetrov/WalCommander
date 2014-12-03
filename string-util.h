#ifndef STRING_UTIL_H
#define STRING_UTIL_H

template<class T> inline const T* find_right_char(const T *s, T c)
{
	const T* p = 0;
	if (s) 
		for (;*s;s++) if (*s == c) p = s;
	return p;
}


inline carray<wchar_t> UnicodeToUtf16(const unicode_t *s)
{
	if (!s) return carray<wchar_t>();
	carray<wchar_t> p(unicode_strlen(s)+1);
	wchar_t *d;
	for (d = p.ptr(); *s; s++, d++) *d = *s;
	*d=0;
	return p;
}

inline carray<unicode_t> Utf16ToUnicode(const wchar_t *s)
{
	carray<unicode_t> p(wcslen(s)+1);
	unicode_t*d;
	for (d = p.ptr(); *s; s++, d++) *d = *s;
	*d=0;
	return p;
}

inline carray<wchar_t> new_wchar_str(const wchar_t *str)
{
	if (!str) return carray<wchar_t>();
	int l = 0; 
	for (const wchar_t *s = str; *s; s++) l++;
	carray<wchar_t> p(l+1);
	wchar_t *t;
	for (t = p.ptr(); *str; t++, str++) *t = *str;
	*t = 0;
	return p;
}


template <class T> inline  int carray_len(const T*s)
{
	for (int i = 0; ; i++)	if (!*(s++)) return i;
}

#define X(i) int n ## i = a ## i ? carray_len<T>(a ## i) : 0;
#define CP(i) if (n ## i) { for ( ;* a ## i; a ## i++) *(s++) = *(a ## i); }

template <class T> inline carray<T> carray_cat(const T *a1, const T *a2)
{
	X(1); X(2);
	carray<T> str(n1+n2+1); T *s = str.ptr();
	CP(1); CP(2); *s=0;
	return str;
}

template <class T> inline  carray<T> carray_cat(const T *a1, const T *a2, const T *a3)
{
	X(1); X(2); X(3);
	carray<T> str(n1+n2+n3+1); T *s = str.ptr();
	CP(1); CP(2); CP(3); *s=0;
	return str;
}

template <class T> inline  carray<T> carray_cat(const T *a1, const T *a2, const T *a3, const T *a4)
{
	X(1); X(2); X(3); X(4);
	carray<T> str(n1+n2+n3+n4+1); T *s = str.ptr();
	CP(1); CP(2); CP(3); CP(4);  *s=0;
	return str;

}

template <class T> inline  carray<T> carray_cat(const T *a1, const T *a2, const T *a3, const T *a4, const T *a5)
{
	X(1); X(2); X(3); X(4); X(5);
	carray<T> str(n1+n2+n3+n4+n5+1);	T *s = str.ptr();
	CP(1); CP(2); CP(3); CP(4); CP(5); *s=0;
	return str;
}

template <class T> inline  carray<T> carray_cat(const T *a1, const T *a2, const T *a3, const T *a4, const T *a5, const T *a6)
{
	X(1); X(2); X(3); X(4); X(5); X(6);
	carray<T> str(n1+n2+n3+n4+n5+n6+1);	T *s = str.ptr();
	CP(1); CP(2); CP(3); CP(4); CP(5); CP(6); *s=0;
	return str;
}

template <class T> inline  carray<T> carray_cat(const T *a1, const T *a2, const T *a3, const T *a4, const T *a5, const T *a6, const T *a7)
{
	X(1); X(2); X(3); X(4); X(5); X(6); X(7)
	carray<T> str(n1+n2+n3+n4+n5+n6+n7+1);	T *s = str.ptr();
	CP(1); CP(2); CP(3); CP(4); CP(5); CP(6); CP(7); *s=0;
	return str;
}


#undef X
#undef CP


#endif
