int CLua::execstring(const char *s, const char *context, int nresults)
{
    int err = 0;
    if ((err = loadstring(s, context)))
        return err;

    lua_State *ls = state();
    lua_call_throttle strangler(this);
    err = lua_pcall(ls, 0, nresults, 0);
    set_error(err, ls);
    return err;
}