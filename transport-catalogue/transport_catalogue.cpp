#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string& stop_name, geo::Coordinates coor)
{
	stops_.push_back({ stop_name,coor });
	stop_name_key_.insert({ stops_.back().name, &stops_.back() });	
	stops_routes_.insert({ &stops_.back(),{} });
}

void TransportCatalogue::AddStopsDist(const std::string& stop_from, const std::string& stop_to, double dist)
{
	stop_dist_.insert({ {stop_name_key_.at(stop_from),stop_name_key_.at(stop_to)},dist });
}

void TransportCatalogue::AddRoute(const std::string& bus_num, const std::vector<std::string_view>& route)
{
	std::vector<Stop_*> stops;
	for (auto stop : route)
	{
		stops.push_back(stop_name_key_.at(stop));
	}
	buses_.push_back({ bus_num,move(stops) });
	bus_route_key_.insert({ buses_.back().num, &buses_.back() });
	for (auto stop : route)
	{		
		stops_routes_.at(stop_name_key_.at(stop)).insert(buses_.back().num);
	}
}

/*Bus route stops and length*/
std::optional<Stats> TransportCatalogue::GetStat(std::string_view bus) const
{
	std::optional<Stats> res;	
	auto it = bus_route_key_.find(bus);	
	if(it != bus_route_key_.end())
	{
		/*Bus route*/
		const std::vector<Stop_*>& stops = it->second->route; 

		/*Count all stops*/
		int all_stops = stops.size();

		/*Count unique stops*/
		std::unordered_set<Stop_*> uniques;
		uniques.insert(stops.begin(), stops.end());		
		int unq_stops = uniques.size();

		/*Count route length*/
		double dist_fly = 0;
		double dist_ride = 0;
		for (int i = 0; i < stops.size() - 1; i++)
		{
			dist_fly += geo::ComputeDistance((*stops[i]).coord, (*stops[i + 1]).coord);
			dist_ride += stop_dist_.count({ stops[i],stops[i + 1] }) ?
				stop_dist_.at({ stops[i],stops[i + 1] }) :
				stop_dist_.at({ stops[i + 1],stops[i] });
		}		
		res = { all_stops,unq_stops,dist_ride,dist_ride/dist_fly };
	}	
	return res;
}

const std::set<std::string_view>* TransportCatalogue::GetBusesWithStop(std::string_view stop) const
{	
	auto it = stop_name_key_.find(stop);	
	if(it == stop_name_key_.end())
	{		
		return nullptr;
	}
	return &stops_routes_.at(it->second);		
}
