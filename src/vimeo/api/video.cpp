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

#include <vimeo/api/video.h>

#include <json/json.h>

namespace json = Json;
using namespace vimeo::api;
using namespace std;

Video::Video(const json::Value &data) {
    name_ = data["name"].asString();
    uri_ = data["link"].asString();
    description_ = data["description"].asString();

    json::Value user = data["user"];
    username_ = user["name"].asString();

    json::Value pictures = data["pictures"];
    json::Value picture = pictures.get(json::ArrayIndex(0), "{}");
    picture_ = picture["link"].asString();
}

const std::string & Video::name() const {
    return name_;
}

const std::string & Video::username() const {
    return username_;
}

const std::string & Video::uri() const {
    return uri_;
}

const std::string & Video::picture() const {
    return picture_;
}

const std::string & Video::description() const {
    return description_;
}
