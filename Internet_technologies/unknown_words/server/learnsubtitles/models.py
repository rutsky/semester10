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
    parent = models.ForeignKey('self', related_name='children')
    name = models.CharField(max_length=300)

class Episode(models.Model):
    category = models.ForeignKey(CategoryNode, related_name='episodes')
    
    name = models.CharField(max_length=300)
    description = models.TextField()
    
    subtitles = models.TextField()
    # TODO: Dictionaty etc.

# vim: ts=4 sw=4 et:
