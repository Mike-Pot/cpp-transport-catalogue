#include "stat_reader.h"

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
    std::ostream& output)
{    
    std::string_view com = request.substr(0, request.find_first_of(' '));
    std::string_view req = request.substr(request.find_first_of(' ') + 1);
    if (com == "Bus")
    {
        reader::PrintBuses(tansport_catalogue, req, output);
    }
    if (com == "Stop")
    {
        reader::PrintStops(tansport_catalogue, req, output);
    }
} 

namespace reader
{
    void PrintBuses(const TransportCatalogue& tansport_catalogue, std::string_view request,
        std::ostream& output)
    {
        std::optional<Stats> stat = tansport_catalogue.GetStat(request);
        if (!stat.has_value())
        {
            output << "Bus " << request << ": not found" << std::endl;
        }
        else
        {
            output << "Bus " << request << ": " << stat.value().stops << " stops on route, " 
                << stat.value().unique_stops << " unique stops, "
                << std::setprecision(6) << stat.value().dist << " route length, "
                << stat.value().curv << " curvature" << std::endl;
        }
    }

    void PrintStops(const TransportCatalogue& tansport_catalogue, std::string_view request,
        std::ostream& output)
    {
        const std::set<std::string_view>* buses = tansport_catalogue.GetBusesWithStop(request);
        if (buses == nullptr)
        {
            output << "Stop " << request << ": not found" << std::endl;
        }
        else
        {
            if (buses->empty())
            {
                output << "Stop " << request << ": no buses" << std::endl;
            }
            else
            {
                output << "Stop " << request << ": buses";
                for (auto bus : *buses)
                {
                    output << " " << bus;
                }
                output << std::endl;
            }
        }
    }
}
