NsWakeupDriver(const Driver *drvPtr) {
    NS_NONNULL_ASSERT(drvPtr != NULL);
    SockTrigger(drvPtr->trigger[1]);
}