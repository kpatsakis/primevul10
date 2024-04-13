mi_prefix(const struct multi_instance *mi)
{
    if (mi && mi->msg_prefix[0])
    {
        return mi->msg_prefix;
    }
    else
    {
        return "UNDEF_I";
    }
}