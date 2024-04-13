acl_fetch_http_auth(struct proxy *px, struct session *s, void *l7, int dir,
		    struct acl_expr *expr, struct acl_test *test)
{

	if (!s)
		return 0;

	if (!get_http_auth(s))
		return 0;

	test->ctx.a[0] = expr->arg.ul;
	test->ctx.a[1] = s->txn.auth.user;
	test->ctx.a[2] = s->txn.auth.pass;

	test->flags |= ACL_TEST_F_READ_ONLY | ACL_TEST_F_NULL_MATCH;

	return 1;
}