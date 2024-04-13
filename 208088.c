pam_trace_iname(int item_type, char *iname_buf)
{
	char *name;

	if (item_type <= 0 ||
	    item_type >= PAM_MAX_ITEMS ||
	    (name = pam_inames[item_type]) == NULL) {
		(void) sprintf(iname_buf, "%d", item_type);
		return (iname_buf);
	}
	return (name);
}