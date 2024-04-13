static void breakpoint_invalidate(CPUState *cpu, target_ulong pc)
{
    /*
     * There may not be a virtual to physical translation for the pc
     * right now, but there may exist cached TB for this pc.
     * Flush the whole TB cache to force re-translation of such TBs.
     * This is heavyweight, but we're debugging anyway.
     */
    tb_flush(cpu);
}