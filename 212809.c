void lua_datum::cleanup()
{
    if (need_cleanup)
    {
        need_cleanup = false;
        lua.remove_shutdown_listener(this);

        lua_pushlightuserdata(lua, this);
        lua_pushnil(lua);
        lua_settable(lua, LUA_REGISTRYINDEX);
    }
}