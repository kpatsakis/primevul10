bool lua_text_pattern::is_lua_pattern(const string &s)
{
    return any_of(begin(pat_ops), end(pat_ops),
            [&s] (const lua_pat_op &op)
            { return s.find(op.token) != string::npos; });
}