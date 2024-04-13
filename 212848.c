int CLua::loadbuffer(const char *buf, size_t size, const char *context)
{
    const int err = luaL_loadbuffer(state(), buf, size, context);
    set_error(err, state());
    return err;
}