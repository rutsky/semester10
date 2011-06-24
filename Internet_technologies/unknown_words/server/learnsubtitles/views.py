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
from django.http import HttpResponse, Http404
from django.utils import simplejson
from django.template import RequestContext

from .models import CategoryNode, category_by_path, episode_by_path, \
    root_category

import dictionary

def home(request):
    root_category = CategoryNode.objects.get(name="root")
    return render_to_response('learnsubtitles/home.html', 
        { 'root_category' : root_category },
        context_instance=RequestContext(request))

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

def category_tree_node(request, path_str=None):
    context = {'root_category': root_category()}

    if path_str is not None:
        path = filter(None, path_str.split('/'))

        episode = episode_by_path(path)
        if episode is not None:
            # DEBUG
            #episode.rebuild_dictionary()

            words = None
            if episode.dictionary is not None:
                words = []
                for word in list(episode.dictionary.words.all()):
                    transl = dictionary.en2ru.get(word.name,
                        'No translation. Sorry.')
                    words.append((word.name, transl))

            context.update({'episode': episode})
            context.update({'words': words})
            return render_to_response('learnsubtitles/episode.html', context,
                context_instance=RequestContext(request))
        
        category = category_by_path(path)
        if path is not None:
            context.update({'category': category})
            return render_to_response('learnsubtitles/category.html', context,
                context_instance=RequestContext(request))

        raise Http404
    else:
        return render_to_response('learnsubtitles/home.html', context,
            context_instance=RequestContext(request))

# vim: ts=4 sw=4 et:
