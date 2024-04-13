AP_CORE_DECLARE(void) ap_random_parent_after_fork(void)
{
    /*
     * To ensure that the RNG state in the parent changes after the fork, we
     * pull some data from the RNG and discard it. This ensures that the RNG
     * states in the children are different even after the pid wraps around.
     * As we only use apr_random for insecure random bytes, pulling 2 bytes
     * should be enough.
     * XXX: APR should probably have some dedicated API to do this, but it
     * XXX: currently doesn't.
     */
    apr_uint16_t data;
    apr_random_insecure_bytes(rng, &data, sizeof(data));
}