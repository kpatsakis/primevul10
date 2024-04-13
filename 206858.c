am_get_service_url(request_rec *r, LassoProfile *profile, char *service_name)
{
    LassoProvider *provider;
    gchar *url;

    provider = lasso_server_get_provider(profile->server, 
                                         profile->remote_providerID);
    if (LASSO_IS_PROVIDER(provider) == FALSE) {
        AM_LOG_RERROR(APLOG_MARK, APLOG_WARNING, 0, r,
                      "Cannot find provider service %s, no provider.",
                      service_name);
        return NULL;
    }

    url = lasso_provider_get_metadata_one(provider, service_name);
    if (url == NULL) {
        AM_LOG_RERROR(APLOG_MARK, APLOG_WARNING, 0, r,
                      "Cannot find provider service %s from metadata.",
                      service_name);
        return NULL;
    }

    return url;
}