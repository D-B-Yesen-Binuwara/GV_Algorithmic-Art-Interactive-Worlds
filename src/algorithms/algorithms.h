#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <utility>

// Bresenham: returns list of integer grid cells (x,y) for a line
std::vector<std::pair<int,int>> bresenhamLine(int x0, int y0, int x1, int y1);

// Midpoint circle: returns list of integer grid points approximating a circle outline
std::vector<std::pair<int,int>> midpointCircle(int cx, int cy, int r);

#endif // ALGORITHMS_H
