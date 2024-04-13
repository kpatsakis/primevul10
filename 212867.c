CLua *lua_call_throttle::find_clua(lua_State *ls)
{
    return lookup(lua_map, ls, nullptr);
}