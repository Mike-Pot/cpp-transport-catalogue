#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
using namespace std;

int main()
{
    catalogue::TransportCatalogue cat;
    JsonReader inp(cin);
    inp.LoadDB(cat);    
    renderer::MapRenderer map_rend;    
    inp.GetRenderSettings(map_rend);
    inp.PrintDB({ cat, map_rend }, cout);   
    return 0;
}