static int _clua_loadfile(lua_State *ls)
{
    const char *file = luaL_checkstring(ls, 1);
    if (!file)
        return 0;

    const int err = CLua::loadfile(ls, file, !CLua::is_managed_vm(ls));
    if (err)
    {
        const int place = lua_gettop(ls);
        lua_pushnil(ls);
        lua_insert(ls, place);
        return 2;
    }
    return 1;
}