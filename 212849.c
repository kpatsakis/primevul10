string quote_lua_string(const string &s)
{
    return replace_all_of(replace_all_of(s, "\\", "\\\\"), "\"", "\\\"");
}