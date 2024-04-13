void CLua::getglobal(const char *name)
{
    lua_getglobal(state(), name);
}