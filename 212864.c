bool lua_datum::is_udata() const
{
    LUA_CHECK_TYPE(lua_isuserdata);
}