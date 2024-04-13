static int _clua_guarded_pcall(lua_State *ls)
{
    const int nargs = lua_gettop(ls);
    const int err = lua_pcall(ls, nargs - 1, LUA_MULTRET, 0);

    if (err)
    {
        const char *errs = lua_tostring(ls, 1);
        if (!errs || strstr(errs, BUGGY_SCRIPT_ERROR))
            luaL_error(ls, errs? errs : BUGGY_PCALL_ERROR);
    }

    lua_pushboolean(ls, !err);
    lua_insert(ls, 1);

    return lua_gettop(ls);
}