acl_fetch_http_first_req(struct proxy *px, struct session *s, void *l7, int dir,
		     struct acl_expr *expr, struct acl_test *test)
{
	if (!s)
		return 0;

	if (s->txn.flags & TX_NOT_FIRST)
		test->flags |= ACL_TEST_F_SET_RES_FAIL;
	else
		test->flags |= ACL_TEST_F_SET_RES_PASS;

	return 1;
}