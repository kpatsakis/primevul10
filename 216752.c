multi_instance_string(const struct multi_instance *mi, bool null, struct gc_arena *gc)
{
    if (mi)
    {
        struct buffer out = alloc_buf_gc(MULTI_PREFIX_MAX_LENGTH, gc);
        const char *cn = tls_common_name(mi->context.c2.tls_multi, true);

        if (cn)
        {
            buf_printf(&out, "%s/", cn);
        }
        buf_printf(&out, "%s", mroute_addr_print(&mi->real, gc));
        return BSTR(&out);
    }
    else if (null)
    {
        return NULL;
    }
    else
    {
        return "UNDEF";
    }
}