static bool loongarch_cpu_has_work(CPUState *cs)
{
#ifdef CONFIG_USER_ONLY
    return true;
#else
    LoongArchCPU *cpu = LOONGARCH_CPU(cs);
    CPULoongArchState *env = &cpu->env;
    bool has_work = false;

    if ((cs->interrupt_request & CPU_INTERRUPT_HARD) &&
        cpu_loongarch_hw_interrupts_pending(env)) {
        has_work = true;
    }

    return has_work;
#endif
}