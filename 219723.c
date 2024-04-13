urnFindMinRtt(url_entry * urls, const HttpRequestMethod &, int *rtt_ret)
{
    int min_rtt = 0;
    url_entry *u = NULL;
    url_entry *min_u = NULL;
    int i;
    int urlcnt = 0;
    debugs(52, 3, "urnFindMinRtt");
    assert(urls != NULL);

    for (i = 0; NULL != urls[i].url; ++i)
        ++urlcnt;

    debugs(53, 3, "urnFindMinRtt: Counted " << i << " URLs");

    if (1 == urlcnt) {
        debugs(52, 3, "urnFindMinRtt: Only one URL - return it!");
        return urls;
    }

    for (i = 0; i < urlcnt; ++i) {
        u = &urls[i];
        debugs(52, 3, "urnFindMinRtt: " << u->host << " rtt=" << u->rtt);

        if (u->rtt == 0)
            continue;

        if (u->rtt > min_rtt && min_rtt != 0)
            continue;

        min_rtt = u->rtt;

        min_u = u;
    }

    if (rtt_ret)
        *rtt_ret = min_rtt;

    debugs(52, DBG_IMPORTANT, "urnFindMinRtt: Returning '" <<
           (min_u ? min_u->url : "NONE") << "' RTT " <<
           min_rtt  );

    return min_u;
}