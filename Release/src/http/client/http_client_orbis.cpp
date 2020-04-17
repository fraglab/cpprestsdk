#include "stdafx.h"

#include "cpprest/http_headers.h"
#include "cpprest/details/string.h"
#include "http_client_impl.h"

#include "httpclientfactory_orbis.h"

// PS4 specific headers
#include <scebase.h>
#include <stdint.h>
#include <stdlib.h>
#include <kernel.h>
#include <libdbg.h>
#include <libhttp.h>
#include <sceerror.h>
#include <scetypes.h>

namespace web::http::client::details
{
    // Additional information necessary to track a request.
    class orbishttp_request_context final : public request_context
    {
    public:
        orbishttp_request_context(const std::shared_ptr<_http_client_communicator>& client, const http_request& request)
            : request_context(client, request)
        {
        }

        // Factory function to create requests on the heap.
        static std::shared_ptr<request_context> create_request_context(
            const std::shared_ptr<_http_client_communicator>& client, const http_request& request)
        {
            return std::make_shared<orbishttp_request_context>(client, request);
        }

    public:
        int m_sceHttpTemplateId{ -1 };
        int m_sceConnectionId{ -1 };
        int m_sceRequestId{ -1 };
    };

    // OrbisHTTP client.
    class orbishttp_client final : public _http_client_communicator
    {
    public:
        orbishttp_client(http::uri address, http_client_config client_config)
            : _http_client_communicator(std::move(address), std::move(client_config))
        {
        }

        orbishttp_client(const orbishttp_client&) = delete;
        orbishttp_client& operator=(const orbishttp_client&) = delete;

        virtual pplx::task<http_response> propagate(http_request request) override
        {
            auto self = std::static_pointer_cast<_http_client_communicator>(shared_from_this());
            auto context = details::orbishttp_request_context::create_request_context(self, request);

            // Use a task to externally signal the final result and completion of the task.
            auto result_task = pplx::create_task(context->m_request_completion);

            // Asynchronously send the response with the HTTP client implementation.
            this->async_send_request(context);

            return result_task;
        }

    protected:
        void send_request(_In_ const std::shared_ptr<request_context>& request)
        {
            static HttpClientFactory_orbis clientFactory;

            int ret = 0;
            int libhttpCtxId = clientFactory.GetHttpCtxId();

            if (libhttpCtxId < 0)
            {
                request->report_error(ret, _XPLATSTR("API init failed"));
                return;
            }

            ret = sceHttpCreateTemplate(clientFactory.GetHttpCtxId(), nullptr, SCE_HTTP_VERSION_1_1, SCE_TRUE);
            if (ret < 0)
            {
                request->report_error(ret, _XPLATSTR("Create template failed"));
                return;
            }

            std::shared_ptr<orbishttp_request_context> orbis_context =
                std::static_pointer_cast<orbishttp_request_context>(request);

            orbis_context->m_sceHttpTemplateId = ret;

            // First see if we need to be opened.
            ret = open_connection(orbis_context, clientFactory);
            if (ret < 0)
            {
                request->report_error(ret, _XPLATSTR("Open failed"));
                return;
            }
            orbis_context->m_sceConnectionId = ret;

            ret = setup_request(orbis_context);
            if (ret < 0)
            {
                close_connection(orbis_context, clientFactory);
                request->report_error(ret, _XPLATSTR("Create request failed"));
                return;
            }

            orbis_context->m_sceRequestId = ret;

            setup_request_header(orbis_context);

            send_request_and_payload(orbis_context, get_request_body_size(request));

            int statusCode = 0;
            int statusReturn = sceHttpGetStatusCode(orbis_context->m_sceRequestId, &statusCode);
            if (statusReturn < 0)
            {
                close_connection(orbis_context, clientFactory);
                request->report_error(statusReturn, _XPLATSTR("sceHttpGetStatusCode error"));
                return;
            }

            request->m_response.set_status_code(http::status_code(statusCode));
            read_response_header(orbis_context);
            int body_size = read_response_body(orbis_context);

            close_connection(orbis_context, clientFactory);

            request->complete_request(body_size);
            request->complete_headers();
        };

    private:
        int open_connection(_In_ const std::shared_ptr<orbishttp_request_context>& request, HttpClientFactory_orbis& clientFactory)
        {
            http_request& msg = request->m_request;
            const utility::string_t encoded_resource =
                http::uri_builder(m_uri).append(msg.relative_uri()).to_uri().to_string();

            int ret = clientFactory.OpenConnection(encoded_resource.c_str(), request->m_sceHttpTemplateId);
            return ret;
        }

        void close_connection(_In_ const std::shared_ptr<orbishttp_request_context>& request, HttpClientFactory_orbis& clientFactory)
        {
            if (request->m_sceRequestId > 0)
            {
                sceHttpDeleteRequest(request->m_sceRequestId);
            }

            int ret = clientFactory.CloseConnection(request->m_sceConnectionId);
            sceHttpDeleteTemplate(request->m_sceHttpTemplateId);
        }

        int setup_request(_In_ const std::shared_ptr<orbishttp_request_context>& request)
        {
            http_request& msg = request->m_request;
            const utility::string_t encoded_resource =
                http::uri_builder(m_uri).append(msg.relative_uri()).to_uri().to_string();

            int ret = sceHttpCreateRequestWithURL2(request->m_sceConnectionId,
                request->m_request.method().c_str(), encoded_resource.c_str(), 0);

            return ret;
        }

