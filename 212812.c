CLua::CLua(bool managed)
    : error(), managed_vm(managed), shutting_down(false),
      throttle_unit_lines(50000),
      throttle_sleep_ms(0), throttle_sleep_start(2),
      throttle_sleep_end(800), n_throttle_sleeps(0), mixed_call_depth(0),
      lua_call_depth(0), max_mixed_call_depth(8),
      max_lua_call_depth(100), memory_used(0),
      _state(nullptr), sourced_files(), uniqindex(0)
{
}