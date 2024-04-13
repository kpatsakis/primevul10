bool lua_text_pattern::matches(const string &s) const
{
    if (isvalid && !translated)
        translate();

    if (!isvalid)
        return false;

    return clua.callbooleanfn(false, lua_fn_name.c_str(), "s", s.c_str());
}