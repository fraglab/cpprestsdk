#include "stdafx.h"

#include "cpprest/http_headers.h"
#include "http_client_impl.h"

namespace web
{
    namespace http
    {
        namespace client
        {
            namespace details
            {
                // OrbisHTTP client.
                class orbishttp_client final : public _http_client_communicator
                {
                public:
                    orbishttp_client(http::uri address, http_client_config client_config)
                        : _http_client_communicator(std::move(address), std::move(client_config))
                        { }

                    orbishttp_client(const orbishttp_client&) = delete;
                    orbishttp_client &operator=(const orbishttp_client&) = delete;

                    // Closes session.
                    ~orbishttp_client() = default;

                    virtual pplx::task<http_response> propagate(http_request request) override
                    {
                        return pplx::task<http_response>();
                    }

                protected:
                    void send_request(_In_ const std::shared_ptr<request_context> &request)
                    {
                    };
                };

                std::shared_ptr<_http_client_communicator> create_platform_final_pipeline_stage(uri&& base_uri, http_client_config&& client_config)
                {
                    return std::make_shared<details::orbishttp_client>(std::move(base_uri), std::move(client_config));
                }

            }
        }
    }
}