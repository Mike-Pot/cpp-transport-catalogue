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
#include "domain.h"

namespace catalogue
{
	using ROUTES = std::set<std::string_view>;
	using STOP_PAIR = std::pair<const Stop_*, const Stop_*>;

	class TransportCatalogue
	{
	public:
		TransportCatalogue() = default;
		void AddStop(const std::string& stop_name, geo::Coordinates coor);
		void AddRoute(const std::string& bus_num, const std::vector<std::string>& route, bool is_round);
		void AddStopsDist(const std::string& stop_from, const std::string& stop_to, double dist);
		std::optional<const Bus_*> GetBusInfo(std::string_view bus) const;
		std::optional<const Stop_*> GetStopInfo(std::string_view stop) const;
		const ROUTES* GetStopRoutes(const Stop_* stop) const;
		std::optional<double> GetStopsDist(STOP_PAIR stops) const;
		BUSES GetAllBuses(bool not_empty) const;
		STOPS GetAllStops(bool not_empty) const;

	private:
		std::deque<Bus_> buses_;
		std::deque<Stop_> stops_;
		std::unordered_map<std::string_view, const Bus_*> bus_route_key_;
		std::unordered_map<std::string_view, const Stop_*> stop_name_key_;
		std::unordered_map<const Stop_*, ROUTES> stops_routes_;
		struct hasher_stops_
		{
			size_t operator()(const STOP_PAIR& p) const
			{
				return std::hash<const void*>{}(p.first) ^ std::hash<const void*>{}(p.second);
			}
		};
		std::unordered_map <STOP_PAIR, double, hasher_stops_> stop_dist_;

		template<typename Key, typename Val, typename Hasher>
		std::optional<Val> GetInfo(const std::unordered_map<Key, Val, Hasher>& where, Key what) const
		{
			auto it = where.find(what);
			if (it != where.end())
			{
				return it->second;
			}
			else
			{
				return std::nullopt;
			}
		}
	};
}
