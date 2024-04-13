void cpu_exec_realizefn(CPUState *cpu)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    cc->tcg_initialize(cpu->uc);
    tlb_init(cpu);
}