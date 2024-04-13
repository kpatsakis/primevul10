void CLua::save(writer &outf)
{
    if (!_state)
        return;

    string res;
    callfn("c_save", ">s", &res);
    outf.write(res.c_str(), res.size());
}