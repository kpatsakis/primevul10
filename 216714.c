multi_signal_instance(struct multi_context *m, struct multi_instance *mi, const int sig)
{
    mi->context.sig->signal_received = sig;
    multi_close_instance_on_signal(m, mi);
}