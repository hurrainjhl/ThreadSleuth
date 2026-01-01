#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "PartitionParser.hpp"
#include "ResultCollector.hpp"

class ReportGenerator {
public:
    static void generate_html(const std::string& filename, 
                              const std::vector<Partition>& partitions, 
                              const std::vector<AnalysisResult>& artifacts,
                              double duration);
};
