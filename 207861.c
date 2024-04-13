static void loongarch_cpu_class_init(ObjectClass *c, void *data)
{
    LoongArchCPUClass *lacc = LOONGARCH_CPU_CLASS(c);
    CPUClass *cc = CPU_CLASS(c);
    DeviceClass *dc = DEVICE_CLASS(c);

    device_class_set_parent_realize(dc, loongarch_cpu_realizefn,
                                    &lacc->parent_realize);
    device_class_set_parent_reset(dc, loongarch_cpu_reset, &lacc->parent_reset);

    cc->class_by_name = loongarch_cpu_class_by_name;
    cc->has_work = loongarch_cpu_has_work;
    cc->dump_state = loongarch_cpu_dump_state;
    cc->set_pc = loongarch_cpu_set_pc;
#ifndef CONFIG_USER_ONLY
    dc->vmsd = &vmstate_loongarch_cpu;
    cc->sysemu_ops = &loongarch_sysemu_ops;
#endif
    cc->disas_set_info = loongarch_cpu_disas_set_info;
    cc->gdb_read_register = loongarch_cpu_gdb_read_register;
    cc->gdb_write_register = loongarch_cpu_gdb_write_register;
    cc->disas_set_info = loongarch_cpu_disas_set_info;
    cc->gdb_num_core_regs = 34;
    cc->gdb_core_xml_file = "loongarch-base64.xml";
    cc->gdb_stop_before_watchpoint = true;

#ifdef CONFIG_TCG
    cc->tcg_ops = &loongarch_tcg_ops;
#endif
}