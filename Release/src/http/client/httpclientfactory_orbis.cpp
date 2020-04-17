#include "httpclientfactory_orbis.h"

// PS4 specific headers
#include <libhttp.h>
#include <net.h>
#include <scebase.h>
#include <libssl.h>

#define SSL_POOL_SIZE  (10 * 1024 * 1024U)
#define LIBHTTP_POOLSIZE (10 * 1024 * 1024)
#define NET_POOL_SIZE (10 * 1024 * 1024)

namespace web
{
    namespace http
    {
        namespace client
        {
            namespace details
            {
                HttpClientFactory_orbis::HttpClientFactory_orbis()
                {
                    InitAPI();
                }

                HttpClientFactory_orbis::~HttpClientFactory_orbis()
                {
                    DoCleanUp();
                    ShutdownAPI();
                }

                void HttpClientFactory_orbis::InitAPI()
                {
                    int ret = sceNetPoolCreate("OrbisHttpClient", NET_POOL_SIZE, 0);
                    if (ret < 0)
                    {
                        m_libhttpCtxId = ret;
                        return;
                    }
                    m_libnetMemId = ret;

                    /* Initialization of Ssl library (when HTTPS communication is performed)*/
                    ret = sceSslInit(SSL_POOL_SIZE);
                    if (ret < 0)
                    {
                        m_libhttpCtxId = ret;
                        return;
                    }
                    m_libsslCtxId = ret;

                    /* Initialize Httplibrary */
                    ret = sceHttpInit(m_libnetMemId, m_libsslCtxId, LIBHTTP_POOLSIZE);
                    m_libhttpCtxId = ret;
                }

                void HttpClientFactory_orbis::ShutdownAPI()
                {
                    if (m_libhttpCtxId)
                    {
                        sceHttpTerm(m_libhttpCtxId);
                        m_libhttpCtxId = 0;
                    }

                    if (m_libsslCtxId)
                    {
                        sceSslTerm(m_libsslCtxId);
                        m_libsslCtxId = 0;
                    }

                    if (m_libnetMemId)
                    {
                        sceNetPoolDestroy(m_libnetMemId);
                        m_libnetMemId = 0;
                    }
                }

                void HttpClientFactory_orbis::DoCleanUp()
                {
                    while (m_handleContainer.size())
                    {
                        auto headElement = m_handleContainer.begin();
                        if (headElement != m_handleContainer.end())
                        {
                            CloseConnection(*headElement);
                        }
                    }
                }

                int HttpClientFactory_orbis::OpenConnection(const char* hostUrl, int templateId)
                {
                    int connectionId = sceHttpCreateConnectionWithURL(templateId, hostUrl, SCE_TRUE);
                    if (connectionId >= 0)
                    {
                        std::lock_guard<std::mutex> locker(m_connectionMutex);
                        m_handleContainer.insert(connectionId);
                    }

                    return connectionId;
                }

                int HttpClientFactory_orbis::CloseConnection(int connectionId)
                {
                    {
                        std::lock_guard<std::mutex> locker(m_connectionMutex);
                        m_handleContainer.erase(connectionId);
                    }

                    return sceHttpDeleteConnection(connectionId);
                }
            }
        }
    }
}
