value_checker2 (void *cls,
                enum MHD_ValueKind kind,
                const char *key,
                const char *filename,
                const char *content_type,
                const char *transfer_encoding,
                const char *data,
                uint64_t off,
                size_t size)
{
  return MHD_YES;
}