static inline bool cpu_loongarch_hw_interrupts_pending(CPULoongArchState *env)
{
    uint32_t pending;
    uint32_t status;
    bool r;

    pending = FIELD_EX64(env->CSR_ESTAT, CSR_ESTAT, IS);
    status  = FIELD_EX64(env->CSR_ECFG, CSR_ECFG, LIE);

    r = (pending & status) != 0;
    return r;
}