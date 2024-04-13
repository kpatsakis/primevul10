multi_reap_new(int buckets_per_pass)
{
    struct multi_reap *mr;
    ALLOC_OBJ(mr, struct multi_reap);
    mr->bucket_base = 0;
    mr->buckets_per_pass = buckets_per_pass;
    mr->last_call = now;
    return mr;
}