bool CLua::callbooleanfn(bool def, const char *fn, const char *params, ...)
{
    va_list args;
    va_start(args, params);
    maybe_bool r = callmbooleanfn(fn, params, args);
    va_end(args);
    return tobool(r, def);
}