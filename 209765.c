Jsi_RC Jsi_GetInt(Jsi_Interp* interp, const char *string, int *n, int base)
{
    char *endptr;
    if (base) {
        *n = strtoul(string, &endptr, base);
    }
    else {
        *n = (int)jsi_strtoul(string, &endptr);
    }
    return JsiCheckConversion(string, endptr);
}