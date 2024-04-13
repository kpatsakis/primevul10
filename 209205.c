NsInitDrivers(void)
{
    DriverDebug = Ns_CreateLogSeverity("Debug(ns:driver)");
    WriterDebug = Ns_CreateLogSeverity("Debug(writer)");
    Ns_LogTaskDebug = Ns_CreateLogSeverity("Debug(task)");
    Ns_LogRequestDebug = Ns_CreateLogSeverity("Debug(request)");
    Ns_LogConnchanDebug = Ns_CreateLogSeverity("Debug(connchan)");
    Ns_LogUrlspaceDebug = Ns_CreateLogSeverity("Debug(urlspace)");
    Ns_LogAccessDebug = Ns_CreateLogSeverity("Debug(access)");
    Ns_LogTimeoutDebug = Ns_CreateLogSeverity("Debug(timeout)");
    Ns_MutexInit(&reqLock);
    Ns_MutexInit(&writerlock);
    Ns_MutexSetName2(&reqLock, "ns:driver", "requestpool");
    Ns_MutexSetName2(&writerlock, "ns:writer", "stream");
}