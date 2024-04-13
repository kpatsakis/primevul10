acl_fetch_hdr_cnt(struct proxy *px, struct session *l4, void *l7, char *sol,
                  struct acl_expr *expr, struct acl_test *test)
{
	struct http_txn *txn = l7;
	struct hdr_idx *idx = &txn->hdr_idx;
	struct hdr_ctx ctx;
	int cnt;

	if (!txn)
		return 0;

	ctx.idx = 0;
	cnt = 0;
	while (http_find_header2(expr->arg.str, expr->arg_len, sol, idx, &ctx))
		cnt++;

	test->i = cnt;
	test->flags = ACL_TEST_F_VOL_HDR;
	return 1;
}