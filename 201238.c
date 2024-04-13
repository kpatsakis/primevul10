
static inline void ext4_decode_extra_time(struct timespec *time, __le32 extra)
{
       if (sizeof(time->tv_sec) > 4)
	       time->tv_sec |= (__u64)(le32_to_cpu(extra) & EXT4_EPOCH_MASK)
			       << 32;
       time->tv_nsec = (le32_to_cpu(extra) & EXT4_NSEC_MASK) >> 2;