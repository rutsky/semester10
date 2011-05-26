import graph3;
import grid3;
import palette;

size(400, 200, IgnoreAspect);

file in = input("first_load_der.csv").line().csv();

real[][] a = in.dimension(0, 0);
a = transpose(a);

real[] ti = a[0];
real[] yi = a[1];

real dt = 1;

triple f( pair t )
{
  real N = t.x;
  real lambda_c = t.y;
  
  real sum = 0;
  for (int i = 0; i < ti.length; ++i)
  {
    real v = yi[i] - 
        N * (exp(-lambda_c * (ti[i] + dt)) - exp(-lambda_c * ti[i]));
    sum += v * v;
  }

  return (N, lambda_c, sum);
}

bool cut( pair t )
{
  return f(t).z < 200;
}

currentprojection = orthographic(10, 10, 10);

size(400, 300, IgnoreAspect);

defaultrender.merge = true;

surface s = surface(f, (10, 0.01), (110, 0.2), 50, Spline, cond=cut);

draw(s, mean(palette(s.map(zpart), Rainbow())), black);

grid3(XYZgrid);

xaxis3(Label("$N$", 0.5), Bounds, InTicks);
yaxis3(Label("$\lambda_c$", 0.5), Bounds, InTicks);
zaxis3(Label("$S$", 0.5), Bounds, InTicks(beginlabel=false));

// vim: set ts=2 sw=2 et:
