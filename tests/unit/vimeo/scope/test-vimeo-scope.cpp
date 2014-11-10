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
#include <unity/scopes/Variant.h>
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

MATCHER_P2(ResultProp, prop, value, "") {
    if (arg.contains(prop)) {
        *result_listener << "result[" << prop << "] is " << arg[prop].serialize_json();
    } else {
        *result_listener << "result[" << prop << "] is not set";
    }
    return arg.contains(prop) && arg[prop] == sc::Variant(value);
}

MATCHER_P(IsDepartment, department, "") {
    return arg->serialize() == department->serialize();
}

typedef sct::TypedScopeFixture<Scope> TypedScopeFixtureScope;

class TestVimeoScope: public TypedScopeFixtureScope {
protected:
    void SetUp() override
    {
        fake_vimeo_server_ = posix::exec(FAKE_VIMEO_SERVER, { }, { },
                posix::StandardStream::stdout);

        ASSERT_GT(fake_vimeo_server_.pid(), 0);
        string port;
        fake_vimeo_server_.cout() >> port;

        string apiroot = "http://127.0.0.1:" + port;
        setenv("VIMEO_SCOPE_APIROOT", apiroot.c_str(), true);

        setenv("VIMEO_SCOPE_IGNORE_ACCOUNTS", "true", true);

        // Do the parent SetUp
        TypedScopeFixture::set_scope_directory(TEST_SCOPE_DIRECTORY);
        TypedScopeFixtureScope::SetUp();
    }

    posix::ChildProcess fake_vimeo_server_ = posix::ChildProcess::invalid();
};

TEST_F(TestVimeoScope, empty_search_string) {
    const sc::CategoryRenderer renderer;
    NiceMock<sct::MockSearchReply> reply;

    sc::CannedQuery query(SCOPE_NAME, "", ""); // Searching with empty query and no department

    sc::Department::SPtr departments = sc::Department::create("", query,
            "My Feed");
    departments->add_subdepartment(sc::Department::create("1", query, "Staff Picks"));
    departments->add_subdepartment(sc::Department::create("2", query, "Apple"));
    departments->add_subdepartment(sc::Department::create("3", query, "Banana"));
    departments->add_subdepartment(sc::Department::create("4", query, "Cherry"));
    EXPECT_CALL(reply, register_departments(IsDepartment(departments))).Times(1);

    EXPECT_CALL(reply, register_category("vimeo", "", "", _)).Times(1)
            .WillOnce(Return(make_shared<sct::Category>("vimeo", "", "", renderer)));

    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
            ResultProp("uri", "http://vimeo.com/videos/1"),
            ResultProp("title", "Video 1"),
            ResultProp("art", "http://images.foo/picture1-1.png"),
            ResultProp("description", "Description 1"),
            ResultProp("username", "user 1"))))).WillOnce(
            Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
            ResultProp("uri", "http://vimeo.com/videos/2"),
            ResultProp("title", "Video 2"),
            ResultProp("art", "http://images.foo/picture2-1.png"),
            ResultProp("description", "Description 2"),
            ResultProp("username", "user 2"))))).WillOnce(
            Return(true));

    sc::SearchReplyProxy reply_proxy(&reply, [](sc::SearchReply*) {}); // note: this is a std::shared_ptr with empty deleter
    sc::SearchMetadata meta_data("en_EN", "phone");
    auto search_query = scope->search(query, meta_data);
    ASSERT_NE(nullptr, search_query);
    search_query->run(reply_proxy);
}

