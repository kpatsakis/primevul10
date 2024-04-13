static uint64_t test_splitting_mmio_unmap_read_callback(uc_engine *uc,
                                                        uint64_t offset,
                                                        unsigned size,
                                                        void *user_data)
{
    TEST_CHECK(offset == 4);
    TEST_CHECK(size == 4);

    return 0x19260817;
}