static void init_meminfo(RedMemSlotInfo *mem_info)
{
    memslot_info_init(mem_info, 1 /* groups */, 1 /* slots */, 1, 1, 0);
    memslot_info_add_slot(mem_info, 0, 0, 0 /* delta */, 0 /* start */, ~0ul /* end */, 0 /* generation */);
}