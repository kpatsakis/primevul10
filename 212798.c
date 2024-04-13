void lua_datum::push() const
{
    lua_pushlightuserdata(lua, const_cast<lua_datum*>(this));
    lua_gettable(lua, LUA_REGISTRYINDEX);

    // The value we saved is now on top of the Lua stack.
}