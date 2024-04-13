CLua &CLua::get_vm(lua_State *ls)
{
    lua_stack_cleaner clean(ls);
    _getregistry(ls, "__clua");
    CLua *vm = clua_get_lightuserdata<CLua>(ls, -1);
    if (!vm)
        luaL_error(ls, "Could not find matching clua for lua state");
    return *vm;
}