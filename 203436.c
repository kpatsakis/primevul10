acl_fetch_url(struct proxy *px, struct session *l4, void *l7, int dir,
              struct acl_expr *expr, struct acl_test *test)
{
	struct http_txn *txn = l7;

	if (!txn)
		return 0;

	if (txn->req.msg_state < HTTP_MSG_BODY)
		return 0;

	if (txn->rsp.msg_state != HTTP_MSG_RPBEFORE)
		/* ensure the indexes are not affected */
		return 0;

	test->len = txn->req.sl.rq.u_l;
	test->ptr = txn->req.sol + txn->req.sl.rq.u;

	/* we do not need to set READ_ONLY because the data is in a buffer */
	test->flags = ACL_TEST_F_VOL_1ST;
	return 1;
}