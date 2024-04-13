    yaffscache_objects_free(YAFFSFS_INFO *yfs)
{
    if((yfs != NULL) && (yfs->cache_objects != NULL)){
        YaffsCacheObject *obj = yfs->cache_objects;
        while(obj != NULL) {
            YaffsCacheObject *to_free = obj;

            YaffsCacheVersion *ver = obj->yco_latest;
            while(ver != NULL) {
                YaffsCacheVersion *v_to_free = ver;
                ver = ver->ycv_prior;
                free(v_to_free);
            }

            obj = obj->yco_next;
            free(to_free);
        }
    }
}