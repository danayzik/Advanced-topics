#include <memory>
#include "ComparativeSimulator.h"
#include "CompetitionSimulator.h"
#include "CommandLineParser.h"

int main(int argc, char* argv[]){

    std::unique_ptr<Simulator> simulator = nullptr;
    CommandLineParser& parser = CommandLineParser::getParser();
    parser.parse(argc, argv);
    const ParsedArguments& arguments = parser.getParsedArguments();
    if (arguments.mode == RunMode::Comparative){
        simulator = std::make_unique<ComparativeSimulator>();
    }
    else{
        simulator = std::make_unique<CompetitionSimulator>();
    }


}