static int lua_loadstring(lua_State *ls)
{
    const auto lua = luaL_checkstring(ls, 1);
    if (lua[0] == 0x1b)
        abort();
    lua_settop(ls, 0);
    if (luaL_loadstring(ls, lua))
    {
        lua_pushnil(ls);
        lua_insert(ls, 1);
    }
    return lua_gettop(ls);
}