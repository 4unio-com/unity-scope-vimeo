#!/usr/bin/env python
#
# Copyright (C) 2014 Canonical Ltd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Pete Woods <pete.woods@canonical.com>

import json

import tornado.httpserver
import tornado.ioloop
import tornado.netutil
import tornado.web

import sys

ACCESS_TOKEN = {'access_token': 'the_access_token'}

CHANNELS = {
    'data': [
        {'name': 'Staff Picks', 'uri': '/channels/1'},
        {'name': 'Apple', 'uri': '/channels/2'},
        {'name': 'Banana', 'uri': '/channels/3'},
        {'name': 'Cherry', 'uri': '/channels/4'},
    ]
}

STAFFPICKS_VIDEOS = {
    'data': [
        {
            'name': 'Video 1',
            'link': 'http://vimeo.com/videos/1',
            'description': 'Description 1',
            'user': {'name': 'user 1'},
            'pictures': [
                {'link': 'http://images.foo/picture1-1.png'},
                {'link': 'http://images.foo/picture1-2.png'},
            ],
        },
        {
            'name': 'Video 2',
            'link': 'http://vimeo.com/videos/2',
            'description': 'Description 2',
            'user': {'name': 'user 2'},
            'pictures': [
                {'link': 'http://images.foo/picture2-1.png'},
                {'link': 'http://images.foo/picture2-2.png'},
            ],
        },
    ]
}

class ErrorHandler(tornado.web.RequestHandler):
    def write_error(self, status_code, **kwargs):
        self.write(json.dumps({'error': '%s: %d' % (kwargs["exc_info"][1], status_code)}))

class AuthlessLogin(ErrorHandler):
    def post(self):
        validate_argument(self, 'grant_type', 'client_credentials')
        validate_argument(self, 'scope', 'public')
            
        if not self.request.headers.get('Authorization', ''):
            raise Exception('No authorization header')
        
        self.write(json.dumps(ACCESS_TOKEN))
        self.finish()
        
class Channels(ErrorHandler):
    def get(self):
        validate_argument(self, 'filter', 'featured')
        validate_argument(self, 'per_page', '10')
        validate_argument(self, 'sort', 'followers')

        self.write(json.dumps(CHANNELS))
        self.finish()
        
class ChannelsVideos(ErrorHandler):
    def get(self, channel):
        if channel == 'staffpicks':
            self.write(json.dumps(STAFFPICKS_VIDEOS))
        else:
            raise Exception("Unknown channel '%s'" % channel)
        self.finish()

def validate_argument(self, name, value):
    if self.get_argument(name, '') != value:
        raise Exception("Argument '%s' != '%s'" % (name, value))

def new_app():
    application = tornado.web.Application([
        (r"/oauth/authorize/client", AuthlessLogin),
        (r"/channels", Channels),
        (r"/channels/(.*)/videos", ChannelsVideos),
    ])
    sockets = tornado.netutil.bind_sockets(0, '127.0.0.1')
    server = tornado.httpserver.HTTPServer(application)
    server.add_sockets(sockets)

    sys.stdout.write('%d\n' % sockets[0].getsockname()[1])
    sys.stdout.flush()

    return application

if __name__ == "__main__":
    application = new_app()
    tornado.ioloop.IOLoop.instance().start()
