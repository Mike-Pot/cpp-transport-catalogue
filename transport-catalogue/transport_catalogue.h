#pragma once
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <set>
#include <optional>
#include "geo.h"

namespace catalogue
{
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

	struct Stats
	{
		int stops;
		int unique_stops;
		double dist;
	};
}

using namespace catalogue;

class TransportCatalogue
{	
public:		
	TransportCatalogue() = default;
	void AddStop(const std::string& stop_name, geo::Coordinates coor);	
	void AddRoute(const std::string& bus_num, const std::vector<std::string_view>& route);
	std::optional<Stats> GetStat(std::string_view bus) const;
	const std::set<std::string_view>* GetBusesWithStop(std::string_view stop) const;
	
private:	
	std::deque<Bus_> buses_;
	std::deque<Stop_> stops_;	
	std::unordered_map<std::string_view, Bus_*> bus_route_key_;
	std::unordered_map<std::string_view, Stop_*> stop_name_key_;
	std::unordered_map<Stop_*, std::set<std::string_view>> stops_routes_;	
};
