management_callback_status(void *arg, const int version, struct status_output *so)
{
    struct multi_context *m = (struct multi_context *) arg;

    if (!version)
    {
        multi_print_status(m, so, m->status_file_version);
    }
    else
    {
        multi_print_status(m, so, version);
    }
}