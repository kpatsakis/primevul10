void CLua::gc()
{
    lua_gc(state(), LUA_GCCOLLECT, 0);
}