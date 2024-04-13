static inline void memory_region_shift_read_access(uint64_t *value,
                                                   signed shift,
                                                   uint64_t mask,
                                                   uint64_t tmp)
{
    if (shift >= 0) {
        *value |= (tmp & mask) << shift;
    } else {
        *value |= (tmp & mask) >> -shift;
    }
}