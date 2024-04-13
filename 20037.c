void memory_listener_unregister(MemoryListener *listener)
{
    if (!listener->address_space) {
        return;
    }

    listener_del_address_space(listener, listener->address_space);
    QTAILQ_REMOVE(&listener->address_space->uc->memory_listeners, listener, link);
    QTAILQ_REMOVE(&listener->address_space->listeners, listener, link_as);
    listener->address_space = NULL;
}