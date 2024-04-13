void CLua::init_throttle()
{
    if (!managed_vm)
        return;

    if (!crawl_state.throttle)
        return;

    if (throttle_unit_lines <= 0)
        throttle_unit_lines = 500;

    if (throttle_sleep_start < 1)
        throttle_sleep_start = 1;

    if (throttle_sleep_end < throttle_sleep_start)
        throttle_sleep_end = throttle_sleep_start;

    if (!mixed_call_depth)
    {
        lua_sethook(_state, _clua_throttle_hook,
                    LUA_MASKCOUNT, throttle_unit_lines);
        throttle_sleep_ms = 0;
        n_throttle_sleeps = 0;
    }
}