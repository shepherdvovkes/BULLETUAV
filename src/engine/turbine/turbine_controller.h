/**
 * @file turbine_controller.h
 * @brief Turbine engine controller for BULLET UAV
 */

#ifndef TURBINE_CONTROLLER_H
#define TURBINE_CONTROLLER_H

#include <cstdint>

namespace bullet {
namespace engine {

enum class EngineState {
    STOPPED,
    STARTING,
    RUNNING,
    SHUTDOWN,
    ERROR
};

struct EngineParameters {
    float rpm;           // Revolutions per minute
    float egt;           // Exhaust gas temperature (Celsius)
    float fuel_flow;     // Liters per hour
    float oil_pressure;  // Bar
    float thrust;        // Newtons
};

class TurbineController {
public:
    TurbineController();
    ~TurbineController();
    
    bool startupSequence();
    bool controlThrust(float thrust_percent);
    EngineParameters monitorEngine();
    bool emergencyShutdown();
    
    EngineState getState() const { return m_state; }
    
private:
    EngineState m_state;
    EngineParameters m_params;
    
    bool validateParameters();
    void overspeedProtection();
    void overtemperatureProtection();
    void flameOutDetection();
};

} // namespace engine
} // namespace bullet

#endif // TURBINE_CONTROLLER_H
