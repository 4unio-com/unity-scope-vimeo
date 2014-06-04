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

#ifndef VIMEO_API_CONFIG_H_
#define VIMEO_API_CONFIG_H_

#include <memory>
#include <string>

namespace vimeo {
namespace api {

struct Config {
    typedef std::shared_ptr<Config> Ptr;

    /*
     * The access token provided at instantiation
     */
    std::string access_token { };

    /*
     * The client id provided at instantiation
     */
    std::string client_id { };

    /*
     * The secret provided at instantiation
     */
    std::string client_secret { };

    /*
     * The root of all API request URLs
     */
    std::string apiroot { };

    /*
     * The custom HTTP user agent string for this library
     */
    std::string user_agent { };

    /*
     * Default "Accept" HTTP header sent with every request if none is specified
     */
    std::string accept { };

    /**
     * Have we got access to private APIs?
     */
    bool authenticated = false;
};

}
}

#endif /* VIMEO_API_CONFIG_H_ */
