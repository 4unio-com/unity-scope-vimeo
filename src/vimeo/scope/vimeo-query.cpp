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

#include <vimeo/scope/vimeo-query.h>

#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>

namespace sc = unity::scopes;
using namespace std;
using namespace vimeo::scope;

VimeoQuery::VimeoQuery(string const& query) :
    query_(query)
{
}

void VimeoQuery::cancelled()
{
}

void VimeoQuery::run(sc::SearchReplyProxy const& reply)
{
    sc::CategoryRenderer rdr;
    auto cat = reply->register_category("cat1", "Category 1", "", rdr);
    sc::CategorisedResult res(cat);
    res.set_uri("uri");
    res.set_title("scope-A: result 1 for query \"" + query_ + "\"");
    res.set_art("icon");
    res.set_dnd_uri("dnd_uri");
    reply->push(res);

    sc::CannedQuery q("scope-A", query_, "");
    sc::Annotation annotation(sc::Annotation::Type::Link);
    annotation.add_link("More...", q);
    reply->register_annotation(annotation);

}
