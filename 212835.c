void CLua::load_persist()
{
    if (Options.no_save)
        return;
    string persistfile = _get_persist_file();
    if (!file_exists(persistfile))
        return;
    FileLineInput f(persistfile.c_str());
    string script;
    while (!f.eof())
        script += f.get_line() + "\n";
    execstring(script.c_str());
}