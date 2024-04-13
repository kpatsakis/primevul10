static void printevents(unsigned int events) {
        if (events == Event_Null) {
                printf("No events");
        } else if (events == Event_All) {
                printf("All events");
        } else {
                if (IS_EVENT_SET(events, Event_Action))
                        printf("Action ");
                if (IS_EVENT_SET(events, Event_ByteIn))
                        printf("ByteIn ");
                if (IS_EVENT_SET(events, Event_ByteOut))
                        printf("ByteOut ");
                if (IS_EVENT_SET(events, Event_Checksum))
                        printf("Checksum ");
                if (IS_EVENT_SET(events, Event_Connection))
                        printf("Connection ");
                if (IS_EVENT_SET(events, Event_Content))
                        printf("Content ");
                if (IS_EVENT_SET(events, Event_Data))
                        printf("Data ");
                if (IS_EVENT_SET(events, Event_Exec))
                        printf("Exec ");
                if (IS_EVENT_SET(events, Event_Exist))
                        printf("Exist ");
                if (IS_EVENT_SET(events, Event_FsFlag))
                        printf("Fsflags ");
                if (IS_EVENT_SET(events, Event_Gid))
                        printf("Gid ");
                if (IS_EVENT_SET(events, Event_Icmp))
                        printf("Icmp ");
                if (IS_EVENT_SET(events, Event_Instance))
                        printf("Instance ");
                if (IS_EVENT_SET(events, Event_Invalid))
                        printf("Invalid ");
                if (IS_EVENT_SET(events, Event_Link))
                        printf("Link ");
                if (IS_EVENT_SET(events, Event_NonExist))
                        printf("Nonexist ");
                if (IS_EVENT_SET(events, Event_PacketIn))
                        printf("PacketIn ");
                if (IS_EVENT_SET(events, Event_PacketOut))
                        printf("PacketOut ");
                if (IS_EVENT_SET(events, Event_Permission))
                        printf("Permission ");
                if (IS_EVENT_SET(events, Event_Pid))
                        printf("PID ");
                if (IS_EVENT_SET(events, Event_PPid))
                        printf("PPID ");
                if (IS_EVENT_SET(events, Event_Resource))
                        printf("Resource ");
                if (IS_EVENT_SET(events, Event_Saturation))
                        printf("Saturation ");
                if (IS_EVENT_SET(events, Event_Size))
                        printf("Size ");
                if (IS_EVENT_SET(events, Event_Speed))
                        printf("Speed ");
                if (IS_EVENT_SET(events, Event_Status))
                        printf("Status ");
                if (IS_EVENT_SET(events, Event_Timeout))
                        printf("Timeout ");
                if (IS_EVENT_SET(events, Event_Timestamp))
                        printf("Timestamp ");
                if (IS_EVENT_SET(events, Event_Uid))
                        printf("Uid ");
                if (IS_EVENT_SET(events, Event_Uptime))
                        printf("Uptime ");

        }
        printf("\n");
}