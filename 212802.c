void CLua::remove_shutdown_listener(lua_shutdown_listener *listener)
{
    erase_val(shutdown_listeners, listener);
}