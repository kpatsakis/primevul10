multi_top_free(struct multi_context *m)
{
    close_context(&m->top, -1, CC_GC_FREE);
    free_context_buffers(m->top.c2.buffers);
}