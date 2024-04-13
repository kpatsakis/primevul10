ofpact_encode_hmap(void)
{
    static struct ovsthread_once once = OVSTHREAD_ONCE_INITIALIZER;
    static struct hmap hmap;

    if (ovsthread_once_start(&once)) {
        struct ofpact_raw_instance *inst;

        hmap_init(&hmap);
        for (inst = all_raw_instances;
             inst < &all_raw_instances[ARRAY_SIZE(all_raw_instances)];
             inst++) {
            hmap_insert(&hmap, &inst->encode_node,
                        hash_2words(inst->raw, inst->hdrs.ofp_version));
        }
        ovsthread_once_done(&once);
    }
    return &hmap;
}