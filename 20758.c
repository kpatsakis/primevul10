VAuditF(const char *f, va_list args)
{
    char *prefix;
    char buf[1024];
    int len;
    static char oldbuf[1024];

    prefix = AuditPrefix();
    len = vsnprintf(buf, sizeof(buf), f, args);

    if (len == oldlen && strcmp(buf, oldbuf) == 0) {
        /* Message already seen */
        nrepeat++;
    }
    else {
        /* new message */
        if (auditTimer != NULL)
            TimerForce(auditTimer);
        ErrorF("%s%s", prefix != NULL ? prefix : "", buf);
        strlcpy(oldbuf, buf, sizeof(oldbuf));
        oldlen = len;
        nrepeat = 0;
        auditTimer = TimerSet(auditTimer, 0, AUDIT_TIMEOUT, AuditFlush, NULL);
    }
    free(prefix);
}