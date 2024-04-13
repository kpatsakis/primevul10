bool lua_datum::is_table() const
{
    LUA_CHECK_TYPE(lua_istable);
}