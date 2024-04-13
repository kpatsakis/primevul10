    yaffscache_objects_stats(YAFFSFS_INFO *yfs, 
    unsigned int *obj_count,
    uint32_t *obj_first, uint32_t *obj_last,
    uint32_t *version_count,
    uint32_t *version_first, uint32_t *version_last)
{
    YaffsCacheObject *obj;
    YaffsCacheVersion *ver;

    /* deleted and unlinked special objects don't have headers */
    *obj_count = 2;
    *obj_first = 0xffffffff;
    *obj_last = 0;

    *version_count = 0;
    *version_first = 0xffffffff;
    *version_last = 0;

    for(obj = yfs->cache_objects; obj != NULL; obj = obj->yco_next) {
        *obj_count += 1;
        if (obj->yco_obj_id < *obj_first)
            *obj_first = obj->yco_obj_id;
        if (obj->yco_obj_id > *obj_last)
            *obj_last = obj->yco_obj_id;

        for(ver = obj->yco_latest; ver != NULL; ver = ver->ycv_prior) {
            *version_count += 1;
            if (ver->ycv_seq_number < *version_first)
                *version_first = ver->ycv_seq_number;
            if (ver->ycv_seq_number > *version_last)
                *version_last = ver->ycv_seq_number;
        }
    }
}