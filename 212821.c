string CLua::setuniqregistry()
{
    char name[100];
    snprintf(name, sizeof name, "__cru%u", uniqindex++);
    lua_pushstring(state(), name);
    lua_insert(state(), -2);
    lua_settable(state(), LUA_REGISTRYINDEX);

    return name;
}