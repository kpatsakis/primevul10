static int _clua_dofile(lua_State *ls)
{
    const char *file = luaL_checkstring(ls, 1);
    if (!file)
        return 0;

    const int err = CLua::loadfile(ls, file, !CLua::is_managed_vm(ls));
    if (err)
        return lua_error(ls);

    lua_call(ls, 0, LUA_MULTRET);
    return lua_gettop(ls);
}