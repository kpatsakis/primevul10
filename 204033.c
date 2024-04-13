register_basic_auth_host (const char *hostname)
{
  if (!basic_authed_hosts)
    {
      basic_authed_hosts = make_nocase_string_hash_table (1);
    }
  if (!hash_table_contains (basic_authed_hosts, hostname))
    {
      hash_table_put (basic_authed_hosts, xstrdup (hostname), NULL);
      DEBUGP (("Inserted %s into basic_authed_hosts\n", quote (hostname)));
    }
}