TEST_F(TestVimeoScope, apple_department) {
    const sc::CategoryRenderer renderer;
    NiceMock<sct::MockSearchReply> reply;

    sc::CannedQuery query(SCOPE_NAME, "", "2"); // searching department "2" == Apple

    sc::Department::SPtr departments = sc::Department::create("", query,
                "My Feed");
    departments->add_subdepartment(sc::Department::create("1", query, "Staff Picks"));
    departments->add_subdepartment(sc::Department::create("2", query, "Apple"));
    departments->add_subdepartment(sc::Department::create("3", query, "Banana"));
    departments->add_subdepartment(sc::Department::create("4", query, "Cherry"));
    EXPECT_CALL(reply, register_departments(IsDepartment(departments))).Times(1);

    EXPECT_CALL(reply, register_category("vimeo", "", "", _)).Times(1)
                .WillOnce(Return(make_shared<sct::Category>("vimeo", "", "", renderer)));

    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
            ResultProp("uri", "http://vimeo.com/videos/apple_1"),
            ResultProp("title", "Apple 1"),
            ResultProp("art", "http://images.foo/apple1-1.png"),
            ResultProp("description", "Apple description 1"),
            ResultProp("username", "apple 1"))))).WillOnce(
            Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
            ResultProp("uri", "http://vimeo.com/videos/apple_2"),
            ResultProp("title", "Apple 2"),
            ResultProp("art", "http://images.foo/apple2-1.png"),
            ResultProp("description", "Apple description 2"),
            ResultProp("username", "apple 2"))))).WillOnce(
            Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
            ResultProp("uri", "http://vimeo.com/videos/apple_3"),
            ResultProp("title", "Apple 3"),
            ResultProp("art", "http://images.foo/apple3-1.png"),
            ResultProp("description", "Apple description 3"),
            ResultProp("username", "apple 3"))))).WillOnce(
            Return(true));

    sc::SearchReplyProxy reply_proxy(&reply, [](sc::SearchReply*) {}); // note: this is a std::shared_ptr with empty deleter
    sc::SearchMetadata meta_data("en_EN", "phone");
    auto search_query = scope->search(query, meta_data);
    ASSERT_NE(nullptr, search_query);
    search_query->run(reply_proxy);
}

TEST_F(TestVimeoScope, non_empty_query) {
    const sc::CategoryRenderer renderer;
    NiceMock<sct::MockSearchReply> reply;

    sc::CannedQuery query(SCOPE_NAME, "query", ""); // searching with query text

    EXPECT_CALL(reply, register_category("vimeo", "", "", _)).Times(1)
                .WillOnce(Return(make_shared<sct::Category>("vimeo", "", "", renderer)));

    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
            ResultProp("uri", "http://vimeo.com/videos/query_1"),
            ResultProp("title", "Query video 1"),
            ResultProp("art", "http://images.foo/query1-1.png"),
            ResultProp("description", "Query description 1"),
            ResultProp("username", "Query user 1"))))).WillOnce(
            Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
            ResultProp("uri", "http://vimeo.com/videos/query_2"),
            ResultProp("title", "Query video 2"),
            ResultProp("art", "http://images.foo/query2-1.png"),
            ResultProp("description", "Query description 2"),
            ResultProp("username", "Query user 2"))))).WillOnce(
            Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
            ResultProp("uri", "http://vimeo.com/videos/query_3"),
            ResultProp("title", "Query video 3"),
            ResultProp("art", "http://images.foo/query3-1.png"),
            ResultProp("description", "Query description 3"),
            ResultProp("username", "Query user 3"))))).WillOnce(
            Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
            ResultProp("uri", "http://vimeo.com/videos/query_4"),
            ResultProp("title", "Query video 4"),
            ResultProp("art", "http://images.foo/query4-1.png"),
            ResultProp("description", "Query description 4"),
            ResultProp("username", "Query user 4"))))).WillOnce(
            Return(true));

    sc::SearchReplyProxy reply_proxy(&reply, [](sc::SearchReply*) {}); // note: this is a std::shared_ptr with empty deleter
    sc::SearchMetadata meta_data("en_EN", "phone");
    auto search_query = scope->search(query, meta_data);
    ASSERT_NE(nullptr, search_query);
    search_query->run(reply_proxy);
}

} // namespace
