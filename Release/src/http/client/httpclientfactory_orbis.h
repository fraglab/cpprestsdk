#pragma once

#include <mutex>
#include <unordered_set>

namespace web
{
    namespace http
    {
        namespace client
        {
            namespace details
            {
                class HttpClientFactory_orbis
                {
                public:
                    HttpClientFactory_orbis();
                    ~HttpClientFactory_orbis();

                    void DoCleanUp();

                    int OpenConnection(const char* hostUrl, int templateId);
                    int CloseConnection(int connectionId);

                    int GetHttpCtxId() const { return m_libhttpCtxId; }

                private:
                    void InitAPI();
                    void ShutdownAPI();

                private:
                    int m_libnetMemId;
                    int m_libsslCtxId;
                    int m_libhttpCtxId;

                    std::mutex m_connectionMutex;
                    std::unordered_set<int> m_handleContainer;
                };
            }
        }
    }
}