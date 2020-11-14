#pragma once

#include "Avoid/Benchmarking/BenchmarkingInterface.h"
#include <algorithm>
#include <string>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <cmath>
#include <ctime>
#include <fstream>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

namespace avoid
{
    namespace benchmarking
    {


        //don't want to make nlohmann a public dependency. So these functions will not be class members.
        nlohmann::json SerializeBenchmarkingDescription(const BenchmarkingDescription& description);

        double CalculateMean(const std::vector<double>& items);
        double CalculateSTD(const std::vector<double>& items);

        BenchmarkingInterface::BenchmarkingInterface(const std::string &ip, uint16_t port)
        {
            client = std::make_unique<Client>(ip, port);
        }

        void BenchmarkingInterface::Run(std::string outputFolder)
        {
            VerifyBenchmarkDescriptions();

            auto folderPath = fs::path(outputFolder);

            if(!fs::exists(folderPath))
            {
                fs::create_directory(outputFolder);
            }



            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            std::ostringstream oss;
            oss << std::put_time(&tm, "AvoidBench_%d-%m-%Y_%H-%M-%S.json");
            std::string fileName( oss.str() );

            auto filePath = folderPath.append(fileName);
            
    
            std::ofstream outputFile(filePath.string());

            //TODO:: Logging class for proper logging

            std::cout << "starting benchmark" << std::endl;

            nlohmann::json benchmarkResults;
            benchmarkResults["Benchmarks"] = nlohmann::json::array();

            uint32_t missionNumber = 0;
            for (auto &benchmarkingDescription : missionDescriptionList)
            {
                auto benchmarkingDescriptionJson = SerializeBenchmarkingDescription(benchmarkingDescription);

                std::cout << "## Running " << benchmarkingDescription.trials << "trial(s) for " << benchmarkingDescription.worldName << std::endl;
                std::cout << "## Starting mission '" <<  benchmarkingDescription.missionName << "'" << std::endl;
               
                for (int i = 0; i < benchmarkingDescription.trials; i++)
                {
                    nlohmann::json trialResultsJson;

                    World world = client->LoadWorld(benchmarkingDescription.worldName);
                    for (auto &proceduralDescription : benchmarkingDescription.proceduralDecriptions)
                    {
                        std::cout << proceduralDescription.proceduralLocation << " " << proceduralDescription.radius << std::endl;
                        world.SetProceduralLocation(proceduralDescription.proceduralLocation, proceduralDescription.radius);
                    }

                    Mission mission = world.LoadMission(benchmarkingDescription.missionName);
                    Vehicle vehicle = client->GetVehicle();

                    //Mission now in ready state.
                    std::vector<double> runningTimeData;

                    //Preallocating data for 2 min at 60Hz. 
                    runningTimeData.reserve(10000);

                    Init();

                    mission.Start();

                    std::chrono::steady_clock::time_point startTime, endTime;
                    std::chrono::milliseconds difference;

                    while (mission.GetMissionState() != MissionState::ENDED)
                    {
                        MissionInfo info = mission.GetMissionInfo();
                        startTime = std::chrono::steady_clock::now();
                        Tick(vehicle, info);
                        endTime = std::chrono::steady_clock::now();
                        difference = std::chrono::duration_cast<std::chrono::milliseconds>(endTime-startTime);
                        runningTimeData.push_back(difference.count());
                    }

                    //cleanup user values
                    Cleanup();
                    trialResultsJson["AlgorithmRuntimeMean"] = CalculateMean(runningTimeData);
                    trialResultsJson["AlgorithmRuntimeSTD"] = CalculateSTD(runningTimeData);
                    trialResultsJson["Metrics"] = nlohmann::json::parse(mission.GetMetricResults());

                    benchmarkingDescriptionJson["Results"].push_back(trialResultsJson);

                }

                benchmarkResults["Benchmarks"].push_back(benchmarkingDescriptionJson);
            }

            outputFile << std::setw(4) << benchmarkResults << std::endl;
            outputFile.close();
            std::cout << "## Benchmark Ended" << std::endl;

        }

        void BenchmarkingInterface::AddBenchmarkingDescription(BenchmarkingDescription &description)
        {

            missionDescriptionList.push_back(description);
        }

        void BenchmarkingInterface::VerifyBenchmarkDescriptions()
        {
            std::vector<std::string> availableWorlds = client->GetAvailableWorlds();
            for (auto &description : missionDescriptionList)
            {
                //check if world exists
                if (std::find(availableWorlds.begin(), availableWorlds.end(), description.worldName) == availableWorlds.end())
                {
                    throw std::runtime_error("The world '" + description.worldName + "' does not exist.");
                }

                //TODO: check if procedural locations are available (currently not possible without changing maps in unreal... ugly)
                if (description.proceduralDecriptions.size() != 0)
                {
                    World temporaryWorld = client->LoadWorld(description.worldName);
                    std::vector<std::string> proceduralLocations = temporaryWorld.GetProceduralLocations();

                    for (auto &proceduralDescription : description.proceduralDecriptions)
                    {
                        if (std::find(proceduralLocations.begin(), proceduralLocations.end(), proceduralDescription.proceduralLocation) == proceduralLocations.end())
                        {
                            throw std::runtime_error("The procedural location '" + proceduralDescription.proceduralLocation + "' does not exist in world '" + description.worldName + "'.");
                        }
                    }
                }

                return;
            }
        }

        nlohmann::json SerializeBenchmarkingDescription(const BenchmarkingDescription& description)
        {
            nlohmann::json descriptionJson;
            descriptionJson["WorldName"] = description.worldName;
            descriptionJson["MissionName"] = description.missionName;
            descriptionJson["Trials"] = description.trials;
            descriptionJson["ProceduralGenerationSettings"] = nlohmann::json::array();
            for (auto &proceduralDescription : description.proceduralDecriptions)
            {
                nlohmann::json proceduralDescriptionJson;
                proceduralDescriptionJson["Location"] = proceduralDescription.proceduralLocation;
                proceduralDescriptionJson["Radius"] = proceduralDescription.radius;
                descriptionJson["ProceduralGenerationSettings"].push_back(proceduralDescriptionJson);
            }

            descriptionJson["Results"] = nlohmann::json::array();
            return descriptionJson;
        }

        double CalculateMean(const std::vector<double>& items)
        {
            double sum = 0;
            for(auto item: items)
                sum+=item;
            return (sum/((double)items.size()));
        }

        double CalculateSTD(const std::vector<double>& items)
        {
            double sum = 0;
            for(auto item: items)
                sum+=item;
            sum = sum/((double)items.size());

            double variance = 0;
            for(auto item: items)
                variance += (pow(item - sum, 2));

            return sqrt(variance/((double)items.size()));
        }
        
    } // namespace benchmarking
};    // namespace avoid
