/**
 * @file enhanced_optical_tracker.h
 * @brief Enhanced optical tracking system for BULLET UAV with thermal camera support
 */

#ifndef ENHANCED_OPTICAL_TRACKER_H
#define ENHANCED_OPTICAL_TRACKER_H

#include "../../targeting/core/targeting_core.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/features2d.hpp>
#include <vector>
#include <memory>
#include <deque>

namespace bullet {
namespace sensors {

enum class CameraType {
    REGULAR,
    THERMAL,
    UNKNOWN
};

enum class TrackerType {
    KCF,
    CSRT,
    MOSSE,
    MIL,
    BOOSTING,
    TLD,
    MEDIANFLOW,
    GOTURN
};

struct TrackerConfig {
    // Камера
    int camera_index = 0;
    int frame_width = 1920;
    int frame_height = 1080;
    int fps = 60;
    CameraType camera_type = CameraType::UNKNOWN;
    
    // Детекция
    double motion_threshold = 0.02;
    double shape_confidence = 0.7;
    double color_tolerance = 0.1;
    double thermal_threshold = 200.0;  // Порог для термальной камеры
    
    // Трекинг
    int max_targets = 10;
    double min_confidence = 0.6;
    int history_size = 50;
    double prediction_threshold = 0.8;
    
    // Фильтрация
    double kalman_process_noise = 0.01;
    double kalman_measurement_noise = 0.1;
    
    // Производительность
    bool use_gpu = false;
    int num_threads = 4;
    bool enable_thermal_processing = true;
};

struct TargetHistory {
    cv::Point2f position;
    double confidence;
    uint64_t timestamp;
    cv::Rect bounding_box;
};

class EnhancedOpticalTracker {
public:
    EnhancedOpticalTracker();
    explicit EnhancedOpticalTracker(const TrackerConfig& config);
    ~EnhancedOpticalTracker();
    
    // Основные функции
    bool initializeCamera();
    bool initializeCamera(int camera_index);
    bool detectAndTrackTargets();
    std::vector<targeting::Target> getActiveTargets() const;
    
    // Детекция целей
    std::vector<cv::Rect> detectTargets(const cv::Mat& frame);
    std::vector<cv::Rect> detectMotion(const cv::Mat& frame);
    std::vector<cv::Rect> detectByShape(const cv::Mat& frame);
    std::vector<cv::Rect> detectByColor(const cv::Mat& frame);
    std::vector<cv::Rect> detectThermalTargets(const cv::Mat& frame);
    
    // Трекинг
    bool initializeTrackers(const cv::Mat& frame, const std::vector<cv::Rect>& detections);
    bool updateTrackers(const cv::Mat& frame);
    cv::Ptr<cv::Tracker> selectBestTracker(const cv::Mat& frame, const cv::Rect& roi);
    
    // Обработка термальных данных
    bool isThermalCamera() const;
    cv::Mat processThermalFrame(const cv::Mat& frame);
    void detectHotObjects(const cv::Mat& frame, std::vector<cv::Rect>& hot_objects);
    
    // Фильтрация и предсказание
    void updateTargetData(size_t target_index, const cv::Rect& bbox);
    cv::Point2f calculateVelocity(const std::deque<TargetHistory>& history);
    double calculateConfidence(const cv::Rect& bbox, const std::deque<TargetHistory>& history);
    
    // Восстановление целей
    bool recoverLostTargets(const cv::Mat& frame);
    cv::Rect searchTarget(const cv::Mat& frame, const targeting::Target& target);
    
    // Управление целями
    void removeLostTargets(const std::vector<bool>& tracking_status);
    void initializeNewTrackers(const cv::Mat& frame, const std::vector<cv::Rect>& detections);
    uint32_t generateTargetId();
    targeting::TargetType classifyTarget(const cv::Mat& frame, const cv::Rect& detection);
    
    // Конфигурация
    void setConfig(const TrackerConfig& config);
    TrackerConfig getConfig() const;
    
    // Диагностика
    struct PerformanceMetrics {
        double fps;
        double avg_tracking_time;
        double memory_usage;
        int active_targets;
        double success_rate;
        int thermal_detections;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    void updatePerformanceMetrics(const PerformanceMetrics& metrics);
    
    // Утилиты
    cv::Mat getLastFrame() const;
    bool isInitialized() const;
    CameraType getCameraType() const;

private:
    // Основные компоненты
    cv::VideoCapture m_camera;
    std::vector<cv::Ptr<cv::Tracker>> m_trackers;
    std::vector<targeting::Target> m_targets;
    std::vector<std::deque<TargetHistory>> m_target_histories;
    
    // Детекторы и матчеры
    cv::Ptr<cv::Feature2D> m_detector;
    cv::Ptr<cv::DescriptorMatcher> m_matcher;
    
    // Фильтры
    cv::KalmanFilter m_kalman_filter;
    cv::Mat m_previous_frame;
    
    // Конфигурация
    TrackerConfig m_config;
    bool m_initialized;
    CameraType m_camera_type;
    
    // Производительность
    mutable PerformanceMetrics m_performance;
    std::deque<PerformanceMetrics> m_performance_history;
    
    // Вспомогательные функции
    bool detectThermalCamera();
    cv::Mat preprocessFrame(const cv::Mat& frame);
    std::vector<cv::Rect> mergeDetections(const std::vector<cv::Rect>& detections1,
                                         const std::vector<cv::Rect>& detections2,
                                         const std::vector<cv::Rect>& detections3);
    void initializeKalmanFilter();
    void cleanupResources();
    
    // Идентификаторы
    uint32_t m_next_target_id;
};

} // namespace sensors
} // namespace bullet

#endif // ENHANCED_OPTICAL_TRACKER_H 