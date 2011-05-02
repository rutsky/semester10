#  This file is part of risc calculation suite.
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

__author__  = "Vladimir Rutsky <altsysrq@gmail.com>"
__license__ = "GPL"

__all__ = ["MainWindow"]

import config

import qtall as qt4

# Using formulaes:
#  ...
#

class MainWindow(qt4.QMainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        # TODO: Use installation path.
        qt4.loadUi('forms/main_window.ui', self)

        # Connect signals and slots.
        self.actionNew_Parameters.triggered.connect(self.on_new_parameters)

    def closeEvent(self, event):
        super(MainWindow, self).closeEvent(event)

    @qt4.Slot(bool)
    def on_new_parameters(self, checked):
        self.bottom_lo_spin.setValue(6)
        self.bottom_hi_spin.setValue(8)
        self.top_lo_spin.setValue(6.883)
        self.top_hi_spin.setValue(8.2)
        self.r_lo_spin.setValue(3.5)
        self.r_hi_spin.setValue(1)
        self.step_spin.setValue(1)
        self.delta_spin.setValue(22)
        self.h_spin.setValue(0.85)
        self.s_text.setText(u"qp * R")

# vim: set ts=4 sw=4 et:
