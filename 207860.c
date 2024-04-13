static void loongarch_cpu_init(Object *obj)
{
    LoongArchCPU *cpu = LOONGARCH_CPU(obj);

    cpu_set_cpustate_pointers(cpu);

#ifndef CONFIG_USER_ONLY
    CPULoongArchState *env = &cpu->env;
    qdev_init_gpio_in(DEVICE(cpu), loongarch_cpu_set_irq, N_IRQS);
    timer_init_ns(&cpu->timer, QEMU_CLOCK_VIRTUAL,
                  &loongarch_constant_timer_cb, cpu);
    memory_region_init_io(&env->system_iocsr, OBJECT(cpu), NULL,
                      env, "iocsr", UINT64_MAX);
    address_space_init(&env->address_space_iocsr, &env->system_iocsr, "IOCSR");
    memory_region_init_io(&env->iocsr_mem, OBJECT(cpu), &loongarch_qemu_ops,
                          NULL, "iocsr_misc", 0x428);
    memory_region_add_subregion(&env->system_iocsr, 0, &env->iocsr_mem);
#endif
}