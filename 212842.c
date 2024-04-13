void CLua::add_shutdown_listener(lua_shutdown_listener *listener)
{
    if (find(shutdown_listeners.begin(), shutdown_listeners.end(), listener)
        == shutdown_listeners.end())
    {
        shutdown_listeners.push_back(listener);
    }
}