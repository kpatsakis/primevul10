process_cmd_authhash(const char *line)
{
  const char *hash_name;
  int new_hash_id;

  assert(auth_hash_id >= 0);
  hash_name = line;

  if (!*hash_name) {
    fprintf(stderr, "Could not parse hash name\n");
    return 0;
  }

  new_hash_id = HSH_GetHashId(hash_name);
  if (new_hash_id < 0) {
    fprintf(stderr, "Unknown hash name: %s\n", hash_name);
    return 0;
  }

  auth_hash_id = new_hash_id;

  return 1;
}