void CLua::fnreturns(const char *format, ...)
{
    lua_State *ls = _state;

    if (!format || !ls)
        return;

    va_list args;
    va_start(args, format);
    vfnreturns(format, args);
    va_end(args);
}