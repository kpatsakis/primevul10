bool CLua::is_path_safe(string s, bool trusted)
{
    lowercase(s);
    return s.find("..") == string::npos && shell_safe(s.c_str())
           // loading dlua stuff would spew tons of error messages
           && (trusted || s.find("dlua") != 0);
}