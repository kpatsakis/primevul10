compare_resource_id (WinResource *wr, const char *id)
{
    if (wr->numeric_id) {
        int32_t cmp1, cmp2;
        if (id[0] == '+')
            return false;
        if (id[0] == '-')
            id++;
        if (!parse_int32(wr->id, &cmp1) || !parse_int32(id, &cmp2) || cmp1 != cmp2)
            return false;
    } else {
        if (id[0] == '-')
            return false;
        if (id[0] == '+')
            id++;
        if (strcmp(wr->id, id))
            return false;
    }

    return true;
}