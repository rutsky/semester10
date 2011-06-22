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
from django.shortcuts import render_to_response
from django.http import HttpResponse
from django.utils import simplejson

from .models import CategoryNode

def home(request):
    root_category = CategoryNode.objects.get(name="root")
    return render_to_response('learnsubtitles/home.html', 
        { 'root_category' : root_category })

def _process_XMLHTTPRequest(request, data):
    print data
    return data

def XMLHTTPRequest_handler(request):
    if request.is_ajax() and request.method == 'POST' and \
            'data' in request.POST:
        try:
            input_data = simplejson.loads(request.POST['data'])
        except ValueError:
            return HttpResponse(status=400)
        
        data = _process_XMLHTTPRequest(request, input_data)
        serialized_data = simplejson.dumps(data)
        return HttpResponse(serialized_data, 'application/javascript')
    else:
        return HttpResponse(status=400)

# vim: ts=4 sw=4 et:
