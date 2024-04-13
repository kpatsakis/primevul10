static void tlsCheckUncommonALPN(struct ndpi_flow_struct *flow)
{
  /* see: https://www.iana.org/assignments/tls-extensiontype-values/tls-extensiontype-values.xhtml */
  static char const * const common_alpns[] = {
    "http/0.9", "http/1.0", "http/1.1",
    "spdy/1", "spdy/2", "spdy/3", "spdy/3.1",
    "stun.turn", "stun.nat-discovery",
    "h2", "h2c", "h2-16", "h2-15", "h2-14",
    "webrtc", "c-webrtc",
    "ftp", "imap", "pop3", "managesieve", "coap",
    "xmpp-client", "xmpp-server",
    "acme-tls/1",
    "mqtt", "dot", "ntske/1", "sunrpc",
    "h3",
    "smb",
    "irc",

    /* QUIC ALPNs */
    "h3-T051", "h3-T050",
    "h3-32", "h3-30", "h3-29", "h3-28", "h3-27", "h3-24", "h3-22",
    "hq-30", "hq-29", "hq-28", "hq-27",
    "h3-fb-05", "h1q-fb",
    "doq-i00"
  };

  /*
   * If the ALPN list increases in size, iterating over all items for every incoming ALPN may
   * have a performance impact. A hash map could solve this issue.
   */

  char * alpn_start = flow->protos.tls_quic_stun.tls_quic.alpn;
  char * comma_or_nul = alpn_start;
  do {
    comma_or_nul = strchr(comma_or_nul, ',');
    if (comma_or_nul == NULL)
    {
      comma_or_nul = alpn_start + strlen(alpn_start);
    }

    int alpn_found = 0;
    int alpn_len = comma_or_nul - alpn_start;
    char const * const alpn = alpn_start;
    for (size_t i = 0; i < sizeof(common_alpns)/sizeof(common_alpns[0]); ++i)
    {
      if (strlen(common_alpns[i]) == alpn_len &&
          strncmp(alpn, common_alpns[i], alpn_len) == 0)
      {
        alpn_found = 1;
        break;
      }
    }

    if (alpn_found == 0)
    {
#ifdef DEBUG_TLS
      printf("TLS uncommon ALPN found: %.*s\n", alpn_len, alpn);
#endif
      ndpi_set_risk(flow, NDPI_TLS_UNCOMMON_ALPN);
      break;
    }

    alpn_start = comma_or_nul + 1;
  } while (*(comma_or_nul++) != '\0');
}