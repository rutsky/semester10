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

from django import template
from django.utils.http import urlquote

register = template.Library()

def _quote_name(name):
    return urlquote(name, safe="")

@register.inclusion_tag('learnsubtitles/category_item.html')
def category_tree(category):
    """Prepares context for displaying single category children nodes 
    template."""

    # TODO: May be sort them?
    children = category.children.all()
    category_path = [_quote_name(cat.name) for cat in category.path()]
    children_data = [
        (child, '/'.join(category_path + [_quote_name(child.name)]))
            for child in children]

    # Returns context for `category_item.html' template.
    return { 'children': children_data }

@register.inclusion_tag('learnsubtitles/episodes_list.html')
def episodes_list(category):
    """Prepares context for displaying single category episodes list
    template."""

    # TODO: May be sort them?
    episodes = category.episodes.all()
    category_path = [_quote_name(cat.name) for cat in category.path()]
    episodes_data = [
        (episode, '/'.join(category_path + [_quote_name(episode.name)]))
            for episode in episodes]

    return { 'episodes': episodes_data }

# vim: ts=4 sw=4 et:
