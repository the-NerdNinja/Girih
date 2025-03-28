#include <bits/stdc++.h>

#include "./anim/animate.hpp"
#include "./object/obj.hpp"
#include "./param/parameter.hpp"

using namespace std;

int main() {
  srand(time(0));
  int n = 10;
  canvas cnv(1024 * 2, 1024 * 2);

  Node origin(-800, 0);
  VerticalLine yaxis(origin);
  HorizontalLine xaxis(origin);

  cnv.draw(&xaxis, Red, 1);
  cnv.draw(&yaxis, Red, 1);

  Node time(-400, 0);
  VerticalLine timeLine(time);

  vector<tuple<Point*, int>> pointList;
  vector<Keyframe> k;

  vector<unique_ptr<Rotor>> vr;
  vector<unique_ptr<Line>> ln;
  vector<unique_ptr<Point>> points;
  vector<RGBA> cl = {Rosewater, Red, Peach, Yellow, Green, Teal, Blue, Lavender, White, Sapphire};
  
  for (int i = 0; i < n; i++) {
    vr.push_back(make_unique<Rotor>(origin, 50 + rand() % 100));
    Keyframe kra(vr.back()->arg, float(0), (2.0f + (rand() % 100) / 10.0f) * pi);
    k.push_back(kra);
    ln.push_back(make_unique<HorizontalLine>(*vr.back()));
    points.push_back(make_unique<IntersectionPoint>(timeLine, *ln.back(), cl[i]));
    pointList.push_back({points.back().get(), 1});
  }
  
  RGBA NeonOrange (255, 85, 0, 255);
  for (int i = 0, j = n - 1; i < n - 1; i++, j++) {
    points.push_back(make_unique<PointSum>(*points[i], *points[j], [](const Point& a, const Point& b) -> Coord{ 
      auto [x1, y1] = a.loc();
      auto [x2, y2] = b.loc();
      return {x1, y1 + y2};
    }, NeonOrange));
  }
  pointList.push_back({points.back().get(), 1});

  Keyframe ktime(time.x, float(-800), float(1000));
  vector<Keyframe *> keys = {&ktime};
  for (auto &u : k) 
    keys.push_back(&u);

  vector<tuple<Drawable *, int>> objList = {};
  vector<tuple<Text *, int>> textList = {};

  animate(pointList, objList, textList, cnv, keys, 0);
}
