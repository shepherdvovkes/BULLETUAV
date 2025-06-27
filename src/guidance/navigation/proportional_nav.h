/**
 * @file proportional_nav.h
 * @brief Proportional navigation guidance for BULLET UAV
 */

#ifndef PROPORTIONAL_NAV_H
#define PROPORTIONAL_NAV_H

#include "../../targeting/core/targeting_core.h"

namespace bullet {
namespace guidance {

struct NavigationState {
    double position[3];    // x, y, z in meters
    double velocity[3];    // vx, vy, vz in m/s
    double acceleration[3]; // ax, ay, az in m/s²
};

struct GuidanceCommand {
    double lateral_acceleration;  // m/s²
    double vertical_acceleration; // m/s²
    double speed_command;        // m/s
};

class ProportionalNavigation {
public:
    ProportionalNavigation();
    ~ProportionalNavigation();
    
    GuidanceCommand calculateGuidanceCommand(
        const NavigationState& missile_state,
        const targeting::Target& target);
    
    void updateNavigationGain(double N);
    void compensateForLatency(double latency_ms);
    bool handleTargetManeuvers(const targeting::Target& target);
    
private:
    double m_navigation_gain;  // N factor (typically 3-5)
    double m_latency_compensation;
    
    double calculateLOSRate(const NavigationState& missile_state,
                           const targeting::Target& target);
    double calculateClosingVelocity(const NavigationState& missile_state,
                                   const targeting::Target& target);
};

} // namespace guidance
} // namespace bullet

#endif // PROPORTIONAL_NAV_H
