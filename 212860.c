bool lua_text_pattern::valid() const
{
    return translated? isvalid : translate();
}