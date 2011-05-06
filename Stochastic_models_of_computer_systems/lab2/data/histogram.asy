from "csv_graph.asy" access draw_graph;

size(400, 200, IgnoreAspect);
draw_graph("histogram.csv", "Load", "Count");

// vim: set ts=2 sw=2 et:
