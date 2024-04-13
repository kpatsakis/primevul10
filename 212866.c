bool CLua::callfn(const char *fn, int nargs, int nret)
{
    error.clear();
    lua_State *ls = state();
    if (!ls)
        return false;

    // If a function is not provided on the stack, get the named function.
    if (fn)
    {
        pushglobal(fn);
        if (!lua_isfunction(ls, -1))
        {
            lua_settop(ls, -nargs - 2);
            return false;
        }

        // Slide the function in front of its args and call it.
        if (nargs)
            lua_insert(ls, -nargs - 1);
    }

    lua_call_throttle strangler(this);
    int err = lua_pcall(ls, nargs, nret, 0);
    set_error(err, ls);
    return !err;
}