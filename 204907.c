const char *SSL_get_psk_identity(const SSL *s)
{
    if (s == NULL || s->session == NULL)
        return NULL;
    return (s->session->psk_identity);
}