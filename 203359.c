static int acl_match_meth(struct acl_test *test, struct acl_pattern *pattern)
{
	int icase;

	if (test->i != pattern->val.i)
		return ACL_PAT_FAIL;

	if (test->i != HTTP_METH_OTHER)
		return ACL_PAT_PASS;

	/* Other method, we must compare the strings */
	if (pattern->len != test->len)
		return ACL_PAT_FAIL;

	icase = pattern->flags & ACL_PAT_F_IGNORE_CASE;
	if ((icase && strncasecmp(pattern->ptr.str, test->ptr, test->len) != 0) ||
	    (!icase && strncmp(pattern->ptr.str, test->ptr, test->len) != 0))
		return ACL_PAT_FAIL;
	return ACL_PAT_PASS;
}