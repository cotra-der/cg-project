#pragma once
struct Point { int x; int y; };
struct PointF { float x; float y; };
inline bool operator==(Point a, Point b) { return a.x == b.x && a.y == b.y; }
