#include "cg/ScanlineFill.h"
#include <algorithm>
#include <cmath>
#include <map>
std::vector<Point> scanlineFill(const std::vector<Point>& p) {
    if(p.size()<3) return {};
    struct Edge { int end; double x,step; };
    std::map<int,std::vector<Edge>> table;
    int ymax=p.front().y;
    for(size_t i=0;i<p.size();++i) {
        auto a=p[i],b=p[(i+1)%p.size()];
        if(a.y==b.y) continue;
        if(a.y>b.y) std::swap(a,b);
        const double step=(double(b.x)-a.x)/(double(b.y)-a.y);
        // Sample at pixel centers; bottom endpoints are excluded to avoid
        // double-counting shared vertices and horizontal edges.
        table[a.y].push_back({b.y,a.x+0.5*step,step});
        ymax=std::max(ymax,b.y);
    }
    if(table.empty()) return {};
    std::vector<Edge> active;
    std::vector<Point> out;
    for(int y=table.begin()->first;y<ymax;++y) {
        active.erase(std::remove_if(active.begin(),active.end(),[y](const Edge& e){return y>=e.end;}),active.end());
        auto it=table.find(y);
        if(it!=table.end()) active.insert(active.end(),it->second.begin(),it->second.end());
        std::sort(active.begin(),active.end(),[](const Edge& a,const Edge& b){return a.x<b.x;});
        for(size_t i=0;i+1<active.size();i+=2)
            for(int x=int(std::ceil(active[i].x-0.5)); x<int(std::ceil(active[i+1].x-0.5)); ++x)
                out.push_back({x,y});
        for(auto& e:active) e.x+=e.step;
    }
    return out;
}
