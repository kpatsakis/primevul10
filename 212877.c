FILE *CLua::CLuaSave::get_file()
{
    if (!handle)
        handle = fopen_u(filename, "w");

    return handle;
}