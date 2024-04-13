ungenerate_prefix(struct multi_instance *mi)
{
    mi->msg_prefix[0] = '\0';
    set_prefix(mi);
}