static void test_memslot_invalid_slot_id(void)
{
    RedMemSlotInfo mem_info;
    init_meminfo(&mem_info);

    memslot_get_virt(&mem_info, 1 << mem_info.memslot_id_shift, 16, 0);
}