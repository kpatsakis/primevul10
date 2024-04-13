get_resource_id_quoted (WinResource *wr)
{
    static char tmp[WINRES_ID_MAXLEN+2];

    if (wr->numeric_id || wr->id[0] == '\0')
        return wr->id;

    sprintf(tmp, "'%s'", wr->id);
    return tmp;
}