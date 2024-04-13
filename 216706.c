reap_buckets_per_pass(int n_buckets)
{
    return constrain_int(n_buckets / REAP_DIVISOR, REAP_MIN, REAP_MAX);
}