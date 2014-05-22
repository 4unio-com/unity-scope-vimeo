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

#include <vimeo-preview.h>

#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewReply.h>

namespace sc = unity::scopes;
using namespace std;

VimeoPreview::VimeoPreview(string const& uri) :
        uri_(uri) {
}

void VimeoPreview::cancelled() {
}

void VimeoPreview::run(sc::PreviewReplyProxy const& reply) {
    sc::PreviewWidgetList widgets;
    widgets.emplace_back(
            sc::PreviewWidget(
                    R"({"id": "header", "type": "header", "components" : { "title": "title", "subtitle": "author" } })"));
    widgets.emplace_back(
            sc::PreviewWidget(
                    R"({"id": "img", "type": "image", "components" : { "source": "screenshot-url" } })"));

    sc::PreviewWidget w("img2", "image");
    w.add_attribute_value("zoomable", sc::Variant(false));
    w.add_attribute_mapping("source", "screenshot-url");
    widgets.emplace_back(w);

    sc::ColumnLayout layout1col(1);
    layout1col.add_column( { "header", "img", "img2" });

    sc::ColumnLayout layout2col(2);
    layout2col.add_column( { "header", "img" });
    layout2col.add_column( { "img2" });

    sc::ColumnLayout layout3col(3);
    layout3col.add_column( { "header" });
    layout3col.add_column( { "img" });
    layout3col.add_column( { "img2" });

    reply->register_layout( { layout1col, layout2col, layout3col });
    reply->push(widgets);
    reply->push("author", sc::Variant("Foo"));
    reply->push("screenshot-url", sc::Variant("/path/to/image.png"));
}
