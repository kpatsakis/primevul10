rfbBool rfbSendTextChatMessage(rfbClientPtr cl, uint32_t length, char *buffer)
{
    rfbTextChatMsg tc;
    int bytesToSend=0;

    memset((char *)&tc, 0, sizeof(tc)); 
    tc.type = rfbTextChat;
    tc.length = Swap32IfLE(length);
    
    switch(length) {
    case rfbTextChatOpen:
    case rfbTextChatClose:
    case rfbTextChatFinished:
        bytesToSend=0;
        break;
    default:
        bytesToSend=length;
        if (bytesToSend>rfbTextMaxSize)
            bytesToSend=rfbTextMaxSize;
    }

    if (cl->ublen + sz_rfbTextChatMsg + bytesToSend > UPDATE_BUF_SIZE) {
        if (!rfbSendUpdateBuf(cl))
            return FALSE;
    }
    
    memcpy(&cl->updateBuf[cl->ublen], (char *)&tc, sz_rfbTextChatMsg);
    cl->ublen += sz_rfbTextChatMsg;
    if (bytesToSend>0) {
        memcpy(&cl->updateBuf[cl->ublen], buffer, bytesToSend);
        cl->ublen += bytesToSend;    
    }
    rfbStatRecordMessageSent(cl, rfbTextChat, sz_rfbTextChatMsg+bytesToSend, sz_rfbTextChatMsg+bytesToSend);

    if (!rfbSendUpdateBuf(cl))
        return FALSE;
        
    return TRUE;
}