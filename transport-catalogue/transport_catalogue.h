#pragma once
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <set>
#include "geo.h"

class TransportCatalogue
{	
public:	
	struct Stats
	{
		int stops;
		int unique_stops;
		double dist;
	};
	TransportCatalogue() = default;
	void AddStop(std::string&, geo::Coordinates);	
	void AddRoute(std::string&, std::vector<std::string_view>);
	bool ShowStat(std::string_view, Stats*) const;
	bool ShowStop(std::string_view, const std::set<std::string_view>**) const;
	
private:
	struct Stop_
	{
		std::string name;
		geo::Coordinates coord;
	};
	struct Bus_
	{
		std::string num;
		std::vector<Stop_*> route;		
	};
	std::deque<Bus_> buses_;
	std::deque<Stop_> stops_;	
	std::unordered_map<std::string_view, Bus_*> bus_route_key_;
	std::unordered_map<std::string_view, Stop_*> stop_name_key_;
	std::unordered_map<Stop_*, std::set<std::string_view>> stops_routes_;	
};
