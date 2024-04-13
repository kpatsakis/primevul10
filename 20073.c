int cpu_watchpoint_address_matches(CPUState *cpu, vaddr addr, vaddr len)
{
#if 0
    CPUWatchpoint *wp;
    int ret = 0;

    QTAILQ_FOREACH(wp, &cpu->watchpoints, entry) {
        if (watchpoint_address_matches(wp, addr, TARGET_PAGE_SIZE)) {
            ret |= wp->flags;
        }
    }
    return ret;
#endif
    return 0;
}