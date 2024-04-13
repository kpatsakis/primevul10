management_client_pf(void *arg,
                     const unsigned long cid,
                     struct buffer_list *pf_config)  /* ownership transferred */
{
    struct multi_context *m = (struct multi_context *) arg;
    struct multi_instance *mi = lookup_by_cid(m, cid);
    bool ret = false;

    if (mi && pf_config)
    {
        ret = pf_load_from_buffer_list(&mi->context, pf_config);
    }

    if (pf_config)
    {
        buffer_list_free(pf_config);
    }
    return ret;
}