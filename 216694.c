id(struct multi_instance *mi)
{
    if (mi)
    {
        return tls_common_name(mi->context.c2.tls_multi, false);
    }
    else
    {
        return "NULL";
    }
}