import graph;

real dt = 1;

string data_file_name = "stat.csv";
string x_label = "$\frac{\sigma}{m_c}$";
string y_label = "$\frac{|K - \widehat{K}|}{K}$";

size(10cm, 10cm, IgnoreAspect);

file in = input(data_file_name).line().csv();

real[][] a = in.dimension(0, 0);
a = transpose(a);

real[] xIn = a[0], xVec;
real[] y1In = a[1], y1Vec;
real[] y2In = a[2], y2Vec;

for (real x : xIn)
  xVec.push(x / 10.0);

for (real y : y1In)
  y1Vec.push(abs(202 - y) / 202.0);
for (real y : y2In)
  y2Vec.push(abs(202 - y) / 202.0);

draw("iterative", graph(xVec, y1Vec));
draw("EM-algorithm", graph(xVec, y2Vec));

xlimits(0.130, 0.145);

xaxis(x_label, BottomTop, LeftTicks(N=5), EndArrow);
yaxis(y_label, LeftRight, RightTicks(N=7), EndArrow);

//attach(legend(), point(NW), 40S+30E, UnFill);
