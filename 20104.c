AddressSpace *cpu_get_address_space(CPUState *cpu, int asidx)
{
    /* only one AddressSpace. */
    return cpu->cpu_ases[0].as;
}