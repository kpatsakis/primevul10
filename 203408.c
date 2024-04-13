acl_fetch_url_ip(struct proxy *px, struct session *l4, void *l7, int dir,
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

	/* Parse HTTP request */
	url2sa(txn->req.sol + txn->req.sl.rq.u, txn->req.sl.rq.u_l, &l4->srv_addr);
	test->ptr = (void *)&((struct sockaddr_in *)&l4->srv_addr)->sin_addr;
	test->i = AF_INET;

	/*
	 * If we are parsing url in frontend space, we prepare backend stage
	 * to not parse again the same url ! optimization lazyness...
	 */
	if (px->options & PR_O_HTTP_PROXY)
		l4->flags |= SN_ADDR_SET;

	test->flags = ACL_TEST_F_READ_ONLY;
	return 1;
}