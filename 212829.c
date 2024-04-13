maybe_bool CLua::callmaybefn(const char *fn, const char *params, va_list args)
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

    return lua_isboolean(ls, -1) ? frombool(lua_toboolean(ls, -1)) : MB_MAYBE;
}