# Based on image source from Wikipedia 
# <http://en.wikipedia.org/wiki/File:Logarithmic_Scales.svg>
# by Autopilot <http://en.wikipedia.org/wiki/User:Autopilot>
# This file is licensed under the Creative Commons Attribution-Share Alike 3.0 
# Unported license.

set grid x
set grid y
set border 0
set xzeroaxis lt -1
set yzeroaxis lt -1
set size square

unset multiplot
set term svg size 800,800
set output "logarithmic_scales.svg"
set multiplot layout 2,2
set samples 3000

unset logscale
set xlabel "Linear"
set ylabel "Linear"
plot [-1:10] [-1:10] \
        (10**x) title "f(x) = 10^x" with lines, \
        (x) title "f(x) = x" with lines, \
        (log(x)) title "f(x) = log(x)" with lines, \
        (1/x) title "f(x) = 1/x" with lines

set logscale x
set xlabel "Log"
plot [0.001:1000] [-1:10] \
        (10**x) title "f(x) = 10^x" with lines, \
        (x) title "f(x) = x" with lines, \
        (log(x)) title "f(x) = log(x)" with lines, \
        (1/x) title "f(x) = 1/x" with lines

unset logscale
set logscale y
set xlabel "Linear"
set ylabel "Log"
plot [-1:10] [0.001:1000] \
        (10**x) title "f(x) = 10^x" with lines, \
        (x) title "f(x) = x" with lines, \
        (log(x)) title "f(x) = log(x)" with lines, \
        (1/x) title "f(x) = 1/x" with lines

unset logscale
set logscale y
set logscale x
set xlabel "Log"
set ylabel "Log"
plot [0.001:1000] [0.001:1000] \
        (10**x) title "f(x) = 10^x" with lines, \
        (x) title "f(x) = x" with lines, \
        (log(x)) title "f(x) = log(x)" with lines, \
        (1/x) title "f(x) = 1/x" with lines
