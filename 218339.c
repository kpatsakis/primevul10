static void mark_eio(struct i915_request *rq)
{
	if (i915_request_completed(rq))
		return;

	GEM_BUG_ON(i915_request_signaled(rq));

	dma_fence_set_error(&rq->fence, -EIO);
	i915_request_mark_complete(rq);
}