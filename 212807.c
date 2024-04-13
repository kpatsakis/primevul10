bool CLua::proc_returns(const char *par) const
{
    return strchr(par, '>') != nullptr;
}