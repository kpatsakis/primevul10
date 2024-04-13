generate_prefix(struct multi_instance *mi)
{
    struct gc_arena gc = gc_new();
    const char *prefix = multi_instance_string(mi, true, &gc);
    if (prefix)
    {
        strncpynt(mi->msg_prefix, prefix, sizeof(mi->msg_prefix));
    }
    else
    {
        mi->msg_prefix[0] = '\0';
    }
    set_prefix(mi);
    gc_free(&gc);
}