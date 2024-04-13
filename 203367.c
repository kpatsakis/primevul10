acl_fetch_meth(struct proxy *px, struct session *l4, void *l7, int dir,
               struct acl_expr *expr, struct acl_test *test)
{
	int meth;
	struct http_txn *txn = l7;

	if (!txn)
		return 0;

	if (txn->req.msg_state < HTTP_MSG_BODY)
		return 0;

	meth = txn->meth;
	test->i = meth;
	if (meth == HTTP_METH_OTHER) {
		if (txn->rsp.msg_state != HTTP_MSG_RPBEFORE)
			/* ensure the indexes are not affected */
			return 0;
		test->len = txn->req.sl.rq.m_l;
		test->ptr = txn->req.sol;
	}
	test->flags = ACL_TEST_F_READ_ONLY | ACL_TEST_F_VOL_1ST;
	return 1;
}