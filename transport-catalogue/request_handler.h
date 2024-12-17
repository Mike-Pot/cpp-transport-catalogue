#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

class RequestHandler {
public:    
    RequestHandler(const catalogue::TransportCatalogue& db, const renderer::MapRenderer& render
    , const rout::TransportRouter& transp_router)
        : db_(db), renderer_(render), transport_router_(transp_router)
    {}    
    std::optional<catalogue::Stats> GetStat(const std::string& bus_name) const;
    std::optional<const catalogue::ROUTES*> GetBusesWithStop(const std::string& stop_name) const;   
    std::optional<catalogue::IDEAL_ROUTE> GetIdealRoute(const std::string& stop_from, const std::string& stop_to) const;
    svg::Document RenderMap() const;

private:    
    const catalogue::TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
    const rout::TransportRouter& transport_router_;
};
//void DrawMap(const RequestHandler& handler, std::ostream& out);