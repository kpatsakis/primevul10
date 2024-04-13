loader_init_callback (lt_dlhandle handle)
{
  lt_get_vtable *vtable_func = (lt_get_vtable *) lt_dlsym (handle, "get_vtable");
  return loader_init (vtable_func, 0);
}