cin_isscopedecl(char_u *s)
{
    int		i;

    s = cin_skipcomment(s);
    if (STRNCMP(s, "public", 6) == 0)
	i = 6;
    else if (STRNCMP(s, "protected", 9) == 0)
	i = 9;
    else if (STRNCMP(s, "private", 7) == 0)
	i = 7;
    else
	return FALSE;
    return (*(s = cin_skipcomment(s + i)) == ':' && s[1] != ':');
}