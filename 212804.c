bool lua_datum::is_string() const
{
    LUA_CHECK_TYPE(lua_isstring);
}