static bool checkreturn find_extension_field(pb_field_iter_t *iter)
{
    const pb_field_t *start = iter->pos;
    
    do {
        if (PB_LTYPE(iter->pos->type) == PB_LTYPE_EXTENSION)
            return true;
        (void)pb_field_iter_next(iter);
    } while (iter->pos != start);
    
    return false;
}