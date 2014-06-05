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

#include <boost/algorithm/string/trim.hpp>

#include <vimeo/scope/query.h>

#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>

namespace sc = unity::scopes;
namespace alg = boost::algorithm;

using namespace std;
using namespace vimeo::api;
using namespace vimeo::scope;

const static string SEARCH_CATEGORY_TEMPLATE = ""
        "{"
        "  \"schema-version\": 1,"
        "  \"template\": {"
        "    \"category-layout\": \"grid\","
        "    \"card-size\": \"large\","
        "    \"overlay\": true"
        "  },"
        "  \"components\": {"
        "    \"title\": \"title\","
        "    \"art\" : {"
        "      \"field\": \"art\","
        "      \"aspect-ratio\": 2.0"
        "    },"
        "    \"subtitle\": \"username\""
        "  }"
        "}";

Query::Query(const sc::CannedQuery &query, Config::Ptr config) :
        query_(query), client_(config) {
}

void Query::cancelled() {
    client_.cancel();
}

void Query::run(sc::SearchReplyProxy const& reply) {
    try {

        sc::Department::SPtr all_depts = sc::Department::create("", query_,
                "All");
        for (Channel::Ptr channel : client_.channels()) {
            cerr << channel->name() << ", " << channel->id() << endl;
            sc::Department::SPtr dept = sc::Department::create(channel->id(),
                    query_, channel->name());
            all_depts->add_subdepartment(dept);
        }
        reply->register_departments(all_depts);

//        Filters filters;
//        OptionSelectorFilter::SPtr filter = OptionSelectorFilter::create("f1",
//                "Options");
//        filter->add_option("1", "Option 1");
//        filter->add_option("2", "Option 2");
//        filters.push_back(filter);
//        FilterState filter_state; // TODO: push real state from query obj
//        if (!reply->push(filters, filter_state)) {
//            return;  // Query was cancelled
//        }

        sc::Category::SCPtr cat;

        Client::VideoList videos;

        string query = alg::trim_copy(query_.query_string());

        if (query.empty()) {
            if (client_.config()->authenticated) {
                videos = client_.feed();
            } else {
                videos = client_.channels_videos("staffpicks");
            }

            cat = reply->register_category("vimeo-feed", "Feed",
                    "vimeo-logo-dark",
                    sc::CategoryRenderer(SEARCH_CATEGORY_TEMPLATE));
        } else {
            cat = reply->register_category("vimeo", "Vimeo", "vimeo-logo-dark",
                    sc::CategoryRenderer(SEARCH_CATEGORY_TEMPLATE));
            videos = client_.videos(query);
        }

        for (Video::Ptr video : videos) {
            sc::CategorisedResult res(cat);
            res.set_uri(video->uri());
            res.set_title(video->name());
            res.set_art(video->picture());
            res["description"] = video->description();
            res["username"] = video->username();

            if (!reply->push(res)) {
                return;
            }
        }
    } catch (domain_error &e) {
    }
}
