acl_fetch_hdr_val(struct proxy *px, struct session *l4, void *l7, char *sol,
                  struct acl_expr *expr, struct acl_test *test)
{
	struct http_txn *txn = l7;
	struct hdr_idx *idx = &txn->hdr_idx;
	struct hdr_ctx *ctx = (struct hdr_ctx *)test->ctx.a;

	if (!txn)
		return 0;

	if (!(test->flags & ACL_TEST_F_FETCH_MORE))
		/* search for header from the beginning */
		ctx->idx = 0;

	if (http_find_header2(expr->arg.str, expr->arg_len, sol, idx, ctx)) {
		test->flags |= ACL_TEST_F_FETCH_MORE;
		test->flags |= ACL_TEST_F_VOL_HDR;
		test->i = strl2ic((char *)ctx->line + ctx->val, ctx->vlen);
		return 1;
	}

	test->flags &= ~ACL_TEST_F_FETCH_MORE;
	test->flags |= ACL_TEST_F_VOL_HDR;
	return 0;
}