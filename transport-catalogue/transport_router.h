#pragma once
#include "router.h"
#include "transport_catalogue.h"

namespace rout
{
	/*
	Key = <edge id>
	Value = <route span, bus or stop name>
	*/
	using EDGE_INF = std::unordered_map<size_t, std::pair<size_t, std::string>>;
	
	struct Data
	{		
		graph::DirectedWeightedGraph<double> route_graph_;
		EDGE_INF edge_inf_;
	};

	struct Params
	{
		const catalogue::TransportCatalogue& db_;
		const catalogue::Bus_* bus;
		size_t stop_from;
	};

	class Builder
	{
	public:
		Builder(size_t stop_num) :	route_graph( stop_num )	{}
		Builder& SetGraph(const catalogue::TransportCatalogue& db_, int wait_time, double velocity);
		Data GetGraph();
	private:	
		void AddRoute(Params params, double velocity, bool forth);
		double ComputeDist(Params params, size_t stop_to) const;
		graph::DirectedWeightedGraph<double> route_graph;
		EDGE_INF edge_inf;
	};

	class TransportRouter
	{
	public:		
		TransportRouter(const catalogue::TransportCatalogue& db_, int wait_time, double velocity)		
			:  data_(
				std::move(Builder( db_.GetAllStops().size()*2 )
				.SetGraph(db_,wait_time,velocity)
				.GetGraph())), router_(data_.route_graph_) {}
		std::optional<catalogue::IDEAL_ROUTE> GetRoute(size_t stop_from, size_t stop_to) const;		
	private:		
		Data data_;	
		graph::Router<double> router_;
	};
}
