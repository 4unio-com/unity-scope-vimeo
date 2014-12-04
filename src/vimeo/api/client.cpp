/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#include <vimeo/api/client.h>
#include <vimeo/api/login.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <core/net/error.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/response.h>
#include <json/json.h>

#include <fstream>

namespace http = core::net::http;
namespace fs = boost::filesystem;
namespace io = boost::iostreams;
namespace json = Json;
namespace net = core::net;

using namespace vimeo::api;
using namespace std;

static const char* CLIENT_ID = "b6758ff9f929cdb9f45a8477732bdbc4c6a89c7e";
static const char* CLIENT_SECRET = "a3222f38f799b3b528e29418fe062c02c677a249";

namespace {

template<typename T>
static deque<shared_ptr<T>> get_list(const json::Value &root) {
    deque<shared_ptr<T>> results;
    json::Value data = root["data"];
    for (json::ArrayIndex index = 0; index < data.size(); ++index) {
        results.push_back(make_shared<T>(data[index]));
    }
    return results;
}

}

class Client::Priv {
public:
    Priv(std::shared_ptr<unity::scopes::OnlineAccountClient> oa_client) :
            client_(http::make_client()), worker_ { [this]() {client_->run();} },
            oa_client_(oa_client), cancelled_(false) {
    }

