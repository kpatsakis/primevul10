bool lua_datum::is_function() const
{
    LUA_CHECK_TYPE(lua_isfunction);
}