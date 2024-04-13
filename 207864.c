const char *loongarch_exception_name(int32_t exception)
{
    assert(excp_names[exception]);
    return excp_names[exception];
}