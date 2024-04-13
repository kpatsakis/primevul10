management_callback_n_clients(void *arg)
{
    struct multi_context *m = (struct multi_context *) arg;
    return m->n_clients;
}