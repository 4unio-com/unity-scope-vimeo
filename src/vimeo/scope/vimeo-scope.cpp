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

#include <vimeo/scope/vimeo-scope.h>
#include <vimeo/scope/vimeo-query.h>
#include <vimeo/scope/vimeo-preview.h>

namespace sc = unity::scopes;
using namespace std;
using namespace vimeo::scope;

int VimeoScope::start(string const&, sc::RegistryProxy const&) {
    return VERSION;
}

void VimeoScope::stop() {
}

sc::SearchQueryBase::UPtr VimeoScope::search(sc::CannedQuery const &q,
        sc::SearchMetadata const&) {
    sc::SearchQueryBase::UPtr query(new VimeoQuery(q.query_string()));
    return query;
}

sc::PreviewQueryBase::UPtr VimeoScope::preview(sc::Result const& result,
        sc::ActionMetadata const& /*metadata*/) {
    sc::PreviewQueryBase::UPtr preview(new VimeoPreview(result.uri()));
    return preview;
}

#define EXPORT __attribute__ ((visibility ("default")))

extern "C" {

EXPORT
unity::scopes::ScopeBase*
// cppcheck-suppress unusedFunction
UNITY_SCOPE_CREATE_FUNCTION() {
    return new VimeoScope();
}

EXPORT
void
// cppcheck-suppress unusedFunction
UNITY_SCOPE_DESTROY_FUNCTION(unity::scopes::ScopeBase* scope_base) {
    delete scope_base;
}

}
