strnEQx(const char* s1, const char* s2, STRLEN n, int ignore_case)
{
    while (n--) {
	if (ignore_case) {
	    if (toLOWER(*s1) != toLOWER(*s2))
		return 0;
	}
	else {
	    if (*s1 != *s2)
		return 0;
	}
	s1++;
	s2++;
    }
    return 1;
}