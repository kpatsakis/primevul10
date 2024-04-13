lua_call_throttle::lua_call_throttle(CLua *_lua)
    : lua(_lua)
{
    lua->init_throttle();
    if (!lua->mixed_call_depth++)
        lua_map[lua->state()] = lua;
}