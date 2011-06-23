#  This file is part of simple web site for learning unknown words from 
#  serials subtitles.
#
#  Copyright (C) 2011  Vladimir Rutsky <altsysrq@gmail.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.


from django.conf.urls.defaults import *

from . import views

urlpatterns = patterns('',
    (r'^$', views.home),
    (r'^xhr$', views.XMLHTTPRequest_handler),
    (r'^cat/$', views.category),
    (r'^cat/(.*)/$', views.category),
)

# vim: ts=4 sw=4 et:
