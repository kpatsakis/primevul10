ins_compl_equal(compl_T *match, char_u *str, int len)
{
    if (match->cp_flags & CP_EQUAL)
	return TRUE;
    if (match->cp_flags & CP_ICASE)
	return STRNICMP(match->cp_str, str, (size_t)len) == 0;
    return STRNCMP(match->cp_str, str, (size_t)len) == 0;
}