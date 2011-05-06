import graph;
import stats;

size(400, 200, IgnoreAspect);

file in = input("total_load_der.csv").line().csv();

real[][] a = in.dimension(0, 0);
a = transpose(a);

real[] xs = a[0];
real[] ys = a[1];

histogram(ys, min(ys), max(ys), bins(ys), 
  normalize=true, low=0, lightred, black, bars=false);

xaxis("Load", BottomTop, LeftTicks);
yaxis("Frequency", LeftRight, RightTicks(trailingzero));

// vim: set ts=2 sw=2 et:
