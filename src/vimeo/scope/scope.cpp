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

#include <vimeo/scope/scope.h>
#include <vimeo/scope/query.h>
#include <vimeo/scope/preview.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

namespace sc = unity::scopes;
using namespace std;
using namespace boost;
using namespace vimeo::scope;
using namespace vimeo::api;

static const char* CLIENT_ID = "b6758ff9f929cdb9f45a8477732bdbc4c6a89c7e";
static const char* CLIENT_SECRET = "a3222f38f799b3b528e29418fe062c02c677a249";

void Scope::anonymous_login(SimpleOAuth &oauth,
        SimpleOAuth::AuthData& auth_data) {
    filesystem::path saved_token_dir = filesystem::path(getenv("HOME"))
            / ".cache" / "unity-scope-vimeo";
    filesystem::path saved_token_path = saved_token_dir
            / "anonymous_auth_token";

    bool save_auth_token = getenv("VIMEO_SCOPE_IGNORE_ACCOUNTS") == nullptr;

    if (filesystem::exists(saved_token_path) && save_auth_token) {
        ifstream in(saved_token_path.native(), ios::in | ios::binary);
        if (in) {
            ostringstream contents;
            contents << in.rdbuf();
            in.close();
            auth_data.access_token = contents.str();
        }
        cerr << "  re-using saved auth_token" << endl;
    } else {
        oauth.unauthenticated(CLIENT_ID, CLIENT_SECRET,
                config_->apiroot + "/oauth/authorize/client", { { "grant_type",
                        "client_credentials" }, { "scope", "public" } });
        auth_data = oauth.auth_data();
        if (save_auth_token) {
            filesystem::create_directories(saved_token_dir);
            ofstream out(saved_token_path.native(), ios::out | ios::binary);
            if (out) {
                out << auth_data.access_token;
                out.close();
            }
        }
        cerr << "  new auth_token" << endl;
    }
}

void Scope::start(string const&, sc::RegistryProxy const&) {
    config_ = make_shared<Config>();

    config_->apiroot =
            getenv("VIMEO_SCOPE_APIROOT") ?
                    getenv("VIMEO_SCOPE_APIROOT") : "https://api.vimeo.com";
    config_->user_agent =
            "unity-scope-vimeo 0.1; (http: //developer.vimeo.com/api/docs)";
    config_->accept = "application/vnd.vimeo.*+json; version=3.0";

    SimpleOAuth oauth("vimeo");
    SimpleOAuth::AuthData auth_data;
    if (getenv("VIMEO_SCOPE_IGNORE_ACCOUNTS") == nullptr) {
        auth_data = oauth.auth_data();
    }
    if (auth_data.access_token.empty()) {
        cerr << "Vimeo scope is unauthenticated" << endl;
        anonymous_login(oauth, auth_data);
    } else {
        cerr << "Vimeo scope is authenticated" << endl;
        config_->authenticated = true;
    }

    config_->access_token = auth_data.access_token;
    config_->client_id = auth_data.client_id;
    config_->client_secret = auth_data.client_secret;
}

void Scope::stop() {
}

sc::SearchQueryBase::UPtr Scope::search(const sc::CannedQuery &query,
        const sc::SearchMetadata &metadata) {
    return sc::SearchQueryBase::UPtr(new Query(query, metadata, config_));
}

sc::PreviewQueryBase::UPtr Scope::preview(sc::Result const& result,
        sc::ActionMetadata const& metadata) {
    return sc::PreviewQueryBase::UPtr(new Preview(result, metadata));
}

#define EXPORT __attribute__ ((visibility ("default")))

extern "C" {

EXPORT
unity::scopes::ScopeBase*
// cppcheck-suppress unusedFunction
UNITY_SCOPE_CREATE_FUNCTION() {
    return new Scope();
}

EXPORT
void
// cppcheck-suppress unusedFunction
UNITY_SCOPE_DESTROY_FUNCTION(unity::scopes::ScopeBase* scope_base) {
    delete scope_base;
}

}
