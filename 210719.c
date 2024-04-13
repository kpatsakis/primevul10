ServiceWorkerScriptContext::ServiceWorkerScriptContext(
    EmbeddedWorkerContextClient* embedded_context,
    blink::WebServiceWorkerContextProxy* proxy)
    : embedded_context_(embedded_context),
      proxy_(proxy),
      current_request_id_(kInvalidRequestId) {
}
