int CLua::execfile(const char *filename, bool trusted, bool die_on_fail,
                   bool force)
{
    if (!force && sourced_files.count(filename))
        return 0;

    lua_State *ls = state();
    int err = loadfile(ls, filename, trusted || !managed_vm, die_on_fail);
    lua_call_throttle strangler(this);
    if (!err)
        err = lua_pcall(ls, 0, 0, 0);
    if (!err)
        sourced_files.insert(filename);
    set_error(err);
    if (die_on_fail && !error.empty())
    {
        end(1, false, "Lua execfile error (%s): %s",
            filename, error.c_str());
    }
    return err;
}