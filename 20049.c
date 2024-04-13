void address_space_remove_listeners(AddressSpace *as)
{
    while (!QTAILQ_EMPTY(&as->listeners)) {
        memory_listener_unregister(QTAILQ_FIRST(&as->listeners));
    }
}