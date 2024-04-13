int CLua::return_count(lua_State *ls, const char *format)
{
    UNUSED(ls);

    if (!format)
        return 0;

    const char *gs = strchr(format, '>');
    if (gs)
        return strlen(gs + 1);

    const char *cs = strchr(format, ':');
    if (cs && isdigit(*format))
    {
        char *es = nullptr;
        int ci = strtol(format, &es, 10);
        // We're capping return at 10 here, which is arbitrary, but avoids
        // blowing the stack.
        if (ci < 0)
            ci = 0;
        else if (ci > 10)
            ci = 10;
        return ci;
    }
    return 0;
}