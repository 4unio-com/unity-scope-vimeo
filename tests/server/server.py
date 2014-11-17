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

import base64
import json
import tornado.httpserver
import tornado.ioloop
import tornado.netutil
import tornado.web
import sys

CLIENT_ID = 'b6758ff9f929cdb9f45a8477732bdbc4c6a89c7e'
CLIENT_SECRET = 'a3222f38f799b3b528e29418fe062c02c677a249'

ACCESS_TOKEN = 'the_access_token'
ACCESS_TOKEN_REPLY = {'access_token': 'the_access_token'}

AUTHORIZATION_BASIC = 'basic %s' % base64.b64encode('%s:%s' % (CLIENT_ID, CLIENT_SECRET))
AUTHORIZATION_BEARER = 'bearer %s' % ACCESS_TOKEN

CHANNELS = {
    'data': [
        {'name': 'Staff Picks', 'uri': '/channels/1'},
        {'name': 'Apple', 'uri': '/channels/2'},
        {'name': 'Banana', 'uri': '/channels/3'},
        {'name': 'Cherry', 'uri': '/channels/4'},
    ]
}

QUERY_VIDEOS = {
    'data': [
        {
            'name': 'Query video 1',
            'link': 'http://vimeo.com/videos/query_1',
            'description': 'Query description 1',
            'user': {'name': 'Query user 1'},
            'pictures': [
                {'link': 'http://images.foo/query1-1.png'},
                {'link': 'http://images.foo/query1-2.png'},
                {'link': 'http://images.foo/query1-3.png'},
            ],
        },
        {
            'name': 'Query video 2',
            'link': 'http://vimeo.com/videos/query_2',
            'description': 'Query description 2',
            'user': {'name': 'Query user 2'},
            'pictures': [
                {'link': 'http://images.foo/query2-1.png'},
                {'link': 'http://images.foo/query2-2.png'},
            ],
        },
        {
            'name': 'Query video 3',
            'link': 'http://vimeo.com/videos/query_3',
            'description': 'Query description 3',
            'user': {'name': 'Query user 3'},
            'pictures': [
                {'link': 'http://images.foo/query3-1.png'},
                {'link': 'http://images.foo/query3-2.png'},
            ],
        },
        {
            'name': 'Query video 4',
            'link': 'http://vimeo.com/videos/query_4',
            'description': 'Query description 4',
            'user': {'name': 'Query user 4'},
            'pictures': [
                {'link': 'http://images.foo/query4-1.png'},
                {'link': 'http://images.foo/query4-2.png'},
            ],
        },
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

APPLE_VIDEOS = {
    'data': [
        {
            'name': 'Apple 1',
            'link': 'http://vimeo.com/videos/apple_1',
            'description': 'Apple description 1',
            'user': {'name': 'apple 1'},
            'pictures': [
                {'link': 'http://images.foo/apple1-1.png'},
                {'link': 'http://images.foo/apple1-2.png'},
            ],
        },
        {
            'name': 'Apple 2',
            'link': 'http://vimeo.com/videos/apple_2',
            'description': 'Apple description 2',
            'user': {'name': 'apple 2'},
            'pictures': [
                {'link': 'http://images.foo/apple2-1.png'},
                {'link': 'http://images.foo/apple2-2.png'},
            ],
        },
        {
            'name': 'Apple 3',
            'link': 'http://vimeo.com/videos/apple_3',
            'description': 'Apple description 3',
            'user': {'name': 'apple 3'},
            'pictures': [
                {'link': 'http://images.foo/apple3-1.png'},
                {'link': 'http://images.foo/apple3-2.png'},
            ],
        },
    ]
}

BANANA_VIDEOS = {
    'data': []
}

CHERRY_VIDEOS = {
    'data': []
}

class ErrorHandler(tornado.web.RequestHandler):
    def write_error(self, status_code, **kwargs):
        self.write(json.dumps({'error': '%s: %d' % (kwargs["exc_info"][1], status_code)}))

class AuthlessLogin(ErrorHandler):
    def post(self):
        validate_argument(self, 'grant_type', 'client_credentials')
        validate_argument(self, 'scope', 'public')
        validate_header(self, 'Authorization', AUTHORIZATION_BASIC)
        
        self.write(json.dumps(ACCESS_TOKEN_REPLY))
        self.finish()
        
class Channels(ErrorHandler):
    def get(self):
        validate_argument(self, 'filter', 'featured')
        validate_argument(self, 'per_page', '10')
        validate_argument(self, 'sort', 'followers')
        validate_header(self, 'Authorization', AUTHORIZATION_BEARER)

        self.write(json.dumps(CHANNELS))
        self.finish()
        
class ChannelsVideos(ErrorHandler):
    def get(self, channel):
        validate_header(self, 'Authorization', AUTHORIZATION_BEARER)
    
        if channel == 'staffpicks' or channel == '1':
            self.write(json.dumps(STAFFPICKS_VIDEOS))
        elif channel == '2':
            self.write(json.dumps(APPLE_VIDEOS))
        elif channel == '3':
            self.write(json.dumps(BANANA_VIDEOS))
        elif channel == '4':
            self.write(json.dumps(CHERRY_VIDEOS))
        else:
            raise Exception("Unknown channel '%s'" % channel)
        self.finish()
        
class Videos(ErrorHandler):
    def get(self):
        validate_header(self, 'Authorization', AUTHORIZATION_BEARER)
        
        query = self.get_argument('query', '')
    
        if query == 'query':
            self.write(json.dumps(QUERY_VIDEOS))
        else:
            raise Exception("Unknown query '%s'" % query)
        self.finish()

def validate_argument(self, name, expected):
    actual = self.get_argument(name, '')
    if actual != expected:
        raise Exception("Argument '%s' == '%s' != '%s'" % (name, actual, expected))

def validate_header(self, name, expected):
    actual = self.request.headers.get(name, '')
    if actual != expected:
        raise Exception("Header '%s' == '%s' != '%s'" % (name, actual, expected))

def new_app():
    application = tornado.web.Application([
        (r"/oauth/authorize/client", AuthlessLogin),
        (r"/channels", Channels),
        (r"/channels/(.*)/videos", ChannelsVideos),
        (r"/videos", Videos),
    ], gzip=True)
    sockets = tornado.netutil.bind_sockets(0, '127.0.0.1')
    server = tornado.httpserver.HTTPServer(application)
    server.add_sockets(sockets)

    sys.stdout.write('%d\n' % sockets[0].getsockname()[1])
    sys.stdout.flush()

    return application

if __name__ == "__main__":
    application = new_app()
    tornado.ioloop.IOLoop.instance().start()
