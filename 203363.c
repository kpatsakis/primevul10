acl_fetch_stcode(struct proxy *px, struct session *l4, void *l7, int dir,
                 struct acl_expr *expr, struct acl_test *test)
{
	struct http_txn *txn = l7;
	char *ptr;
	int len;

	if (!txn)
		return 0;

	if (txn->rsp.msg_state < HTTP_MSG_BODY)
		return 0;

	len = txn->rsp.sl.st.c_l;
	ptr = txn->rsp.sol + txn->rsp.sl.st.c;

	test->i = __strl2ui(ptr, len);
	test->flags = ACL_TEST_F_VOL_1ST;
	return 1;
}