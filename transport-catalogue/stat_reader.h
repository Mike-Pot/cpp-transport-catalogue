#pragma once
#include <iosfwd>
#include <string_view>
#include <iomanip>
#include "transport_catalogue.h"

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
    std::ostream& output);
namespace reader
{
    void PrintBuses(const TransportCatalogue& tansport_catalogue, std::string_view request,
        std::ostream& output);
    void PrintStops(const TransportCatalogue& tansport_catalogue, std::string_view request,
        std::ostream& output);
}
