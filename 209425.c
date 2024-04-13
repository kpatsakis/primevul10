Jsi_OptionCustom* Jsi_OptionCustomBuiltin(Jsi_OptionCustom* cust) {
    if ((uintptr_t)cust < (uintptr_t)(sizeof(custOpts)/sizeof(custOpts[0])))
        return custOpts[(uintptr_t)cust];
    return cust;
}