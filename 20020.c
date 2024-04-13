static void mmio_region_destructor_uc(MemoryRegion *mr)
{
    g_free(mr->opaque);
}