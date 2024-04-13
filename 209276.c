PLT_HttpServer::SetupResponse(NPT_HttpRequest&              request, 
                              const NPT_HttpRequestContext& context,
                              NPT_HttpResponse&             response) 
{
    NPT_String prefix = NPT_String::Format("PLT_HttpServer::SetupResponse %s request from %s for \"%s\"", 
        (const char*) request.GetMethod(),
        (const char*) context.GetRemoteAddress().ToString(),
        (const char*) request.GetUrl().ToString());
    PLT_LOG_HTTP_REQUEST(NPT_LOG_LEVEL_FINE, prefix, &request);

    NPT_List<NPT_HttpRequestHandler*> handlers = FindRequestHandlers(request);
    if (handlers.GetItemCount() == 0) return NPT_ERROR_NO_SUCH_ITEM;

    // ask the handler to setup the response
    NPT_Result result = (*handlers.GetFirstItem())->SetupResponse(request, context, response);
    
    // DLNA compliance
    PLT_UPnPMessageHelper::SetDate(response);
    if (request.GetHeaders().GetHeader("Accept-Language")) {
        response.GetHeaders().SetHeader("Content-Language", "en");
    }
    return result;
}