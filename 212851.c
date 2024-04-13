lua_call_throttle::~lua_call_throttle()
{
    if (!--lua->mixed_call_depth)
        lua_map.erase(lua->state());
}