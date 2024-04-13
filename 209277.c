PLT_HttpServer::ServeStream(const NPT_HttpRequest&        request, 
                            const NPT_HttpRequestContext& context,
                            NPT_HttpResponse&             response,
                            NPT_InputStreamReference&     body, 
                            const char*                   content_type) 
{    
    if (body.IsNull()) return NPT_FAILURE;
    
    // set date
    NPT_TimeStamp now;
    NPT_System::GetCurrentTimeStamp(now);
    response.GetHeaders().SetHeader("Date", NPT_DateTime(now).ToString(NPT_DateTime::FORMAT_RFC_1123), true);
    
    // get entity
    NPT_HttpEntity* entity = response.GetEntity();
    NPT_CHECK_POINTER_FATAL(entity);
    
    // set the content type
    entity->SetContentType(content_type);
    
    // check for range requests
    const NPT_String* range_spec = request.GetHeaders().GetHeaderValue(NPT_HTTP_HEADER_RANGE);
    
    // setup entity body
    NPT_CHECK(NPT_HttpFileRequestHandler::SetupResponseBody(response, body, range_spec));
              
    // set some default headers
    if (response.GetEntity()->GetTransferEncoding() != NPT_HTTP_TRANSFER_ENCODING_CHUNKED) {
        // set but don't replace Accept-Range header only if body is seekable
        NPT_Position offset;
        if (NPT_SUCCEEDED(body->Tell(offset)) && NPT_SUCCEEDED(body->Seek(offset))) {
            response.GetHeaders().SetHeader(NPT_HTTP_HEADER_ACCEPT_RANGES, "bytes", false); 
        }
    }
    
    // set getcontentFeatures.dlna.org
    const NPT_String* value = request.GetHeaders().GetHeaderValue("getcontentFeatures.dlna.org");
    if (value) {
        PLT_HttpRequestContext tmp_context(request, context);
        const char* dlna = PLT_ProtocolInfo::GetDlnaExtension(entity->GetContentType(),
                                                              &tmp_context);
        if (dlna) response.GetHeaders().SetHeader("ContentFeatures.DLNA.ORG", dlna, false);
    }
    
    // transferMode.dlna.org
    value = request.GetHeaders().GetHeaderValue("transferMode.dlna.org");
    if (value) {
        // Interactive mode not supported?
        /*if (value->Compare("Interactive", true) == 0) {
            response.SetStatus(406, "Not Acceptable");
            return NPT_SUCCESS;
        }*/
        
        response.GetHeaders().SetHeader("TransferMode.DLNA.ORG", value->GetChars(), false);
    } else {
        response.GetHeaders().SetHeader("TransferMode.DLNA.ORG", "Streaming", false);
    }
    
    if (request.GetHeaders().GetHeaderValue("TimeSeekRange.dlna.org")) {
        response.SetStatus(406, "Not Acceptable");
        return NPT_SUCCESS;
    }
    
    return NPT_SUCCESS;
}