void global_cache_flush(void)
{
	if (on_each_cpu(ipi_handler, NULL, 1) != 0)
		panic(PFX "timed out waiting for the other CPUs!\n");
}