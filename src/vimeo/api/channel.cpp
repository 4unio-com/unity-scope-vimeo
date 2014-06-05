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

#include <vimeo/api/channel.h>

#include <iostream>
#include <json/json.h>

namespace json = Json;

using namespace vimeo::api;
using namespace std;

Channel::Channel(const json::Value &data) {
    name_ = data["name"].asString();

    string uri = data["uri"].asString();
    if (uri.find("/channels/") == 0) {
        id_ = uri.substr(10);
    }
}

const std::string & Channel::name() const {
    return name_;
}

const std::string & Channel::id() const {
    return id_;
}
