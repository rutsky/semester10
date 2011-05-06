import graph;

from "csv_graph.asy" access draw_graph;

real dt = 1;

draw_graph("requests.csv", "t", "Requests");
real[][] requests = draw_graph("total_load.csv", "t", "Requests");

file in = input("detected_requests.txt").line().csv();

real[][] a = in.dimension(0, 0);
a = transpose(a);

real[] indices = a[0];

for (real idx : indices)
{
  dot((requests[0][(int)idx], requests[1][(int)idx]), red);
}

// vim: set ts=2 sw=2 et:
