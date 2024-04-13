mrb_alloca(mrb_state *mrb, size_t size)
{
  struct RString *s;
  s = (struct RString*)mrb_obj_alloc(mrb, MRB_TT_STRING, mrb->string_class);
  return s->as.heap.ptr = (char*)mrb_malloc(mrb, size);
}