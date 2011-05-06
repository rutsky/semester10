import graph;

real[][] draw_graph(string data_file_name, string x_label, string y_label)
{
  //size(200, 150, IgnoreAspect);

  file in = input(data_file_name).line().csv();

  real[][] a = in.dimension(0, 0);
  a = transpose(a);

  real[] time = a[0];
  real[] value = a[1];

  draw(graph(time, value), black);

  yaxis(y_label, RightTicks, EndArrow);
  xaxis(x_label, LeftTicks, EndArrow);

  return a;
}

// vim: set ts=2 sw=2 et:
