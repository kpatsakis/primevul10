static void sqlite3FreeCodecArg(void *pCodecArg) {
  codec_ctx *ctx = (codec_ctx *) pCodecArg;
  if(pCodecArg == NULL) return;
  sqlcipher_codec_ctx_free(&ctx); /* wipe and free allocated memory for the context */
  sqlcipher_deactivate(); /* cleanup related structures, OpenSSL etc, when codec is detatched */
}