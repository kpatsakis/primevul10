static void _php_image_output_ctxfree(struct gdIOCtx *ctx) {
  if (ctx) {
    IM_FREE(ctx);
  }
}