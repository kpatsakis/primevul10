static inline void metadata_access_enable(void)
{
	kasan_disable_current();
}