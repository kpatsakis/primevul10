bool CLua::runhook(const char *hook, const char *params, ...)
{
    error.clear();

    lua_State *ls = state();
    if (!ls)
        return false;

    lua_stack_cleaner clean(ls);

    pushglobal(hook);
    if (!lua_istable(ls, -1))
        return false;
    for (int i = 1; ; ++i)
    {
        lua_stack_cleaner clean2(ls);

        lua_rawgeti(ls, -1, i);
        if (!lua_isfunction(ls, -1))
        {
            lua_pop(ls, 1);
            break;
        }

        // So what's on top *is* a function. Call it with the args we have.
        va_list args;
        va_start(args, params);
        calltopfn(ls, params, args);
        va_end(args);
    }
    return true;
}