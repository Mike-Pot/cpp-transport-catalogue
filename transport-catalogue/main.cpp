#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_router.h"
using namespace std;

int main()
{
    /*
    graph::DirectedWeightedGraph<double> g {3};
    g.AddEdge({ 0, 1, 10 });
    graph::Router r{g};
    r.BuildRoute(1, 0);
    */
    
    catalogue::TransportCatalogue cat;
    JsonReader inp(cin);
    inp.LoadDB(cat);    
    renderer::MapRenderer map_rend;    
    inp.GetRenderSettings(map_rend);
    rout::TransportRouter router = inp.GetRoutSettings(cat);
 //   cout << router.kkk();
  //  auto a = router.GetRoute(2, 6);
    inp.PrintDB({ cat, map_rend, router }, cout);   
    return 0;
    
}