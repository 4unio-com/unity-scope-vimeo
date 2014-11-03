#include <vimeo/api/login.h>

#include <core/net/error.h>
#include <core/net/uri.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/response.h>
#include <json/json.h>

#include <string>

namespace http = core::net::http;
namespace json = Json;
namespace net = core::net;

using namespace std;

namespace vimeo {
namespace api {

string unauthenticated(const std::string &client_id,
        const std::string &client_secret, const std::string &api_root) {
    auto client = http::make_client();

    http::Request::Configuration configuration;
    net::Uri uri = net::make_uri(api_root, { "oauth", "authorize", "client" }, {
            { "grant_type", "client_credentials" }, { "scope", "public" } });
    configuration.uri = client->uri_to_string(uri);

    string auth = "basic "
            + client->base64_encode(client_id + ":" + client_secret);
    configuration.header.add("Authorization", auth);

    auto request = client->post(configuration, string(), string());

    try {
        auto response = request->execute([](const http::Request::Progress&) {
            return http::Request::Progress::Next::continue_operation;
        });

        json::Reader reader;
        json::Value root;
        reader.parse(response.body, root);

        if (response.status == http::Status::ok) {
            return root["access_token"].asString();
        } else {
            throw domain_error(root["error"].asString());
        }
    } catch (net::Error &e) {
        throw domain_error(e.what());
    }
}

}
}
