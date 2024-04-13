void CLua::print_stack()
{
    struct lua_Debug dbg;
    int              i = 0;
    lua_State       *L = state();

    fprintf(stderr, "\n");
    while (lua_getstack(L, i++, &dbg) == 1)
    {
        lua_getinfo(L, "lnuS", &dbg);

        char* file = strrchr(dbg.short_src, '/');
        if (file == nullptr)
            file = dbg.short_src;
        else
            file++;

        fprintf(stderr, "%s, function %s, line %d\n", file,
                dbg.name, dbg.currentline);
    }

    fprintf(stderr, "\n");
}