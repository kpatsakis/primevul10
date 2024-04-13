bool CLua::calltopfn(lua_State *ls, const char *params, va_list args,
                     int retc, va_list *copyto)
{
    // We guarantee to remove the function from the stack
    int argc = push_args(ls, params, args, copyto);
    if (retc == -1)
        retc = return_count(ls, params);
    lua_call_throttle strangler(this);
    int err = lua_pcall(ls, argc, retc, 0);
    set_error(err, ls);
    return !err;
}