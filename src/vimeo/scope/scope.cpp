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

#include <vimeo/api/simple-oauth.h>

#include <vimeo/scope/scope.h>
#include <vimeo/scope/query.h>
#include <vimeo/scope/preview.h>

#include <iostream>

namespace sc = unity::scopes;
using namespace std;
using namespace vimeo::scope;
using namespace vimeo::api;

static const char* CLIENT_ID = "b6758ff9f929cdb9f45a8477732bdbc4c6a89c7e";
static const char* CLIENT_SECRET = "a3222f38f799b3b528e29418fe062c02c677a249";

int Scope::start(string const&, sc::RegistryProxy const&) {
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
        oauth.unauthenticated(CLIENT_ID, CLIENT_SECRET,
                config_->apiroot + "/oauth/authorize/client", { { "grant_type",
                        "client_credentials" }, { "scope", "public" } });
        auth_data = oauth.auth_data();
    } else {
        config_->authenticated = true;
    }

    config_->access_token = auth_data.access_token;
    config_->client_id = auth_data.client_id;
    config_->client_secret = auth_data.client_secret;

    return VERSION;
}

void Scope::stop() {
}

sc::SearchQueryBase::UPtr Scope::search(sc::CannedQuery const &q,
        sc::SearchMetadata const&) {
    return sc::SearchQueryBase::UPtr(new Query(q, config_));
}

sc::PreviewQueryBase::UPtr Scope::preview(sc::Result const& result,
        sc::ActionMetadata const& /*metadata*/) {
    return sc::PreviewQueryBase::UPtr(new Preview(result));
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
