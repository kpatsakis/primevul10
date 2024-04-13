cid_hash_function(const void *key, uint32_t iv)
{
    const unsigned long *k = (const unsigned long *)key;
    return (uint32_t) *k;
}