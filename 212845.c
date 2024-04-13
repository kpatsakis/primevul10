lua_datum::lua_datum(const lua_datum &o)
    : lua(o.lua), need_cleanup(true)
{
    set_from(o);
}