    ~Priv() {
        client_->stop();
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    std::shared_ptr<core::net::http::Client> client_;

    std::thread worker_;

    Config config_;
    std::mutex config_mutex_;

    std::shared_ptr<unity::scopes::OnlineAccountClient> oa_client_;

    std::atomic<bool> cancelled_;

    void get(const net::Uri::Path &path,
            const net::Uri::QueryParameters &parameters,
            http::Request::Handler &handler) {
        std::lock_guard<std::mutex> lock(config_mutex_);
        update_config();

        http::Request::Configuration configuration;
        net::Uri::QueryParameters complete_parameters(parameters);

        net::Uri uri = net::make_uri(config_.apiroot, path,
                complete_parameters);
        configuration.uri = client_->uri_to_string(uri);
        if (!config_.access_token.empty()) {
            configuration.header.add("Authorization",
                    "bearer " + config_.access_token);
        } else if (!config_.client_id.empty() && !config_.client_secret.empty()) {
            string auth = "basic "
                    + client_->base64_encode(
                            config_.client_id + ":" + config_.client_secret);
            configuration.header.add("Authorization", auth);
        }
        configuration.header.add("Accept", config_.accept);
        configuration.header.add("User-Agent", config_.user_agent + " (gzip)");
        configuration.header.add("Accept-Encoding", "gzip");

        auto request = client_->head(configuration);

        request->async_execute(handler);
    }

    http::Request::Progress::Next progress_report(
            const http::Request::Progress&) {
        return cancelled_ ?
                http::Request::Progress::Next::abort_operation :
                http::Request::Progress::Next::continue_operation;
    }

    template<typename T>
    future<T> async_get(const net::Uri::Path &path,
            const net::Uri::QueryParameters &parameters,
            const function<T(const json::Value &root)> &func) {
        auto prom = make_shared<promise<T>>();

        http::Request::Handler handler;
        handler.on_progress(
                bind(&Client::Priv::progress_report, this, placeholders::_1));
        handler.on_error([prom](const net::Error& e)
        {
            prom->set_exception(make_exception_ptr(e));
        });
        handler.on_response(
                [prom,func](const http::Response& response)
                {
                    string decompressed;

                    if(!response.body.empty()) {
                        try {
                            io::filtering_ostream os;
                            os.push(io::gzip_decompressor());
                            os.push(io::back_inserter(decompressed));
                            os << response.body;
                            boost::iostreams::close(os);
                        } catch(io::gzip_error &e) {
                            prom->set_exception(make_exception_ptr(e));
                            return;
                        }
                    }

                    json::Value root;
                    json::Reader reader;
                    reader.parse(decompressed, root);

                    if (response.status != http::Status::ok) {
                        prom->set_exception(make_exception_ptr(domain_error(root["error"].asString())));
                    } else {
                        prom->set_value(func(root));
                    }
                });

        get(path, parameters, handler);

        return prom->get_future();
    }

    bool authenticated() {
        std::lock_guard<std::mutex> lock(config_mutex_);
        update_config();
        return config_.authenticated;
    }

    void anonymous_login() {
        fs::path saved_token_dir = fs::path(getenv("HOME"))
                / ".local" / "share" / "unity-scopes" / "leaf-net" / SCOPE_NAME;
        fs::path saved_token_path = saved_token_dir
                / "anonymous_auth_token";

        bool save_auth_token = getenv("VIMEO_SCOPE_IGNORE_ACCOUNTS") == nullptr;

        config_.client_id = CLIENT_ID;
        config_.client_secret = CLIENT_SECRET;

        if (fs::exists(saved_token_path) && save_auth_token) {
            ifstream in(saved_token_path.native(), ios::in | ios::binary);
            if (in) {
                ostringstream contents;
                contents << in.rdbuf();
                in.close();
                config_.access_token = contents.str();
            }
            cerr << "  re-using saved auth_token" << endl;
        } else {
            config_.access_token = unauthenticated(CLIENT_ID, CLIENT_SECRET,
                    config_.apiroot);
            if (save_auth_token) {
                fs::create_directories(saved_token_dir);
                ofstream out(saved_token_path.native(), ios::out | ios::binary);
                if (out) {
                    out << config_.access_token;
                    out.close();
                }
            }
            cerr << "  new auth_token" << endl;
        }
    }

    void update_config() {
        config_ = Config();

        if (getenv("VIMEO_SCOPE_APIROOT")) {
            config_.apiroot = getenv("VIMEO_SCOPE_APIROOT");
        }

        if (getenv("VIMEO_SCOPE_IGNORE_ACCOUNTS") != nullptr) {
            anonymous_login();
            return;
        }

        /// TODO: The code commented out below should be uncommented as soon as
        /// OnlineAccountClient::refresh_service_statuses() is fixed (Bug #1398813).
        /// For now we have to re-instantiate a new OnlineAccountClient each time.

        ///if (oa_client_ == nullptr) {
            oa_client_.reset(
                    new unity::scopes::OnlineAccountClient(SCOPE_INSTALL_NAME,
                            "sharing", SCOPE_ACCOUNTS_NAME));
        ///} else {
        ///    oa_client_->refresh_service_statuses();
        ///}

        for (auto const& status : oa_client_->get_service_statuses()) {
            if (status.service_authenticated) {
                config_.authenticated = true;
                config_.access_token = status.access_token;
                config_.client_id = status.client_id;
                config_.client_secret = status.client_secret;
                break;
            }
        }

        if (!config_.authenticated) {
            std::cerr << "Vimeo scope is unauthenticated" << std::endl;
            anonymous_login();
        } else {
            std::cerr << "Vimeo scope is authenticated" << std::endl;
        }
    }
};

Client::Client(std::shared_ptr<unity::scopes::OnlineAccountClient> oa_client) :
        p(new Priv(oa_client)) {
}

future<Client::VideoList> Client::videos(const string &query) {
    return p->async_get<VideoList>( { "videos" }, { { "query", query } },
            [](const json::Value &root) {
                return get_list<Video>(root);
            });
}

future<Client::ChannelList> Client::channels() {
    return p->async_get<ChannelList>( { "channels" }, { { "sort", "followers" },
            { "filter", "featured" }, { "per_page", "10" } },
            [](const json::Value &root) {
                return get_list<Channel>(root);
            });
}

future<Client::VideoList> Client::channels_videos(const string &channel) {
    return p->async_get<VideoList>( { "channels", channel, "videos" }, { },
            [](const json::Value &root) {
                return get_list<Video>(root);
            });
}

future<Client::VideoList> Client::feed() {
    return p->async_get<VideoList>( { "me", "feed" }, { },
            [](const json::Value &root) {
                return get_list<Video>(root);
            });
}

void Client::cancel() {
    p->cancelled_ = true;
}

bool Client::authenticated() {
    return p->authenticated();
}

