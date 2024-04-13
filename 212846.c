maybe_bool CLua::callmaybefn(const char *fn, const char *params, ...)
{
    va_list args;
    va_start(args, params);
    maybe_bool r = callmaybefn(fn, params, args);
    va_end(args);
    return r;
}