#include "request_handler.h"

std::optional<catalogue::Stats> RequestHandler::GetStat(const std::string& bus_name) const
{
	return db_.GetStat(bus_name);	
}

std::optional<const catalogue::ROUTES*> RequestHandler::GetBusesWithStop(const std::string& stop_name) const
{	
	auto stop = db_.GetStopInfo(stop_name);

	if (stop)
	{
		return db_.GetStopRoutes(*stop);
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<catalogue::IDEAL_ROUTE> RequestHandler::GetIdealRoute(const std::string& stop_from, const std::string& stop_to) const
{
	auto from = db_.GetStopInfo(stop_from);
	auto to = db_.GetStopInfo(stop_to);
	if (from && to)
	{
		return transport_router_.GetRoute((*from)->id, (*to)->id);
	}
	else
	{
		return std::nullopt;
	}
}

svg::Document RequestHandler::RenderMap() const
{
	using namespace catalogue;
	STOPS all_stops = db_.GetAllStops(true);
	BUSES all_buses = db_.GetAllBuses(true);
	std::sort(all_stops.begin(), all_stops.end(), [](const Stop_* lhs, const Stop_* rhs)
		{ return lhs->name < rhs->name; });
	std::sort(all_buses.begin(), all_buses.end(), [](const Bus_* lhs, const Bus_* rhs)
		{ return lhs->num < rhs->num; });	
	return renderer_.RenderMap(all_buses, all_stops);
}
/*
void DrawMap(const RequestHandler& handler, std::ostream& out)
{
	handler.RenderMap().Render(out);
}
*/