static void test_memslot_invalid_group_id(void)
{
    RedMemSlotInfo mem_info;
    init_meminfo(&mem_info);

    memslot_get_virt(&mem_info, 0, 16, 1);
}