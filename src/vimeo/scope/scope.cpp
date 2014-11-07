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

#include <vimeo/api/login.h>
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

void Scope::anonymous_login() {
    filesystem::path saved_token_dir = filesystem::path(getenv("HOME"))
            / ".local" / "share" / "unity-scopes" / "leaf-net" / SCOPE_NAME;
    filesystem::path saved_token_path = saved_token_dir
            / "anonymous_auth_token";

    bool save_auth_token = getenv("VIMEO_SCOPE_IGNORE_ACCOUNTS") == nullptr;

    config_->client_id = CLIENT_ID;
    config_->client_secret = CLIENT_SECRET;

    if (filesystem::exists(saved_token_path) && save_auth_token) {
        ifstream in(saved_token_path.native(), ios::in | ios::binary);
        if (in) {
            ostringstream contents;
            contents << in.rdbuf();
            in.close();
            config_->access_token = contents.str();
        }
        cerr << "  re-using saved auth_token" << endl;
    } else {
        config_->access_token = unauthenticated(CLIENT_ID, CLIENT_SECRET,
                config_->apiroot);
        if (save_auth_token) {
            filesystem::create_directories(saved_token_dir);
            ofstream out(saved_token_path.native(), ios::out | ios::binary);
            if (out) {
                out << config_->access_token;
                out.close();
            }
        }
        cerr << "  new auth_token" << endl;
    }
}

void Scope::service_update(sc::OnlineAccountClient::ServiceStatus const&)
{
    update_config();
}

void Scope::update_config()
{
    std::lock_guard<std::mutex> lock(config_mutex_);
    init_config();

    for (auto const& status : oa_client_->get_service_statuses())
    {
        if (status.service_authenticated)
        {
            config_->authenticated = true;
            config_->access_token = status.access_token;
            config_->client_id = status.client_id;
            config_->client_secret = status.client_secret;
            break;
        }
    }

    if (!config_->authenticated) {
        cerr << "Vimeo scope is unauthenticated" << endl;
        anonymous_login();
    } else {
        cerr << "Vimeo scope is authenticated" << endl;
    }

    config_cond_.notify_all();
}

void Scope::init_config()
{
    config_ = make_shared<Config>();
    if (getenv("VIMEO_SCOPE_APIROOT")) {
        config_->apiroot = getenv("VIMEO_SCOPE_APIROOT");
    }
}

void Scope::start(string const&) {
    setlocale(LC_ALL, "");
    string translation_directory = ScopeBase::scope_directory()
            + "/../share/locale/";
    bindtextdomain(GETTEXT_PACKAGE, translation_directory.c_str());

    if (getenv("VIMEO_SCOPE_IGNORE_ACCOUNTS") == nullptr) {
        oa_client_.reset(
                new sc::OnlineAccountClient(SCOPE_INSTALL_NAME,
                        "sharing", SCOPE_ACCOUNTS_NAME));
        oa_client_->set_service_update_callback(
                std::bind(&Scope::service_update, this, std::placeholders::_1));

        ///! TODO: We should only be waiting here if we know that there is at least one Google account enabled.
        ///        OnlineAccountClient needs to expose some functionality for us to determine that.

        // Allow 1 second for the callback to initialize config_
        std::unique_lock<std::mutex> lock(config_mutex_);
        config_cond_.wait_for(lock, std::chrono::seconds(1), [this] { return config_ != nullptr; });
    }

    if (config_ == nullptr)
    {
        // If the callback was not invoked, default initialize config_
        init_config();
        cerr << "Vimeo scope is unauthenticated" << endl;
        anonymous_login();
    }
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
