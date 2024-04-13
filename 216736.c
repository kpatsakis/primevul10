set_cc_config(struct multi_instance *mi, struct buffer_list *cc_config)
{
    if (mi->cc_config)
    {
        buffer_list_free(mi->cc_config);
    }
    mi->cc_config = cc_config;
}