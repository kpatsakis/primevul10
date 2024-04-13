lua_datum::lua_datum(CLua &_lua, int stackpos, bool pop)
    : lua(_lua), need_cleanup(true)
{
    // Store the datum in the registry indexed by "this".
    lua_pushvalue(lua, stackpos);
    lua_pushlightuserdata(lua, this);
    // Move the key (this) before the value.
    lua_insert(lua, -2);
    lua_settable(lua, LUA_REGISTRYINDEX);

    if (pop && stackpos < 0)
        lua_pop(lua, -stackpos);

    lua.add_shutdown_listener(this);
}