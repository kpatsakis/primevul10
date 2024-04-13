lua_State *CLua::state()
{
    if (!_state)
        init_lua();
    return _state;
}