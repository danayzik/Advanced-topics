#include <memory>
#include "ComparativeSimulator.h"
#include "CompetitionSimulator.h"
#include "CommandLineParser.h"
#include <iostream>
int main(int argc, char* argv[]){

    std::unique_ptr<Simulator> simulator;
    CommandLineParser& parser = CommandLineParser::getParser();
    try{
        parser.parse(argc, argv);
    }
    catch (const std::exception& e){
        std::cerr << "Critical error encountered! can't continue.\n";
        std::cerr << e.what() << std::endl;
        return 1;
    }

    const ParsedArguments& arguments = parser.getParsedArguments();
    if (arguments.mode == RunMode::Comparative){
        simulator = std::make_unique<ComparativeSimulator>();
    }
    else{
        simulator = std::make_unique<CompetitionSimulator>();
    }
    try{
        simulator->loadArguments(arguments);
        simulator->run();
    }
    catch (const std::exception& e){
        std::cerr << "Critical error encountered! can't continue.\n";
        std::cerr << e.what() << std::endl;
    }



}