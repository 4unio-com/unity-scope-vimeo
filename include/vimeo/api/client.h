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

#ifndef VIMEO_API_CLIENT_H_
#define VIMEO_API_CLIENT_H_

#include <vimeo/api/channel.h>
#include <vimeo/api/config.h>
#include <vimeo/api/video.h>

#include <atomic>
#include <deque>
#include <map>
#include <string>
#include <core/net/http/request.h>

namespace Json {
class Value;
}

namespace vimeo {
namespace api {

class Client {
public:
    typedef std::deque<Channel::Ptr> ChannelList;

    typedef std::deque<Video::Ptr> VideoList;

    Client(Config::Ptr config);

    virtual ~Client() = default;

    virtual ChannelList channels();

    virtual VideoList videos(const std::string &query = std::string());

    virtual VideoList channels_videos(const std::string &channel);

    virtual VideoList feed();

    virtual void cancel();

    virtual Config::Ptr config();

protected:
    void get(const std::deque<std::string> &endpoint,
            const std::map<std::string, std::string> &querys,
            Json::Value &root);

    core::net::http::Request::Progress::Next progress_report(
            const core::net::http::Request::Progress& progress);

    Config::Ptr config_;

    std::atomic<bool> cancelled_;
};

}
}

#endif // VIMEO_API_CLIENT_H_
