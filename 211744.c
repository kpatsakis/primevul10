int skipheader (gdIOCtx *ctx) {
  int i;

  do {
    i = (ctx->getC)(ctx);
    if (i < 0) return (-1);
  }
  while (i & 0x80);

  return (0);
}