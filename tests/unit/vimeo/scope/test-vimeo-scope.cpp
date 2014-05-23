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

#include <core/net/error.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/request.h>
#include <core/net/http/response.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <json/json.h>

namespace http = core::net::http;
namespace json = Json;
namespace net = core::net;

using namespace std;
using namespace testing;

namespace {

auto default_progress_reporter =
        [](const http::Request::Progress& progress)
        {
            return http::Request::Progress::Next::continue_operation;
        };

class TestVimeoScope: public Test {
protected:
};

TEST_F(TestVimeoScope, Foo) {
    auto client = http::make_client();
    string url = "http://vimeo.com/api/v2/channel/delicioussandwich/info.json";
    auto request = client->head(
            http::Request::Configuration::from_uri_as_string(url));
    auto response = request->execute(default_progress_reporter);

    json::Value root;
    json::Reader reader;
    reader.parse(response.body, root);
    EXPECT_EQ("A Sandwich Blog", root["description"].asString());
}

} // namespace
