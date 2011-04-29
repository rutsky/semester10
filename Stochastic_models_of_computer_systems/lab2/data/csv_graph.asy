import graph;

real[][] draw_graph(string data_file_name, string ch_label)
{
  //size(200, 150, IgnoreAspect);

  file in = input(data_file_name).line().csv();

  real[][] a = in.dimension(0, 0);
  a = transpose(a);

  real[] time = a[0];
  real[] value = a[1];

  draw(graph(time, value), black, ch_label);

  yaxis(ch_label, RightTicks, EndArrow);
  xaxis("t", LeftTicks, EndArrow);

  return a;
}

// vim: set ts=2 sw=2 et:
