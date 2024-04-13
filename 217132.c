find_line_end(char_u *ptr)
{
    char_u	*s;

    s = ptr + STRLEN(ptr);
    while (s > ptr && (s[-1] == CAR || s[-1] == NL))
	--s;
    return s;
}