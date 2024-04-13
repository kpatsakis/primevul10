static inline bool cpu_loongarch_hw_interrupts_enabled(CPULoongArchState *env)
{
    bool ret = 0;

    ret = (FIELD_EX64(env->CSR_CRMD, CSR_CRMD, IE) &&
          !(FIELD_EX64(env->CSR_DBG, CSR_DBG, DST)));

    return ret;
}