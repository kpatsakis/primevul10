acl_fetch_rqver(struct proxy *px, struct session *l4, void *l7, int dir,
                struct acl_expr *expr, struct acl_test *test)
{
	struct http_txn *txn = l7;
	char *ptr;
	int len;

	if (!txn)
		return 0;

	if (txn->req.msg_state < HTTP_MSG_BODY)
		return 0;

	len = txn->req.sl.rq.v_l;
	ptr = txn->req.sol + txn->req.sl.rq.v;

	while ((len-- > 0) && (*ptr++ != '/'));
	if (len <= 0)
		return 0;

	test->ptr = ptr;
	test->len = len;

	test->flags = ACL_TEST_F_READ_ONLY | ACL_TEST_F_VOL_1ST;
	return 1;
}