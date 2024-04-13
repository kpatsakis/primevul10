ofpacts_equal(const struct ofpact *a, size_t a_len,
              const struct ofpact *b, size_t b_len)
{
    return a_len == b_len && !memcmp(a, b, a_len);
}