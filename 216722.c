lookup_by_cid(struct multi_context *m, const unsigned long cid)
{
    if (m)
    {
        struct multi_instance *mi = (struct multi_instance *) hash_lookup(m->cid_hash, &cid);
        if (mi && !mi->halt)
        {
            return mi;
        }
    }
    return NULL;
}