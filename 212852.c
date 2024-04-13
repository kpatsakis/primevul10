static int _clua_require(lua_State *ls)
{
    const char *file = luaL_checkstring(ls, 1);
    if (!file)
        return 0;

    CLua &vm(CLua::get_vm(ls));
    if (vm.execfile(file, false, false) != 0)
        luaL_error(ls, vm.error.c_str());

    lua_pushboolean(ls, true);
    return 1;
}