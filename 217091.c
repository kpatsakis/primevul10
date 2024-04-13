file_is_tempfile(i_ctx_t *i_ctx_p, const uchar *fname, int len)
{
    ref *SAFETY;
    ref *tempfiles;
    ref kname;

    if (dict_find_string(systemdict, "SAFETY", &SAFETY) <= 0 ||
            dict_find_string(SAFETY, "tempfiles", &tempfiles) <= 0)
        return false;
    if (name_ref(imemory, fname, len, &kname, -1) < 0 ||
            dict_find(tempfiles, &kname, &SAFETY) <= 0)
        return false;
    return true;
}