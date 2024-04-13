const lua_datum &lua_datum::operator = (const lua_datum &o)
{
    if (this != &o)
    {
        cleanup();
        set_from(o);
    }
    return *this;
}