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


#ifndef VIMEO_API_LOGIN_H_
#define VIMEO_API_LOGIN_H_

#include <vimeo/api/config.h>

namespace vimeo {
namespace api {

std::string unauthenticated(const std::string &client_id,
        const std::string &client_secret, const std::string &api_root);
}
}

#endif // VIMEO_API_LOGIN_H_
