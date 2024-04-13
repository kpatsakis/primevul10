acl_fetch_url_port(struct proxy *px, struct session *l4, void *l7, int dir,
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

	/* Same optimization as url_ip */
	url2sa(txn->req.sol + txn->req.sl.rq.u, txn->req.sl.rq.u_l, &l4->srv_addr);
	test->i = ntohs(((struct sockaddr_in *)&l4->srv_addr)->sin_port);

	if (px->options & PR_O_HTTP_PROXY)
		l4->flags |= SN_ADDR_SET;

	test->flags = ACL_TEST_F_READ_ONLY;
	return 1;
}