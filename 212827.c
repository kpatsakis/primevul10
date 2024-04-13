lua_text_pattern::lua_text_pattern(const string &_pattern)
    : translated(false), isvalid(true), pattern(_pattern),
      lua_fn_name(new_fn_name())
{
}