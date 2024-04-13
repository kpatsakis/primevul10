multi_reap_process_dowork(const struct multi_context *m)
{
    struct multi_reap *mr = m->reaper;
    if (mr->bucket_base >= hash_n_buckets(m->vhash))
    {
        mr->bucket_base = 0;
    }
    multi_reap_range(m, mr->bucket_base, mr->bucket_base + mr->buckets_per_pass);
    mr->bucket_base += mr->buckets_per_pass;
    mr->last_call = now;
}