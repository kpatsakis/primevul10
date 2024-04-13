bool lua_text_pattern::translate() const
{
    if (translated || !isvalid)
        return false;

    if (pattern.find("]]") != string::npos || pattern.find("[[") != string::npos)
        return false;

    string textp;
    string luafn;
    const lua_pat_op *currop = nullptr;
    for (string::size_type i = 0; i < pattern.length(); ++i)
    {
        bool match = false;
        for (unsigned p = 0; p < ARRAYSZ(pat_ops); ++p)
        {
            const lua_pat_op &lop = pat_ops[p];
            if (pattern.find(lop.token, i) == i)
            {
                match = true;
                if (lop.pretext && (!currop || currop->posttext))
                {
                    if (currop)
                        textp.erase(0, textp.find_first_not_of(" \r\n\t"));
                    pre_pattern(textp, luafn);
                }

                currop = &lop;
                luafn += lop.luatok;

                i += strlen(lop.token) - 1;

                break;
            }
        }

        if (match)
            continue;

        textp += pattern[i];
    }

    if (currop && currop->posttext)
        post_pattern(textp, luafn);

    luafn = "function " + lua_fn_name + "(text) return " + luafn + " end";

    const_cast<lua_text_pattern *>(this)->translated = true;

    int err = clua.execstring(luafn.c_str(), "stash-search");
    if (err)
    {
        lua_text_pattern *self = const_cast<lua_text_pattern *>(this);
        self->isvalid = self->translated = false;
    }

    return translated;
}