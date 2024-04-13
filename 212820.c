bool lua_datum::is_number() const
{
    LUA_CHECK_TYPE(lua_isnumber);
}