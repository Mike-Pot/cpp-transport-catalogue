#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_router.h"
using namespace std;

int main()
{    
    catalogue::TransportCatalogue cat;
    JsonReader inp(cin);
    inp.LoadDB(cat);    
    renderer::MapRenderer map_rend;    
    inp.GetRenderSettings(map_rend);
    rout::TransportRouter router = inp.GetRoutSettings(cat);
    inp.PrintDB({ cat, map_rend, router }, cout);   
    return 0;    
}
