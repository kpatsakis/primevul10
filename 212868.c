bool CLua::is_managed_vm(lua_State *ls)
{
    lua_stack_cleaner clean(ls);
    lua_pushstring(ls, "lua_vm_is_managed");
    lua_gettable(ls, LUA_REGISTRYINDEX);
    return lua_toboolean(ls, -1);
}