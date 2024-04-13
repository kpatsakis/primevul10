
static inline __le32 ext4_encode_extra_time(struct timespec *time)
{
       return cpu_to_le32((sizeof(time->tv_sec) > 4 ?
			   time->tv_sec >> 32 : 0) |
			   ((time->tv_nsec << 2) & EXT4_NSEC_MASK));