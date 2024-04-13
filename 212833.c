void CLua::init_lua()
{
    if (_state)
        return;

#ifdef NO_CUSTOM_ALLOCATOR
    // If this is likely to be used as a server, warn the builder.
    // NOTE: #warning doesn't work on MSVC, so this will be fatal there
    // (not that webtiles or dgamelaunch are supported on Windows anyway).
# if defined(USE_TILE_WEB) || defined(DGAMELAUNCH)
#   warning Detected 64-bit Luajit, disabling CLua memory throttling.
# endif
    _state = luaL_newstate();
#else
    // Throttle memory usage in managed (clua) VMs
    _state = managed_vm? lua_newstate(_clua_allocator, this) : luaL_newstate();
#endif
    if (!_state)
        end(1, false, "Unable to create Lua state.");

    lua_stack_cleaner clean(_state);

    lua_atpanic(_state, _clua_panic);

#ifdef CLUA_UNRESTRICTED_LIBS
    // open all libs -- this is not safe for public servers or releases!
    // Intended for people writing bots and the like.
    luaL_openlibs(_state);
#else
    // Selectively load some, but not all Lua core libraries.
    //
    // In Lua 5.1, these library setup calls are not supposed to be called
    // directly from C. If the lua version changes, this may need to be changed:
    // recommended practice is (apparently) checking the lua version's linit.cc
    // and seeing how that does the full library setup.
    //
    // This doesn't seem to *obviously* impact the libraries we use by default,
    // but some of the libraries we don't use will panic if not called
    // correctly; since someone writing a bot (for example) might want to
    // expand this, do things "correctly". The core lua libraries in 5.1 we are
    // not loading are:
    //
    // {LUA_LOADLIBNAME, luaopen_package},    // (require etc)
    // {LUA_IOLIBNAME, luaopen_io},           //
    // {LUA_OSLIBNAME, luaopen_os},
    // {LUA_DBLIBNAME, luaopen_debug},
    const vector<pair<string, lua_CFunction>> lua_core_libs =
    {
        {"", luaopen_base}, // XX: why no name? but this is how linit.cc does it
        {LUA_TABLIBNAME, luaopen_table},
        {LUA_STRLIBNAME, luaopen_string},
        {LUA_MATHLIBNAME, luaopen_math},
    };

    for (auto l : lua_core_libs)
    {
        lua_pushcfunction(_state, l.second);
        lua_pushstring(_state, l.first.c_str());
        lua_call(_state, 1, 0);
    }
#endif

    lua_pushboolean(_state, managed_vm);
    setregistry("lua_vm_is_managed");

    lua_pushlightuserdata(_state, this);
    setregistry("__clua");
}