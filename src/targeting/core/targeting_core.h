/**
 * @file targeting_core.h
 * @brief Main targeting system core for BULLET UAV
 * 
 * Handles target detection, classification, and engagement coordination
 * for Shahed drones, helicopters, and ground targets.
 */

#ifndef TARGETING_CORE_H
#define TARGETING_CORE_H

#include <vector>
#include <memory>

namespace bullet {
namespace targeting {

enum class TargetType {
    SHAHED_DRONE,
    HELICOPTER, 
    GROUND_VEHICLE,
    AIR_DEFENSE_SYSTEM,  // S-300/400, Buk
    MARITIME_TARGET,
    UNKNOWN
};

struct Target {
    uint32_t id;
    TargetType type;
    double lat, lon, alt;
    double velocity[3];  // x, y, z
    double confidence;
    uint64_t timestamp;
};

class TargetingCore {
public:
    TargetingCore();
    ~TargetingCore();
    
    bool initializeSystem();
    std::vector<Target> detectTargets();
    Target selectPrimaryTarget(const std::vector<Target>& targets);
    bool calculateEngagementSolution(const Target& target);
    
private:
    bool m_initialized;
    // Implementation details
};

} // namespace targeting
} // namespace bullet

#endif // TARGETING_CORE_H
