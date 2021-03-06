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

from django.contrib import admin

from .models import CategoryNode, Dictionary, Word, Episode

admin.site.register(CategoryNode)
admin.site.register(Dictionary)
admin.site.register(Word)
admin.site.register(Episode)

# vim: ts=4 sw=4 et:
