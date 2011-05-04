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
import itertools

import qtall as qt4
qwt = qt4.Qwt5

# Using formulaes:
#  ...
#

class Constants(object):
    min_hi_lo_dist = 1.0

def lerp(x0, x1, y0, y1):
    assert x0 != x1
    k = (y1 - y0) / (x1 - x0)
    def lerp_impl(x):
        return y0 + k * (x - x0)
    return lerp_impl

def xfrange(start, stop, step):
    for i in itertools.count():
        value = start + step * i
        if step >= 0 and value < stop:
            yield value
        elif stop < value:
            yield value
        else:
            break
lg = lambda x: math.log(x, 10)

class MainWindow(qt4.QMainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        # TODO: Use installation path.
        qt4.loadUi('forms/main_window.ui', self)

        # Initialize parameters in GUI.
        self.reset_parameters()
        self.update_ranges()

        # Initialize plot.
        self.init_plot()
        self.update_plot()

        # Connect signals and slots.
        self.actionNew_Parameters.triggered.connect(self.on_new_parameters)
        self.bottom_lo_spin.valueChanged[float].connect(
            self.on_bound_spin_changed)
        self.bottom_hi_spin.valueChanged[float].connect(
            self.on_bound_spin_changed)
        self.top_lo_spin.valueChanged[float].connect(
            self.on_bound_spin_changed)
        self.top_hi_spin.valueChanged[float].connect(
            self.on_bound_spin_changed)
        self.r_lo_spin.valueChanged[float].connect(
            self.on_bound_spin_changed)
        self.r_hi_spin.valueChanged[float].connect(
            self.on_bound_spin_changed)
        self.step_spin.valueChanged[float].connect(
            self.on_bound_spin_changed)
        self.c_spin.valueChanged[float].connect(
            self.on_bound_spin_changed)

    def closeEvent(self, event):
        super(MainWindow, self).closeEvent(event)

    @property
    def bottom_lo_log(self):
        return -self.bottom_lo_spin.value()

    @property
    def bottom_hi_log(self):
        return -self.bottom_hi_spin.value()

    @property
    def top_lo_log(self):
        return -self.top_lo_spin.value()

    @property
    def top_hi_log(self):
        return -self.top_hi_spin.value()

    @property
    def r_lo_log(self):
        return -self.r_lo_spin.value()

    @property
    def r_hi_log(self):
        return -self.r_hi_spin.value()

    @property
    def top_step_log(self):
        return -self.step_spin.value()

    @property
    def bottom_lo(self):
        return 10**self.bottom_lo_log

    @property
    def bottom_hi(self):
        return 10**self.bottom_hi_log

    @property
    def top_lo(self):
        return 10**self.top_lo_log

    @property
    def top_hi(self):
        return 10**self.top_hi_log

    @property
    def r_lo(self):
        return 10**self.r_lo_log

    @property
    def r_hi(self):
        return 10**self.r_hi_log

    @property
    def top_step(self):
        return 10**self.top_step_log

    @property
    def c_coef(self):
        return self.c_spin.value()

    def init_plot(self):
        self.bottom_curve = qwt.QwtPlotCurve()
        self.bottom_curve.attach(self.qwtPlot)
        self.bottom_curve.setPen(qt4.QPen(qt4.Qt.black, 1))

        self.left_curve = qwt.QwtPlotCurve()
        self.left_curve.attach(self.qwtPlot)
        self.left_curve.setPen(qt4.QPen(qt4.Qt.black, 1))

        self.right_curve = qwt.QwtPlotCurve()
        self.right_curve.attach(self.qwtPlot)
        self.right_curve.setPen(qt4.QPen(qt4.Qt.black, 1))

        self.top_curve = qwt.QwtPlotCurve()
        self.top_curve.attach(self.qwtPlot)
        self.top_curve.setPen(qt4.QPen(qt4.Qt.black, 1))

        # Use logarithmic axes.
        self.qwtPlot.setAxisScaleEngine(
            qwt.QwtPlot.xBottom, qwt.QwtLog10ScaleEngine())
        self.qwtPlot.setAxisScaleEngine(
            qwt.QwtPlot.yLeft, qwt.QwtLog10ScaleEngine())

        # Set axes titles.
        self.qwtPlot.setAxisTitle(qwt.QwtPlot.xBottom, 
            qwt.QwtText(self.tr("<big>lg <i>q<sub>project</sub></i></big>")))
        self.qwtPlot.setAxisTitle(qwt.QwtPlot.yLeft, 
            qwt.QwtText(self.tr("<big>lg |R|</big>")))
        # No top axis currently.
        #self.qwtPlot.setAxisTitle(qwt.QwtPlot.xTop, 
        #    qwt.QwtText(self.tr("<big>lg <i>q<sub>real</sub></i></big>")))

        # Invert axes.
        self.qwtPlot.axisScaleEngine(qwt.QwtPlot.xBottom).setAttribute(
            qwt.QwtScaleEngine.Inverted, True)
        self.qwtPlot.axisScaleEngine(qwt.QwtPlot.yLeft).setAttribute(
            qwt.QwtScaleEngine.Inverted, True)

        # Upper line markers.
        self.top_markers = []
        self.bottom_markers = []
        self.markers_qreal = []
        self.markers_qproject = []
        self.markers_lines = []
        self.parabolas_lines = []
        self.update_markers()

        self.top_label = None
        self.update_top_label()
    
        self.qwtPlot.setMargin(0)

        self.qwtPlot.replot()

    def update_plot(self):
        x = [self.bottom_lo, self.bottom_hi]
        y = [1, 1]
        self.bottom_curve.setData(x, y)

        x = [self.bottom_lo, self.bottom_lo]
        y = [1, self.r_lo]
        self.left_curve.setData(x, y)

        x = [self.bottom_hi, self.bottom_hi]
        y = [1, self.r_hi]
        self.right_curve.setData(x, y)

        x = [self.bottom_lo, self.bottom_hi]
        y = [self.r_lo, self.r_hi]
        self.top_curve.setData(x, y)

    def reset_parameters(self):
        self.bottom_lo_spin.setRange(1, 12)
        self.bottom_lo_spin.setValue(2)

        self.bottom_hi_spin.setRange(1, 12)
        self.bottom_hi_spin.setValue(8)

        self.top_lo_spin.setRange(1, 12)
        self.top_lo_spin.setValue(2.3)

        self.top_hi_spin.setRange(1, 12)
        self.top_hi_spin.setValue(8.4)

        self.r_lo_spin.setRange(1, 10)
        self.r_lo_spin.setValue(3.5)

        self.r_hi_spin.setRange(1, 10)
        self.r_hi_spin.setValue(1)

        self.step_spin.setValue(1)
        self.step_spin.setRange(0.5, 5)

        self.delta_spin.setValue(22)
        self.h_spin.setValue(0.85)
        self.s_text.setText(u"qp * R")

        self.c_spin.setValue(7.0)
        self.c_spin.setRange(0.001, 100)

    def update_ranges(self):
        assert self.bottom_lo_spin.value() + Constants.min_hi_lo_dist <= \
            self.bottom_hi_spin.value()
        assert self.top_lo_spin.value() + Constants.min_hi_lo_dist <= \
            self.top_hi_spin.value()
        assert self.r_hi_spin.value() + Constants.min_hi_lo_dist <= \
            self.r_lo_spin.value()

        self.bottom_lo_spin.setMaximum(
            self.bottom_hi_spin.value() - Constants.min_hi_lo_dist)
        self.bottom_hi_spin.setMinimum(
            self.bottom_lo_spin.value() + Constants.min_hi_lo_dist)
        self.top_lo_spin.setMaximum(
            self.top_hi_spin.value() - Constants.min_hi_lo_dist)
        self.top_hi_spin.setMinimum(
            self.top_lo_spin.value() + Constants.min_hi_lo_dist)
        self.r_hi_spin.setMaximum(
            self.r_lo_spin.value() - Constants.min_hi_lo_dist)
        self.r_lo_spin.setMinimum(
            self.r_hi_spin.value() + Constants.min_hi_lo_dist)

    def update_top_label(self):
        if self.top_label is None:
            self.top_label = qwt.QwtPlotMarker()
            self.top_label.attach(self.qwtPlot)

        self.top_label.setValue(
            10**((self.bottom_hi_log + self.bottom_lo_log) / 2.0), 
            min(self.r_lo, self.r_hi))
        self.top_label.setLabel(
            qwt.QwtText(self.tr("<big>lg <i>q<sub>real</sub></i></big>")))
        self.top_label.setLabelAlignment(qt4.Qt.AlignTop)

    def update_markers(self):
        for m in self.top_markers:
            m.attach(None)
        for m in self.bottom_markers:
            m.attach(None)

        self.top_markers = []
        self.bottom_markers = []
        self.markers_qproject = []
        self.markers_qreal = []

        qreal = 10**math.ceil(self.top_lo_log)
        while qreal > 10**math.floor(self.top_hi_log):
            qproject = self.top_real_to_project(qreal)

            if qreal > self.bottom_lo or qreal < self.bottom_hi or \
                    qproject > self.bottom_lo or qproject < self.bottom_hi:
                qreal *= self.top_step
                continue

            self.markers_qreal.append(qreal)
            self.markers_qproject.append(qproject)

            # Top marker.
            m = qwt.QwtPlotMarker()
            m.setValue(qproject, self.project_to_top_r(qproject))
            m.setLabel(qwt.QwtText("{0:.1e}".format(qreal)))
            m.setLabelAlignment(qt4.Qt.AlignTop | qt4.Qt.AlignRight)
            m.setSymbol(qwt.QwtSymbol(
                qwt.QwtSymbol.Ellipse, qt4.QBrush(qt4.QColor(0, 0, 0)), 
                qt4.QPen(qt4.QColor(0, 0, 0)), qt4.QSize(4, 4)))
            m.attach(self.qwtPlot)
            
            self.top_markers.append(m)

            # Bottom marker.
            m = qwt.QwtPlotMarker()
            m.setValue(qreal, 1)
            m.setSymbol(qwt.QwtSymbol(
                qwt.QwtSymbol.Ellipse, qt4.QBrush(qt4.QColor(0, 0, 0)), 
                qt4.QPen(qt4.QColor(0, 0, 0)), qt4.QSize(4, 4)))
            m.attach(self.qwtPlot)

            self.bottom_markers.append(m)

            qreal *= self.top_step

        # Update lines connecting markers.
        for curve in self.markers_lines:
            curve.attach(None)

        self.markers_lines = []

        for qreal, qproject in zip(self.markers_qreal, self.markers_qproject):
            curve = qwt.QwtPlotCurve()
            curve.setPen(qt4.QPen(qt4.Qt.DashLine))
            curve.setData(
                [qproject, qreal],
                [self.project_to_top_r(qproject), 1])
            curve.attach(self.qwtPlot)

            self.markers_lines.append(curve)

        # Draw parabolas.
        for curve in self.parabolas_lines:
            curve.attach(None)

        self.parabolas_lines = []

        for qreal, qproject in zip(self.markers_qreal, self.markers_qproject):
            curve = qwt.QwtPlotCurve()
            curve.setPen(qt4.QPen(qt4.Qt.DashDotLine))
            #curve.setData(
            #    [qproject, qreal],
            #    [self.project_to_top_r(qproject), 1])

            angle = math.atan2(
                -lg(self.project_to_top_r(qproject)), 
                lg(qreal) - lg(qproject)) - math.pi / 2.0
            #print angle # DEBUG
            #angle = 0 # DEBUG

            xs = []
            ys = []

            for x_log in xfrange(
                    self.bottom_lo_log, 
                    self.bottom_hi_log, 
                    -0.01):
                #print "x_log:", x_log # DEBUG
                #print "qreal:", qreal, lg(qreal) # DEBUG
                y_log = -self.c_coef * (lg(qreal) - x_log)**2
                #print "y_log:", y_log # DEBUG

                x_log_rot = \
                    math.cos(angle) * (x_log - lg(qreal)) - \
                    math.sin(angle) * (y_log) + \
                    lg(qreal)
                y_log_rot = \
                    math.sin(angle) * (x_log - lg(qreal)) + \
                    math.cos(angle) * (y_log)

                if (x_log_rot < self.bottom_lo_log and
                    x_log_rot > self.bottom_hi_log and
                    y_log_rot < 1 and
                    y_log_rot > lg(self.project_to_top_r(10**x_log_rot))):
                    xs.append(10**x_log_rot)
                    ys.append(10**y_log_rot)

            curve.setData(xs, ys)

            curve.attach(self.qwtPlot)

            self.parabolas_lines.append(curve)

    def project_to_top_r(self, qproject):
        return 10**(lerp(self.bottom_lo_log, self.bottom_hi_log, 
            self.r_lo_log, self.r_hi_log)(lg(qproject)))

    def project_to_top_real(self, qproject):
        return 10**(lerp(self.bottom_lo_log, self.bottom_hi_log, 
            self.top_lo_log, self.top_hi_log)(lg(qproject)))

    def top_real_to_project(self, qreal):
        return 10**lerp(self.top_lo_log, self.top_hi_log,
            self.bottom_lo_log, self.bottom_hi_log)(lg(qreal))

    @qt4.Slot(bool)
    def on_new_parameters(self, checked):
        self.reset_parameters()

    @qt4.Slot(float)
    def on_bound_spin_changed(self, val):
        self.update_ranges()
        self.update_plot()
        self.update_markers()
        self.update_top_label()
        self.qwtPlot.replot()

# vim: set ts=4 sw=4 et:
