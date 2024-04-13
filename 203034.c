static void ipi_handler(void *null)
{
	flush_agp_cache();
}