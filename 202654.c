vte_terminal_termcap_string_same_as_for (VteTerminal *terminal,
					 const char  *cap_str,
					 const char  *cap_other)
{
	char *other_str;
	gboolean ret;

	other_str = _vte_termcap_find_string(terminal->pvt->termcap,
					     terminal->pvt->emulation,
					     cap_other);

	ret = other_str && (g_ascii_strcasecmp(cap_str, other_str) == 0);

	g_free (other_str);

	return ret;
}