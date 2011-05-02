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

import math

import qtall as qt4
qwt5 = qt4.Qwt5

# Using formulaes:
#  ...
#

class MainWindow(qt4.QMainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        # TODO: Use installation path.
        qt4.loadUi('forms/main_window.ui', self)

        self.reset_parameters()

        # Connect signals and slots.
        self.actionNew_Parameters.triggered.connect(self.on_new_parameters)

        # Initialize plot.
        self.init_plot()

    def closeEvent(self, event):
        super(MainWindow, self).closeEvent(event)

    @property
    def bottom_lo(self):
        return 10**(-self.bottom_lo_spin.value())

    @property
    def bottom_hi(self):
        return 10**(-self.bottom_hi_spin.value())

    @property
    def top_lo(self):
        return 10**(-self.top_lo_spin.value())

    @property
    def top_hi(self):
        return 10**(-self.top_hi_spin.value())

    @property
    def r_lo(self):
        return 10**(-self.r_lo_spin.value())

    @property
    def r_hi(self):
        return 10**(-self.r_hi_spin.value())

    def init_plot(self):
        self.bottom_curve = qwt5.QwtPlotCurve()
        self.bottom_curve.attach(self.qwtPlot)
        self.bottom_curve.setPen(qt4.QPen(qt4.Qt.black, 1))
        x = [self.bottom_lo, self.bottom_hi]
        y = [1, 1]
        self.bottom_curve.setData(x, y)

        self.left_curve = qwt5.QwtPlotCurve()
        self.left_curve.attach(self.qwtPlot)
        self.left_curve.setPen(qt4.QPen(qt4.Qt.black, 1))
        x = [self.bottom_lo, self.bottom_lo]
        y = [1, self.r_lo]
        self.left_curve.setData(x, y)

        self.right_curve = qwt5.QwtPlotCurve()
        self.right_curve.attach(self.qwtPlot)
        self.right_curve.setPen(qt4.QPen(qt4.Qt.black, 1))
        x = [self.bottom_hi, self.bottom_hi]
        y = [1, self.r_hi]
        self.right_curve.setData(x, y)

        self.top_curve = qwt5.QwtPlotCurve()
        self.top_curve.attach(self.qwtPlot)
        self.top_curve.setPen(qt4.QPen(qt4.Qt.black, 1))
        x = [self.bottom_lo, self.bottom_hi]
        y = [self.r_lo, self.r_hi]
        self.top_curve.setData(x, y)

        self.qwtPlot.setAxisScaleEngine(
            qwt5.QwtPlot.xBottom, qwt5.QwtLog10ScaleEngine())
        self.qwtPlot.setAxisScaleEngine(
            qwt5.QwtPlot.yLeft, qwt5.QwtLog10ScaleEngine())

        self.qwtPlot.replot()

    def update_plot(self):
        pass

    def reset_parameters(self):
        self.bottom_lo_spin.setValue(2)
        self.bottom_hi_spin.setValue(8)
        self.top_lo_spin.setValue(6.883)
        self.top_hi_spin.setValue(8.2)
        self.r_lo_spin.setValue(3.5)
        self.r_hi_spin.setValue(1)
        self.step_spin.setValue(1)
        self.delta_spin.setValue(22)
        self.h_spin.setValue(0.85)
        self.s_text.setText(u"qp * R")

    @qt4.Slot(bool)
    def on_new_parameters(self, checked):
        self.reset_parameters()

# vim: set ts=4 sw=4 et:
