void CLua::_getregistry(lua_State *ls, const char *name)
{
    lua_pushstring(ls, name);
    lua_gettable(ls, LUA_REGISTRYINDEX);
}