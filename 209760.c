Jsi_RC Jsi_GetDouble(Jsi_Interp* interp, const char *string, Jsi_Number *n)
{
    char *endptr;

    /* Callers can check for underflow via ERANGE */
    errno = 0;

    *n = strtod(string, &endptr);

    return JsiCheckConversion(string, endptr);
}