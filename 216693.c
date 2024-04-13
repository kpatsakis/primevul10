multi_top_init(struct multi_context *m, const struct context *top)
{
    inherit_context_top(&m->top, top);
    m->top.c2.buffers = init_context_buffers(&top->c2.frame);
}