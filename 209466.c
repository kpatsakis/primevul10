Jsi_RC Jsi_GetWide(Jsi_Interp* interp, const char *string, Jsi_Wide *widePtr, int base)
{
    char *endptr;

    if (base) {
        *widePtr = strtoull(string, &endptr, base);
    }
    else {
        *widePtr = jsi_strtoull(string, &endptr);
    }

    return JsiCheckConversion(string, endptr);
}