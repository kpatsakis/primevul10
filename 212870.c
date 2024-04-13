void CLua::pushglobal(const string &name)
{
    vector<string> pieces = split_string(".", name);
    lua_State *ls(state());

    if (pieces.empty())
        lua_pushnil(ls);

    for (unsigned i = 0, size = pieces.size(); i < size; ++i)
    {
        if (!i)
            lua_getglobal(ls, pieces[i].c_str());
        else
        {
            if (lua_istable(ls, -1))
            {
                lua_pushstring(ls, pieces[i].c_str());
                lua_gettable(ls, -2);
                // Swap the value we just found with the table itself.
                lua_insert(ls, -2);
                // And remove the table.
                lua_pop(ls, 1);
            }
            else
            {
                // We expected a table here, but got something else. Fail.
                lua_pop(ls, 1);
                lua_pushnil(ls);
                break;
            }
        }
    }
}