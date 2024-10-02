#include "transport_catalogue.h"

void TransportCatalogue::AddStop(std::string& stop_name, geo::Coordinates coor)
{
	stops_.push_back({ move(stop_name),coor });
	stop_name_key_.insert({ stops_.back().name, &stops_.back() });	
	stops_routes_.insert({ &stops_.back(),{} });
}

void TransportCatalogue::AddRoute(std::string& bus_num, std::vector<std::string_view> route)
{
	std::vector<Stop_*> stops;
	for (auto stop : route)
	{
		stops.push_back(stop_name_key_.at(stop));
	}
	buses_.push_back({ move(bus_num),move(stops) });
	bus_route_key_.insert({ buses_.back().num, &buses_.back() });
	for (auto stop : route)
	{		
		stops_routes_.at(stop_name_key_.at(stop)).insert(buses_.back().num);
	}
}

bool TransportCatalogue::ShowStat(std::string_view bus, Stats* res) const
{
	if (!bus_route_key_.count(bus))
	{
		return false;
	}	
	std::vector<Stop_*> stops = bus_route_key_.at(bus)->route;
	res->stops = stops.size();
	std::unordered_set<Stop_*> uniques;
	uniques.insert(stops.begin(), stops.end());
	res->unique_stops = uniques.size();	
	double dist = 0;
	for(int i =0; i < stops.size()-1; i++)
	{
		dist += geo::ComputeDistance((*stops[i]).coord, (*stops[i + 1]).coord);
	}
	res->dist = dist;
	return true;
}

bool TransportCatalogue::ShowStop(std::string_view stop, const std::set<std::string_view>** buses) const
{	
	if(!stop_name_key_.count(stop))
	{		
		return false;
	}	
	*buses = &stops_routes_.at(stop_name_key_.at(stop));	
	return true;
}
