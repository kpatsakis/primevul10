 */
static void io_submit_state_end(struct io_submit_state *state,
				struct io_ring_ctx *ctx)
{
	if (state->link.head)
		io_queue_sqe(state->link.head);
	if (state->comp.nr)
		io_submit_flush_completions(ctx);
	if (state->plug_started)
		blk_finish_plug(&state->plug);
	io_state_file_put(state);