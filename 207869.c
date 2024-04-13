static void loongarch_cpu_do_transaction_failed(CPUState *cs, hwaddr physaddr,
                                                vaddr addr, unsigned size,
                                                MMUAccessType access_type,
                                                int mmu_idx, MemTxAttrs attrs,
                                                MemTxResult response,
                                                uintptr_t retaddr)
{
    LoongArchCPU *cpu = LOONGARCH_CPU(cs);
    CPULoongArchState *env = &cpu->env;

    if (access_type == MMU_INST_FETCH) {
        do_raise_exception(env, EXCCODE_ADEF, retaddr);
    } else {
        do_raise_exception(env, EXCCODE_ADEM, retaddr);
    }
}