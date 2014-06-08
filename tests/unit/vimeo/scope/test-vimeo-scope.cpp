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

#include <core/posix/exec.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <unity/scopes/SearchReply.h>
#include <unity/scopes/SearchReplyProxyFwd.h>
#include <unity/scopes/testing/Category.h>
#include <unity/scopes/testing/MockSearchReply.h>
#include <unity/scopes/testing/TypedScopeFixture.h>

using namespace std;
using namespace testing;
using namespace vimeo::scope;

namespace posix = core::posix;
namespace sc = unity::scopes;
namespace sct = unity::scopes::testing;

namespace {

typedef sct::TypedScopeFixture<Scope> TypedScopeFixtureScope;

class TestVimeoScope: public TypedScopeFixtureScope {
protected:
    void SetUp() override
    {
        fake_vimeo_server_ = posix::exec("/usr/bin/python",
                { FAKE_VIMEO_SERVER }, { }, posix::StandardStream::stdout);

        ASSERT_GT(fake_vimeo_server_.pid(), 0);
        string port;
        fake_vimeo_server_.cout() >> port;

        string apiroot = "http://127.0.0.1:" + port;
        setenv("VIMEO_SCOPE_APIROOT", apiroot.c_str(), true);

        // Do the parent SetUp
        TypedScopeFixtureScope::SetUp();
    }

    posix::ChildProcess fake_vimeo_server_ = posix::ChildProcess::invalid();
};

TEST_F(TestVimeoScope, search_results) {
    const sc::CategoryRenderer renderer;
    NiceMock<sct::MockSearchReply> reply;
    EXPECT_CALL(reply, register_departments(_)).Times(1);
    EXPECT_CALL(reply, register_category("vimeo", "Vimeo", "vimeo-logo-dark", _)).Times(
            1).WillOnce(
            Return(
                    sc::Category::SCPtr(
                            new sct::Category("vimeo", "Vimeo",
                                    "vimeo-logo-dark", renderer))));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(_))).Times(2).WillRepeatedly(
            Return(true));
    sc::SearchReplyProxy reply_proxy(&reply, [](sc::SearchReply*) {}); // note: this is a std::shared_ptr with empty deleter
    sc::CannedQuery query("unity-scope-vimeo", "", "");
    sc::SearchMetadata meta_data("en_EN", "phone");
    auto search_query = scope->search(query, meta_data);
    ASSERT_NE(nullptr, search_query);
    search_query->run(reply_proxy);
}

} // namespace
