#  This file is part of risk calculation suite.
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

# Based on examples provided with cx_Freeze.

import sys

from cx_Freeze import setup, Executable

base = None
if sys.platform == "win32":
    base = "Win32GUI"

include_files = [
    ('forms/main_window.ui', 'forms/main_window.ui'), 
    ('i18n/ru_RU.qm', 'i18n/ru_RU.qm')]
includes = ['sip', 'encodings.ascii', 'encodings.utf_8', 'encodings.hex_codec']

setup(
    name="risk_calculation",
    version="0.1",
    description="Risk calculation suite",
    author="Vladimir Rutsky",
    author_email="altsysrq@gmail.com",
    options={
        'build_exe': {
            'include_files': include_files,
            'includes': includes,
            },
        },
    executables=[Executable("main.py", base=base)],
    )

# vim: set ts=4 sw=4 et:
