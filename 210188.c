static Jsi_Wide jsi_strtoull(const char *str, char **endptr)
{
#ifdef JSI__LONG_LONG
    int sign;
    int base;
    int i = JsiNumberBase(str, &base, &sign);

    if (base != 10) {
        Jsi_Wide value = strtoull(str + i, endptr, base);
        if (endptr == NULL || *endptr != str + i) {
            return value * sign;
        }
    }

    /* Can just do a regular base-10 conversion */
    return strtoull(str, endptr, 10);
#else
    return (unsigned long)jsi_strtol(str, endptr);
#endif
}