        void setup_request_header(_In_ const std::shared_ptr<orbishttp_request_context>& request)
        {
            http_request& msg = request->m_request;
            http_headers& requestHeaders = msg.headers();

            for (auto& requestHeader : requestHeaders)
            {
                std::string inputStr = requestHeader.first.c_str();
                if (inputStr == "Content-Length")
                {
                    // We use the sceHttpSetRequestContentLength when sending our payload which handles content length
                    continue;
                }
                else if (inputStr == "Host")
                {
                    // Use this Host value so we don't send two hosts
                    inputStr = "Host";
                }
                else if (inputStr == "User-Agent")
                {
                    // We need to use the user-agent that we signed our request with - set to Caps to overwrite the orbis User-Agent which has been automatically added
                    inputStr = "User-Agent";
                }
                add_request_header(inputStr.c_str(), requestHeader.second.c_str(), SCE_HTTP_HEADER_OVERWRITE, request);
            }

            if (!requestHeaders.has("Content-Type"))
            {
                add_request_header("Content-Type", "", SCE_HTTP_HEADER_OVERWRITE, request);
            }
        }

        int send_request_and_payload(_In_ const std::shared_ptr<orbishttp_request_context>& request, uint64_t sceRequestSize) const
        {
            sceHttpSetRequestContentLength(request->m_sceRequestId, sceRequestSize);
            int payloadRequestCount = 0;
            int sendResult = 0;
            bool done = false;
            if (sceRequestSize > 0)
            {
                uint64_t bytesRemaining = sceRequestSize;
                auto rbuf = request->_get_readbuffer();

                uint8_t* block = nullptr;
                size_t length = 0;
                while (!done)
                {
                    if (rbuf.acquire(block, length))
                    {
                        if (length == 0)
                        {
                            if (rbuf.exception() == nullptr)
                            {
                                request->report_error(errno,
                                    _XPLATSTR("Error reading outgoing HTTP body from its stream."));
                            }
                            else
                            {
                                request->report_exception(rbuf.exception());
                            }

                            return -1;
                        }

                        size_t bufferSize = request->m_http_client->client_config().chunksize();
                        const size_t to_write = sceRequestSize < bufferSize ? sceRequestSize : bufferSize;

                        sendResult = sceHttpSendRequest(request->m_sceRequestId, block, to_write);

                        if (sendResult < 0)
                        {
                            done = true;
                        }

                        sceRequestSize -= to_write;
                        done |= sceRequestSize == 0;
                    }
                }
                return sendResult;
            }
            else
            {
                sendResult = sceHttpSendRequest(request->m_sceRequestId, nullptr, 0);
                return sendResult;
            }
        }


        int read_response_header(_In_ const std::shared_ptr<orbishttp_request_context>& request)
        {
            size_t headerSize = 0;
            char* dataStr = nullptr;

            int ret = sceHttpGetAllResponseHeaders(request->m_sceRequestId, &dataStr, &headerSize);
            if (ret < 0)
            {
                return ret;
            }

            auto& response = request->m_response;
            http_headers& requestHeaders = response.headers();

            char* token = strtok(dataStr, "\n");
            while (token != nullptr)
            {
                std::string header = token;
            std:size_t pos = header.find(':');
                if (pos != std::string::npos)
                {
                    std::string headerName = header.substr(0, pos);
                    std::string headerValue = header.substr(pos + 1, header.size() - pos);

                    web::lib::algorithm::trim(headerName);
                    web::lib::algorithm::trim(headerValue);

                    requestHeaders.add(headerName, headerValue);
                }
                token = strtok(NULL, "\n");
            }

            return ret;
        }

        int read_response_body(_In_ const std::shared_ptr<orbishttp_request_context>& request)
        {
            uint64_t    contentLength = 0;

            uint64_t chunksize = request->m_http_client->client_config().chunksize();

            bool readingDone = false;
            int ret = 0;
            int contentLengthType = 0;

            ret = sceHttpGetResponseContentLength(request->m_sceRequestId, &contentLengthType, &contentLength);
            if (ret < 0)
            {
                return ret;
            }

            auto writeBuffer = request->_get_writebuffer();

            int bytesRead = 0;
            int totalBytes = 0;
            while (!readingDone)
            {
                uint64_t leftToRead = contentLength - totalBytes;
                uint64_t bufferSize = leftToRead > chunksize ? chunksize : leftToRead;
                unsigned char* recvBuf = writeBuffer.alloc(bufferSize);
                bytesRead = sceHttpReadData(request->m_sceRequestId, recvBuf, bufferSize);
                if (bytesRead < 0)
                {
                    writeBuffer.release(recvBuf, bufferSize);
                    return bytesRead;
                }
                else
                {
                    totalBytes += bytesRead;
                    readingDone = bytesRead == 0 || totalBytes >= contentLength;
                }
                writeBuffer.commit(bytesRead);
            }
            request->m_response.set_body(writeBuffer, totalBytes);

            return totalBytes;
        }

        void add_request_header(const char* headerName, const char* headerValue, SceHttpAddHeaderMode addMode, const std::shared_ptr<orbishttp_request_context>& request)
        {
            sceHttpAddRequestHeader(request->m_sceRequestId, headerName, headerValue ? headerValue : "", addMode);
        }

        uint64_t get_request_body_size(const std::shared_ptr<request_context>& request) const
        {
            http_request& msg = request->m_request;

            size_t content_length;
            try
            {
                content_length = msg._get_impl()->_get_content_length_and_set_compression();
            }
            catch (...)
            {
                request->report_exception(std::current_exception());
                return 0;
            }

            return content_length;
        }
    };

    std::shared_ptr<_http_client_communicator> create_platform_final_pipeline_stage(uri&& base_uri, http_client_config&& client_config)
    {
        return std::make_shared<details::orbishttp_client>(std::move(base_uri), std::move(client_config));
    }
}