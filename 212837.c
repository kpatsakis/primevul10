maybe_bool CLua::callmbooleanfn(const char *fn, const char *params,
                                va_list args)
{
    error.clear();
    lua_State *ls = state();
    if (!ls)
        return MB_MAYBE;

    lua_stack_cleaner clean(ls);

    pushglobal(fn);
    if (!lua_isfunction(ls, -1))
        return MB_MAYBE;

    bool ret = calltopfn(ls, params, args, 1);
    if (!ret)
        return MB_MAYBE;

    return frombool(lua_toboolean(ls, -1));
}