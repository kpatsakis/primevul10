static inline void metadata_access_disable(void)
{
	kasan_enable_current();
}