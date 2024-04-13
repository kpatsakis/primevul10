acl_fetch_chdr_ip(struct proxy *px, struct session *l4, void *l7, int dir,
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

	return acl_fetch_hdr_ip(px, l4, txn, txn->req.sol, expr, test);
}