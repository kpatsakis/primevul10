multi_close_instance_on_signal(struct multi_context *m, struct multi_instance *mi)
{
    remap_signal(&mi->context);
    set_prefix(mi);
    print_signal(mi->context.sig, "client-instance", D_MULTI_LOW);
    clear_prefix();
    multi_close_instance(m, mi, false);
}