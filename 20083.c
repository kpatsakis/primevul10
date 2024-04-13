void cpu_exec_init_all(struct uc_struct *uc)
{
    /* The data structures we set up here depend on knowing the page size,
     * so no more changes can be made after this point.
     * In an ideal world, nothing we did before we had finished the
     * machine setup would care about the target page size, and we could
     * do this much later, rather than requiring board models to state
     * up front what their requirements are.
     */
    finalize_target_page_bits(uc);
    memory_map_init(uc);
    io_mem_init(uc);
}