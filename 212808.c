int CLua::file_write(lua_State *ls)
{
    if (!lua_islightuserdata(ls, 1))
    {
        luaL_argerror(ls, 1, "Expected filehandle at arg 1");
        return 0;
    }
    CLuaSave *sf = static_cast<CLuaSave *>(lua_touserdata(ls, 1));
    if (!sf)
        return 0;

    FILE *f = sf->get_file();
    if (!f)
        return 0;

    const char *text = luaL_checkstring(ls, 2);
    if (text)
        fprintf(f, "%s", text);
    return 0;
}