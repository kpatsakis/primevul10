FreeAuditTimer(void)
{
    if (auditTimer != NULL) {
        /* Force output of pending messages */
        TimerForce(auditTimer);
        TimerFree(auditTimer);
        auditTimer = NULL;
    }
}