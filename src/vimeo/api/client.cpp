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

#include <core/net/error.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/response.h>
#include <json/json.h>

namespace http = core::net::http;
namespace json = Json;
namespace net = core::net;

using namespace vimeo::api;
using namespace std;

Client::Client(Config::Ptr config) :
        config_(config), cancelled_(false) {
}

void Client::get(const deque<string> &endpoints,
        const map<string, string> &querys, json::Value &root) {
    auto client = http::make_client();

    http::Request::Configuration configuration;
    configuration.uri = config_->apiroot;
    for (const string &endpoint : endpoints) {
        configuration.uri.append("/" + endpoint);
    }
    bool first = true;
    for (auto it : querys) {
        if (first) {
            configuration.uri.append("?");
            first = false;
        } else {
            configuration.uri.append("&");
        }
        configuration.uri.append(client->url_escape(it.first));
        configuration.uri.append("=");
        configuration.uri.append(client->url_escape(it.second));
    }

    if (!config_->access_token.empty()) {
        configuration.header.add("Authorization",
                "bearer " + config_->access_token);
    } else if (!config_->client_id.empty() && !config_->client_secret.empty()) {
        string auth = "basic "
                + client->base64_encode(
                        config_->client_id + ":" + config_->client_secret);
        configuration.header.add("Authorization", auth);
    }
    configuration.header.add("Accept", config_->accept);
    configuration.header.add("User-Agent", config_->user_agent);

    auto request = client->head(configuration);

    try {
        auto response = request->execute(
                bind(&Client::progress_report, this, placeholders::_1));

        json::Reader reader;
        reader.parse(response.body, root);

        if (response.status != http::Status::ok) {
            cerr << "ERROR: " << response.body << endl;
            throw domain_error(root["error"].asString());
        }
    } catch (net::Error &) {
    }
}

template<typename T>
static deque<shared_ptr<T>> get_list(const json::Value &root) {
    deque<shared_ptr<T>> results;
    json::Value data = root["data"];
    for (json::ArrayIndex index = 0; index < data.size(); ++index) {
        results.push_back(make_shared<T>(data[index]));
    }
    return results;
}

Client::VideoList Client::videos(const string &query) {
    json::Value root;
    get( { "videos" }, { { "query", query } }, root);
    return get_list<Video>(root);
}

Client::ChannelList Client::channels() {
    json::Value root;
    get( { "channels" }, { { "sort", "followers" } }, root);
    return get_list<Channel>(root);
}

Client::VideoList Client::channels_videos(const string &channel) {
    json::Value root;
    get( { "channels", channel, "videos" }, { }, root);
    return get_list<Video>(root);
}

Client::VideoList Client::feed() {
    json::Value root;
    get( { "me", "feed" }, { }, root);
    return get_list<Video>(root);
}

http::Request::Progress::Next Client::progress_report(
        const http::Request::Progress&) {

    return cancelled_ ?
            http::Request::Progress::Next::abort_operation :
            http::Request::Progress::Next::continue_operation;
}

void Client::cancel() {
    cancelled_ = true;
}

Config::Ptr Client::config() {
    return config_;
}
