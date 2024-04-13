static bool loongarch_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
    if (interrupt_request & CPU_INTERRUPT_HARD) {
        LoongArchCPU *cpu = LOONGARCH_CPU(cs);
        CPULoongArchState *env = &cpu->env;

        if (cpu_loongarch_hw_interrupts_enabled(env) &&
            cpu_loongarch_hw_interrupts_pending(env)) {
            /* Raise it */
            cs->exception_index = EXCCODE_INT;
            loongarch_cpu_do_interrupt(cs);
            return true;
        }
    }
    return false;
}