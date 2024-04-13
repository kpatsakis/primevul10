create_thread_context(blosc2_context* context, int32_t tid) {
  struct thread_context* thread_context;
  thread_context = (struct thread_context*)my_malloc(sizeof(struct thread_context));
  init_thread_context(thread_context, context, tid);
  return thread_context;
}