ofpact_decode_hmap(void)
{
    static struct ovsthread_once once = OVSTHREAD_ONCE_INITIALIZER;
    static struct hmap hmap;

    if (ovsthread_once_start(&once)) {
        struct ofpact_raw_instance *inst;

        hmap_init(&hmap);
        for (inst = all_raw_instances;
             inst < &all_raw_instances[ARRAY_SIZE(all_raw_instances)];
             inst++) {
            hmap_insert(&hmap, &inst->decode_node,
                        ofpact_hdrs_hash(&inst->hdrs));
        }
        ovsthread_once_done(&once);
    }
    return &hmap;
}