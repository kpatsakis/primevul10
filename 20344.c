static struct io_kiocb *io_alloc_req(struct io_ring_ctx *ctx)
{
	struct io_submit_state *state = &ctx->submit_state;

	BUILD_BUG_ON(ARRAY_SIZE(state->reqs) < IO_REQ_ALLOC_BATCH);

	if (!state->free_reqs) {
		gfp_t gfp = GFP_KERNEL | __GFP_NOWARN;
		int ret, i;

		if (io_flush_cached_reqs(ctx))
			goto got_req;

		ret = kmem_cache_alloc_bulk(req_cachep, gfp, IO_REQ_ALLOC_BATCH,
					    state->reqs);

		/*
		 * Bulk alloc is all-or-nothing. If we fail to get a batch,
		 * retry single alloc to be on the safe side.
		 */
		if (unlikely(ret <= 0)) {
			state->reqs[0] = kmem_cache_alloc(req_cachep, gfp);
			if (!state->reqs[0])
				return NULL;
			ret = 1;
		}

		/*
		 * Don't initialise the fields below on every allocation, but
		 * do that in advance and keep valid on free.
		 */
		for (i = 0; i < ret; i++) {
			struct io_kiocb *req = state->reqs[i];

			req->ctx = ctx;
			req->link = NULL;
			req->async_data = NULL;
			/* not necessary, but safer to zero */
			req->result = 0;
		}
		state->free_reqs = ret;
	}
got_req:
	state->free_reqs--;
	return state->reqs[state->free_reqs];
}