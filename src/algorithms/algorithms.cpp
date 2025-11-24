#include "algorithms.h"
#include <cmath>

std::vector<std::pair<int,int>> bresenhamLine(int x0, int y0, int x1, int y1) {
    std::vector<std::pair<int,int>> pts;
    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int x = x0, y = y0;
    while (true) {
        pts.emplace_back(x,y);
        if (x==x1 && y==y1) break;
        int e2 = 2*err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
    }
    return pts;
}

std::vector<std::pair<int,int>> midpointCircle(int cx, int cy, int r) {
    std::vector<std::pair<int,int>> pts;
    int x = r;
    int y = 0;
    int dx = 1 - (r << 1);
    int dy = 0;
    int err = 0;
    while (x >= y) {
        pts.emplace_back(cx + x, cy + y);
        pts.emplace_back(cx + y, cy + x);
        pts.emplace_back(cx - y, cy + x);
        pts.emplace_back(cx - x, cy + y);
        pts.emplace_back(cx - x, cy - y);
        pts.emplace_back(cx - y, cy - x);
        pts.emplace_back(cx + y, cy - x);
        pts.emplace_back(cx + x, cy - y);

        y++;
        err += dy;
        dy += 2;
        if ((err << 1) + dx > 0) {
            x--;
            err += dx;
            dx += 2;
        }
    }
    return pts;
}
