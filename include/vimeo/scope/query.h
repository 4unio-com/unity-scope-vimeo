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

#ifndef VIMEO_SCOPE_QUERY_H_
#define VIMEO_SCOPE_QUERY_H_

#include <vimeo/api/client.h>

#include <unity/scopes/SearchQueryBase.h>
#include <unity/scopes/ReplyProxyFwd.h>

namespace vimeo {
namespace scope {

class Query: public unity::scopes::SearchQueryBase {
public:
    Query(std::string const& query);

    ~Query() = default;

    void cancelled() override;

    void run(unity::scopes::SearchReplyProxy const& reply) override;

private:
    std::string query_;

    vimeo::api::Client client_;
};

}
}

#endif // VIMEO_SCOPE_QUERY_H_

