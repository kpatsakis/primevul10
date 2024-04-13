static void test_memslot_invalid_addresses(void)
{
    g_test_trap_subprocess("/server/memslot-invalid-addresses/subprocess/group_id", 0, 0);
    g_test_trap_assert_stderr("*group_id too big*");

    g_test_trap_subprocess("/server/memslot-invalid-addresses/subprocess/slot_id", 0, 0);
    g_test_trap_assert_stderr("*slot_id 1 too big*");
}