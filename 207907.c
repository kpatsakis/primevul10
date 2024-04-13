static void assert_valid_malloc_pointer(void* mem)
{
    if (mem && !is_valid_malloc_pointer(mem)) {
        assert_log_message(
            "*** MALLOC CHECK: buffer %p, is not a valid "
            "malloc pointer (are you mixing up new/delete "
            "and malloc/free?)", mem);
    }
}