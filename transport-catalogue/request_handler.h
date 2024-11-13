#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

class RequestHandler {
public:    
    RequestHandler(const catalogue::TransportCatalogue& db, const renderer::MapRenderer& render)
        : db_(db), renderer_(render)
    {}    
    std::optional<catalogue::Stats> GetStat(const std::string& bus_name) const;
    std::optional<const catalogue::ROUTES*> GetBusesWithStop(const std::string& stop_name) const;
    svg::Document RenderMap() const;

private:    
    catalogue::STOPS MakeFullRoute(const catalogue::Bus_* bus) const;
    const catalogue::TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
//void DrawMap(const RequestHandler& handler, std::ostream& out);