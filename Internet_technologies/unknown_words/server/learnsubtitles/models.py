#  This file is part of simple web site for learning unknown words from 
#  serial's subtitles.
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

from django.db import models

class CategoryNode(models.Model):
    parent = models.ForeignKey('self', related_name='children', blank=True, 
            null=True)
    name = models.CharField(max_length=300)

    def __unicode__(self):
        return self.name

    def path(self):
        """Returns path to current category: list of categories from root to
        current category, starting with first non-root parent and ending with 
        category himself (for root category it is empty list).
        """
        path = []
        p = self
        while p.name != "root":
            path.append(p)
            p = p.parent
        path.reverse()

        return path

class Episode(models.Model):
    category = models.ForeignKey(CategoryNode, related_name='episodes')
    
    name = models.CharField(max_length=300)
    description = models.TextField()
    
    subtitles = models.TextField()
    # TODO: Dictionaty etc.

    def __unicode__(self):
        return self.name

def root_category():
    return CategoryNode.objects.get(name="root")

def category_by_path(path):
    """Find category by path to it.
    If category with provided path doesn't exists returns None.
    """
    category = root_category()
    for item in path:
        try:
            child = category.children.get(name=item)
        except CategoryNode.DoesNotExist:
            return None
        category = child
    return category

def episode_by_path(path):
    """Find episode by path to it.
    If episode with provided path doesn't exists returns None.
    """
    assert len(path) >= 1

    category = category_by_path(path[:-1])
    if category is None:
        return None

    try:
        return category.episodes.get(name=path[-1])
    except Episode.DoesNotExist:
        return None

# vim: ts=4 sw=4 et:
