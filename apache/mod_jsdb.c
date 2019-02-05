
#include <unistd.h>
#include <stdio.h>
#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"

/* Define prototypes of our functions in this module */
static void register_hooks(apr_pool_t *pool);
static int jsdb_handler(request_rec *r);

/* Define our module as an entity and assign a function for registering hooks  */

module AP_MODULE_DECLARE_DATA  jsdb_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,            // Per-directory configuration handler
    NULL,            // Merge handler for per-directory configurations
    NULL,            // Per-server configuration handler
    NULL,            // Merge handler for per-server configurations
    NULL,            // Any directives we may have for httpd
    register_hooks   // Our hook registering function
};


/* register_hooks: Adds a hook to the httpd process */
static void register_hooks(apr_pool_t *pool) 
{
    /* Hook the request handler */
    ap_hook_handler(jsdb_handler, NULL, NULL, APR_HOOK_LAST);
}

/* The handler function for our module.
 * This is where all the fun happens!
 */

static int jsdb_handler(request_rec *r)
{
    /* First off, we need to check if this is a call for the "jsdb" handler.
     * If it is, we accept it and do our things, it not, we simply return DECLINED,
     * and Apache will try somewhere else.
     */
    if (!r->handler /*|| strcmp(r->handler, "jsdb")*/) return (DECLINED);
    
    // The first thing we will do is write a simple "Hello, world!" back to the client.
    ap_rputs("Helloooooooooo wooooooooorld!\n", r);
    if (r->handler) ap_rputs("handler = ",r), ap_rputs(r->handler,r), ap_rputs("\n",r);
	if (r->method) ap_rputs("method = ",r), ap_rputs(r->method,r), ap_rputs("\n",r);
	if (r->filename) ap_rputs("filename = ",r), ap_rputs(r->filename,r), ap_rputs("\n",r);
	if (r->args) ap_rputs("args = ",r), ap_rputs(r->args,r), ap_rputs("\n",r);
	if (r->user) ap_rputs("user = ",r), ap_rputs(r->user,r), ap_rputs("\n",r);
	if (r->useragent_ip) ap_rputs("useragent = ",r), ap_rputs(r->useragent_ip,r), ap_rputs("\n",r);

    int pid = getpid();
    char pidStr[64];
    sprintf(pidStr, "%d", pid);
    ap_rputs("pid = ",r), ap_rputs(pidStr,r), ap_rputs("\n",r);
    return OK;
}
