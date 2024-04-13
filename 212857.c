void lua_datum::set_from(const lua_datum &o)
{
    lua_pushlightuserdata(lua, this);
    o.push();
    lua_settable(lua, LUA_REGISTRYINDEX);
    lua.add_shutdown_listener(this);
    need_cleanup = true;
}