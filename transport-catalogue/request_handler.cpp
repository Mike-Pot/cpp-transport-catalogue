#include "request_handler.h"

std::optional<catalogue::Stats> RequestHandler::GetStat(const std::string& bus_name) const
{	
	using namespace catalogue;
	auto bus = db_.GetBusInfo(bus_name);

	if (bus)
	{			
		STOPS stops = MakeFullRoute(*bus);
		int all_stops = stops.size();

		std::unordered_set<const Stop_*> uniques;
		uniques.insert(stops.begin(), stops.end());
		int unq_stops = uniques.size();	

		double dist_fly = 0;
		double dist_ride = 0;
		for (int i = 0; i < stops.size() - 1; i++)
		{			
			dist_fly += geo::ComputeDistance(stops[i]->coord, stops[i + 1]->coord);			
			auto dist = db_.GetStopsDist({ stops[i],stops[i + 1] });
			if (dist)
			{
				dist_ride += *dist;
			}
			else
			{
				dist_ride += *db_.GetStopsDist({ stops[i + 1],stops[i] });
			}
		}
		return Stats{ all_stops,unq_stops,dist_ride,dist_ride / dist_fly };
	}
	else
	{
		return std::nullopt;
	}
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

svg::Document RequestHandler::RenderMap() const
{
	using namespace renderer;
	using namespace catalogue;
	STOPS all_stops = db_.GetAllStops(true);
	BUSES all_buses = db_.GetAllBuses(true);
	svg::Document map;

	std::vector<geo::Coordinates> geo_coords;
	for (auto stop : all_stops)
	{
		geo_coords.push_back(stop->coord);		
	}

	std::sort(all_stops.begin(), all_stops.end(), [](const Stop_* lhs, const Stop_* rhs)
		{ return lhs->name < rhs->name; });	
	std::sort(all_buses.begin(), all_buses.end(), [](const Bus_* lhs, const Bus_* rhs)
		{ return lhs->num < rhs->num; });

	SphereProjector proj{
		geo_coords.begin(), geo_coords.end(),
		map_sets.width, map_sets.height, map_sets.padding
	};
	std::unordered_map<const Stop_*, svg::Point> stop_projs;
	for (auto stop : all_stops)
	{
		stop_projs.insert({ stop,proj(stop->coord) });
	}

	size_t curr_col = 0;
	for (auto bus : all_buses)
	{
		std::vector<svg::Point> points;
		STOPS stops = MakeFullRoute(bus);
		for (auto stop : stops)
		{
			points.push_back(stop_projs.at(stop));
		}		
		renderer_.AddRouteToMap(map, points, curr_col);
		curr_col = (curr_col + 1) % map_sets.num_cols;		
	}

	curr_col = 0;
	for (auto bus : all_buses)
	{
		const STOPS& stops = bus->route;
		renderer_.AddRouteNameToMap(map, bus->num, curr_col,
			stop_projs.at(stops.front()));
		if (!bus->is_round && stops.front() != stops.back())
		{
			renderer_.AddRouteNameToMap(map, bus->num, curr_col,
				stop_projs.at(stops.back()));
		}		
		curr_col = (curr_col + 1) % map_sets.num_cols;		
	}

	for (auto stop : all_stops)
	{
		renderer_.AddStopToMap(map, stop_projs.at(stop));
	}

	for (auto stop : all_stops)
	{
		renderer_.AddStopNameToMap(map, stop->name, stop_projs.at(stop));
	}
	return map;
}

catalogue::STOPS RequestHandler::MakeFullRoute(const catalogue::Bus_* bus) const
{
	catalogue::STOPS res = bus->route;
	if (!bus->is_round)
	{
		for (auto it = bus->route.rbegin() + 1; it != bus->route.rend(); it++)
		{
			res.push_back(*it);
		}
	}
	return std::move(res);
}

void DrawMap(const RequestHandler& handler, std::ostream& out)
{
	handler.RenderMap().Render(out);
}