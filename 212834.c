void CLua::setglobal(const char *name)
{
    lua_setglobal(state(), name);
}