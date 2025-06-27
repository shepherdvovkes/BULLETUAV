/**
 * @file optical_tracker.h
 * @brief Optical tracking system for BULLET UAV
 */

#ifndef OPTICAL_TRACKER_H
#define OPTICAL_TRACKER_H

#include "../../targeting/core/targeting_core.h"
#include <opencv2/opencv.hpp>

namespace bullet {
namespace sensors {

class OpticalTracker {
public:
    OpticalTracker();
    ~OpticalTracker();
    
    bool initializeCamera();
    bool trackTarget(targeting::Target& target);
    bool lostTargetRecovery();
    cv::Point2d calculateTargetVelocity();
    
private:
    cv::VideoCapture m_camera;
    cv::Ptr<cv::Tracker> m_tracker;
    bool m_initialized;
};

} // namespace sensors
} // namespace bullet

#endif // OPTICAL_TRACKER_H
