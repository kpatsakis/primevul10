void lua_text_pattern::post_pattern(string &pat, string &fn) const
{
    pat.erase(0, pat.find_first_not_of(" \t\n\r"));

    fn += " pmatch([[";
    fn += pat;
    fn += "]], text, false) ";

    pat.clear();
}