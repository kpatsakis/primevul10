static void loongarch_cpu_realizefn(DeviceState *dev, Error **errp)
{
    CPUState *cs = CPU(dev);
    LoongArchCPUClass *lacc = LOONGARCH_CPU_GET_CLASS(dev);
    Error *local_err = NULL;

    cpu_exec_realizefn(cs, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }

    loongarch_cpu_register_gdb_regs_for_features(cs);

    cpu_reset(cs);
    qemu_init_vcpu(cs);

    lacc->parent_realize(dev, errp);
}