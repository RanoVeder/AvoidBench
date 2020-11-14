#pragma once

#include <vector>
#include <memory>
#include "Avoid/Client.h"
#include "Avoid/Mission.h"

namespace avoid
{
    namespace benchmarking
    {

        struct ProceduralGenerationDescription
        {
            std::string proceduralLocation;
            float radius;
        };

        struct BenchmarkingDescription
        {
            std::string worldName;
            std::string missionName;
            std::vector<ProceduralGenerationDescription> proceduralDecriptions;
            int trials;
        };

        class BenchmarkingInterface
        {
        public:
            BenchmarkingInterface(const std::string &ip, uint16_t port);

            void AddBenchmarkingDescription(BenchmarkingDescription &description);
            void Run(std::string outputFolder);
            
        protected:
            //virtual function
            virtual void Init() = 0;
            virtual void Tick(Vehicle &vehicle, MissionInfo &missionInfo) = 0;
            virtual void Cleanup() = 0;
            
        private:
            void VerifyBenchmarkDescriptions();

            


        private:
            std::unique_ptr<::avoid::Client> client;
            std::vector<BenchmarkingDescription> missionDescriptionList;
            std::string outputFolder;
        };
    } // namespace benchmarking
} // namespace avoid