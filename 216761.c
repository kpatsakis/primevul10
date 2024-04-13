cid_compare_function(const void *key1, const void *key2)
{
    const unsigned long *k1 = (const unsigned long *)key1;
    const unsigned long *k2 = (const unsigned long *)key2;
    return *k1 == *k2;
}