/**
 * @file enhanced_optical_tracker.cpp
 * @brief Implementation of enhanced optical tracking system for BULLET UAV
 */

#include "enhanced_optical_tracker.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <chrono>
#include <algorithm>

namespace bullet {
namespace sensors {

EnhancedOpticalTracker::EnhancedOpticalTracker() 
    : m_initialized(false)
    , m_camera_type(CameraType::UNKNOWN)
    , m_next_target_id(1) {
    
    // Инициализация детекторов
    m_detector = cv::ORB::create();
    m_matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
    
    // Инициализация Kalman фильтра
    initializeKalmanFilter();
}

EnhancedOpticalTracker::EnhancedOpticalTracker(const TrackerConfig& config)
    : m_config(config)
    , m_initialized(false)
    , m_camera_type(config.camera_type)
    , m_next_target_id(1) {
    
    // Инициализация детекторов
    m_detector = cv::ORB::create();
    m_matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
    
    // Инициализация Kalman фильтра
    initializeKalmanFilter();
}

EnhancedOpticalTracker::~EnhancedOpticalTracker() {
    cleanupResources();
}

void EnhancedOpticalTracker::cleanupResources() {
    if (m_camera.isOpened()) {
        m_camera.release();
    }
    
    m_trackers.clear();
    m_targets.clear();
    m_target_histories.clear();
    m_initialized = false;
}

bool EnhancedOpticalTracker::initializeCamera() {
    return initializeCamera(m_config.camera_index);
}

bool EnhancedOpticalTracker::initializeCamera(int camera_index) {
    std::cout << "Initializing camera " << camera_index << "..." << std::endl;
    
    m_camera.open(camera_index);
    
    if (!m_camera.isOpened()) {
        std::cerr << "Failed to open camera " << camera_index << std::endl;
        return false;
    }
    
    // Настройка параметров камеры
    m_camera.set(cv::CAP_PROP_FRAME_WIDTH, m_config.frame_width);
    m_camera.set(cv::CAP_PROP_FRAME_HEIGHT, m_config.frame_height);
    m_camera.set(cv::CAP_PROP_FPS, m_config.fps);
    
    // Определение типа камеры
    if (m_camera_type == CameraType::UNKNOWN) {
        m_camera_type = detectThermalCamera() ? CameraType::THERMAL : CameraType::REGULAR;
    }
    
    std::cout << "Camera initialized successfully. Type: " 
              << (m_camera_type == CameraType::THERMAL ? "THERMAL" : "REGULAR") << std::endl;
    
    m_initialized = true;
    return true;
}

bool EnhancedOpticalTracker::detectThermalCamera() {
    cv::Mat frame;
    m_camera >> frame;
    
    if (frame.empty()) {
        return false;
    }
    
    // Проверка характеристик кадра
    bool is_thermal = false;
    
    // Один канал может указывать на термальную камеру
    if (frame.channels() == 1) {
        is_thermal = true;
    }
    
    // Проверка температурного диапазона
    double temp = m_camera.get(cv::CAP_PROP_TEMPERATURE);
    
    if (temp != -1) {
        is_thermal = true;
    }
    
    // Проверка специфичных свойств (используем доступные свойства)
    double format = m_camera.get(cv::CAP_PROP_FORMAT);
    if (format == CV_8UC1) {  // Один канал может указывать на термальную камеру
        is_thermal = true;
    }
    
    return is_thermal;
}

bool EnhancedOpticalTracker::detectAndTrackTargets() {
    if (!m_initialized || !m_camera.isOpened()) {
        return false;
    }
    
    cv::Mat frame;
    m_camera >> frame;
    
    if (frame.empty()) {
        return false;
    }
    
    // Предварительная обработка
    cv::Mat processed_frame = preprocessFrame(frame);
    
    // Детекция целей
    std::vector<cv::Rect> detections = detectTargets(processed_frame);
    
    // Инициализация новых трекеров
    if (m_trackers.size() < m_config.max_targets && !detections.empty()) {
        initializeNewTrackers(processed_frame, detections);
    }
    
    // Обновление существующих трекеров
    std::vector<bool> tracking_status;
    bool success = updateTrackers(processed_frame);
    
    // Удаление потерянных целей
    removeLostTargets(tracking_status);
    
    // Восстановление потерянных целей
    if (m_config.enable_thermal_processing && m_camera_type == CameraType::THERMAL) {
        recoverLostTargets(processed_frame);
    }
    
    return success;
}

std::vector<cv::Rect> EnhancedOpticalTracker::detectTargets(const cv::Mat& frame) {
    std::vector<cv::Rect> all_detections;
    
    // Детекция движения
    auto motion_detections = detectMotion(frame);
    
    // Детекция по форме
    auto shape_detections = detectByShape(frame);
    
    // Детекция по цвету
    auto color_detections = detectByColor(frame);
    
    // Специальная детекция для термальных камер
    std::vector<cv::Rect> thermal_detections;
    if (m_camera_type == CameraType::THERMAL) {
        thermal_detections = detectThermalTargets(frame);
    }
    
    // Объединение результатов
    all_detections = mergeDetections(motion_detections, shape_detections, color_detections);
    
    if (m_camera_type == CameraType::THERMAL) {
        all_detections.insert(all_detections.end(), thermal_detections.begin(), thermal_detections.end());
    }
    
    return all_detections;
}

std::vector<cv::Rect> EnhancedOpticalTracker::detectMotion(const cv::Mat& frame) {
    std::vector<cv::Rect> detections;
    
    if (m_previous_frame.empty()) {
        m_previous_frame = frame.clone();
        return detections;
    }
    
    cv::Mat diff;
    cv::absdiff(frame, m_previous_frame, diff);
    
    cv::Mat gray;
    if (diff.channels() == 3) {
        cv::cvtColor(diff, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = diff.clone();
    }
    
    cv::Mat thresholded;
    cv::threshold(gray, thresholded, m_config.motion_threshold * 255, 255, cv::THRESH_BINARY);
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresholded, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 500) {  // Минимальный размер для движения
            cv::Rect bbox = cv::boundingRect(contour);
            detections.push_back(bbox);
        }
    }
    
    m_previous_frame = frame.clone();
    return detections;
}

std::vector<cv::Rect> EnhancedOpticalTracker::detectByShape(const cv::Mat& frame) {
    std::vector<cv::Rect> detections;
    
    cv::Mat gray;
    if (frame.channels() == 3) {
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = frame.clone();
    }
    
    // Детекция кругов (для Shahed дронов)
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, gray.rows/8, 
                     100, 30, 20, 100);
    
    for (const auto& circle : circles) {
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);
        
        cv::Rect bbox(center.x - radius, center.y - radius, 
                     radius * 2, radius * 2);
        detections.push_back(bbox);
    }
    
    return detections;
}

std::vector<cv::Rect> EnhancedOpticalTracker::detectByColor(const cv::Mat& frame) {
    std::vector<cv::Rect> detections;
    
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    
    // Детекция красного цвета (для Shahed дронов)
    cv::Mat red_mask1, red_mask2;
    cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), red_mask1);
    cv::inRange(hsv, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), red_mask2);
    
    cv::Mat red_mask = red_mask1 | red_mask2;
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(red_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 200) {
            cv::Rect bbox = cv::boundingRect(contour);
            detections.push_back(bbox);
        }
    }
    
    return detections;
}

std::vector<cv::Rect> EnhancedOpticalTracker::detectThermalTargets(const cv::Mat& frame) {
    std::vector<cv::Rect> detections;
    
    cv::Mat gray;
    if (frame.channels() == 3) {
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = frame.clone();
    }
    
    // Пороговая обработка для выделения горячих объектов
    cv::Mat thresholded;
    cv::threshold(gray, thresholded, m_config.thermal_threshold, 255, cv::THRESH_BINARY);
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresholded, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 100) {  // Минимальный размер горячего объекта
            cv::Rect bbox = cv::boundingRect(contour);
            detections.push_back(bbox);
        }
    }
    
    return detections;
}

bool EnhancedOpticalTracker::initializeTrackers(const cv::Mat& frame, const std::vector<cv::Rect>& detections) {
    m_trackers.clear();
    m_targets.clear();
    m_target_histories.clear();
    
    for (const auto& detection : detections) {
        if (m_trackers.size() >= m_config.max_targets) break;
        
        cv::Ptr<cv::Tracker> tracker = selectBestTracker(frame, detection);
        
        if (tracker) {
            tracker->init(frame, detection);
            m_trackers.push_back(tracker);
            
            // Создание новой цели
            targeting::Target new_target;
            new_target.id = generateTargetId();
            new_target.type = classifyTarget(frame, detection);
            new_target.confidence = 0.8;
            m_targets.push_back(new_target);
            
            // Инициализация истории
            m_target_histories.emplace_back();
        }
    }
    
    return !m_trackers.empty();
}

cv::Ptr<cv::Tracker> EnhancedOpticalTracker::selectBestTracker(const cv::Mat& frame, const cv::Rect& roi) {
    double area = roi.width * roi.height;
    double aspect_ratio = static_cast<double>(roi.width) / roi.height;
    
    // Выбор трекера на основе характеристик цели
    if (area < 1000) {
        return cv::TrackerKCF::create();
    } else if (aspect_ratio > 2.0) {
        return cv::TrackerCSRT::create();
    } else {
        return cv::TrackerKCF::create();  // MOSSE не доступен в OpenCV 4.x
    }
}

bool EnhancedOpticalTracker::updateTrackers(const cv::Mat& frame) {
    std::vector<bool> tracking_status;
    
    for (size_t i = 0; i < m_trackers.size(); ++i) {
        cv::Rect bbox;
        bool success = m_trackers[i]->update(frame, bbox);
        
        tracking_status.push_back(success);
        
        if (success) {
            updateTargetData(i, bbox);
        }
    }
    
    return !tracking_status.empty();
}

void EnhancedOpticalTracker::updateTargetData(size_t target_index, const cv::Rect& bbox) {
    if (target_index >= m_targets.size()) return;
    
    targeting::Target& target = m_targets[target_index];
    
    // Обновление позиции
    cv::Point2f current_pos(bbox.x + bbox.width/2.0, bbox.y + bbox.height/2.0);
    
    // Добавление в историю
    TargetHistory history_entry;
    history_entry.position = current_pos;
    history_entry.bounding_box = bbox;
    history_entry.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    m_target_histories[target_index].push_back(history_entry);
    
    // Ограничение размера истории
    if (m_target_histories[target_index].size() > m_config.history_size) {
        m_target_histories[target_index].pop_front();
    }
    
    // Расчет скорости
    if (m_target_histories[target_index].size() >= 2) {
        cv::Point2f velocity = calculateVelocity(m_target_histories[target_index]);
        target.velocity[0] = velocity.x;
        target.velocity[1] = velocity.y;
    }
    
    // Обновление координат
    target.lat = current_pos.x;
    target.lon = current_pos.y;
    
    // Обновление уверенности
    target.confidence = calculateConfidence(bbox, m_target_histories[target_index]);
}

cv::Point2f EnhancedOpticalTracker::calculateVelocity(const std::deque<TargetHistory>& history) {
    if (history.size() < 2) {
        return cv::Point2f(0.0, 0.0);
    }
    
    const auto& current = history.back();
    const auto& previous = history[history.size() - 2];
    
    uint64_t time_diff = current.timestamp - previous.timestamp;
    if (time_diff == 0) return cv::Point2f(0.0, 0.0);
    
    float dx = (current.position.x - previous.position.x) / time_diff * 1000.0f;
    float dy = (current.position.y - previous.position.y) / time_diff * 1000.0f;
    
    return cv::Point2f(dx, dy);
}

double EnhancedOpticalTracker::calculateConfidence(const cv::Rect& bbox, const std::deque<TargetHistory>& history) {
    if (history.empty()) return 0.0;
    
    double confidence = 0.8;  // Базовая уверенность
    
    // Увеличение уверенности при стабильном трекинге
    if (history.size() > 5) {
        confidence += 0.1;
    }
    
    // Уменьшение уверенности при больших изменениях размера
    if (history.size() >= 2) {
        const auto& current = history.back();
        const auto& previous = history[history.size() - 2];
        
        double size_change = std::abs(current.bounding_box.area() - previous.bounding_box.area()) / 
                           static_cast<double>(previous.bounding_box.area());
        
        if (size_change > 0.5) {
            confidence -= 0.2;
        }
    }
    
    return std::max(0.0, std::min(1.0, confidence));
}

void EnhancedOpticalTracker::removeLostTargets(const std::vector<bool>& tracking_status) {
    for (int i = static_cast<int>(tracking_status.size()) - 1; i >= 0; --i) {
        if (!tracking_status[i] || m_targets[i].confidence < m_config.min_confidence) {
            m_trackers.erase(m_trackers.begin() + i);
            m_targets.erase(m_targets.begin() + i);
            m_target_histories.erase(m_target_histories.begin() + i);
        }
    }
}

void EnhancedOpticalTracker::initializeNewTrackers(const cv::Mat& frame, const std::vector<cv::Rect>& detections) {
    for (const auto& detection : detections) {
        if (m_trackers.size() >= m_config.max_targets) break;
        
        // Проверка, не отслеживается ли уже эта область
        bool already_tracked = false;
        for (const auto& target : m_targets) {
            cv::Rect existing_bbox(target.lat - 50, target.lon - 50, 100, 100);
            if ((detection & existing_bbox).area() > 0) {
                already_tracked = true;
                break;
            }
        }
        
        if (!already_tracked) {
            cv::Ptr<cv::Tracker> tracker = selectBestTracker(frame, detection);
            
            if (tracker) {
                tracker->init(frame, detection);
                m_trackers.push_back(tracker);
                
                targeting::Target new_target;
                new_target.id = generateTargetId();
                new_target.type = classifyTarget(frame, detection);
                new_target.confidence = 0.7;
                m_targets.push_back(new_target);
                
                m_target_histories.emplace_back();
            }
        }
    }
}

uint32_t EnhancedOpticalTracker::generateTargetId() {
    return m_next_target_id++;
}

targeting::TargetType EnhancedOpticalTracker::classifyTarget(const cv::Mat& frame, const cv::Rect& detection) {
    // Простая классификация на основе размера и формы
    double area = detection.area();
    double aspect_ratio = static_cast<double>(detection.width) / detection.height;
    
    if (area < 1000) {
        return targeting::TargetType::SHAHED_DRONE;
    } else if (aspect_ratio > 2.0) {
        return targeting::TargetType::HELICOPTER;
    } else {
        return targeting::TargetType::UNKNOWN;
    }
}

bool EnhancedOpticalTracker::recoverLostTargets(const cv::Mat& frame) {
    bool recovered = false;
    
    for (size_t i = 0; i < m_targets.size(); ++i) {
        if (m_targets[i].confidence < m_config.min_confidence) {
            cv::Rect recovered_bbox = searchTarget(frame, m_targets[i]);
            
            if (recovered_bbox.width > 0) {
                m_trackers[i] = selectBestTracker(frame, recovered_bbox);
                m_trackers[i]->init(frame, recovered_bbox);
                m_targets[i].confidence = 0.6;
                recovered = true;
            }
        }
    }
    
    return recovered;
}

cv::Rect bullet::sensors::EnhancedOpticalTracker::searchTarget(const cv::Mat& frame, const targeting::Target& target) {
    // Простой поиск в расширенной области
    cv::Point2f last_pos(target.lat, target.lon);
    int search_radius = 100;
    
    cv::Rect search_area(
        std::max(0, static_cast<int>(last_pos.x - search_radius)),
        std::max(0, static_cast<int>(last_pos.y - search_radius)),
        std::min(frame.cols - static_cast<int>(last_pos.x - search_radius), search_radius * 2),
        std::min(frame.rows - static_cast<int>(last_pos.y - search_radius), search_radius * 2)
    );
    
    // Поиск в области
    std::vector<cv::Rect> detections = detectTargets(frame(search_area));
    
    for (const auto& detection : detections) {
        cv::Rect global_detection = detection + cv::Point(search_area.x, search_area.y);
        if (cv::norm(cv::Point2f(global_detection.x + global_detection.width/2.0,
                                global_detection.y + global_detection.height/2.0) - last_pos) < search_radius) {
            return global_detection;
        }
    }
    
    return cv::Rect();
}

cv::Mat bullet::sensors::EnhancedOpticalTracker::preprocessFrame(const cv::Mat& frame) {
    cv::Mat processed = frame.clone();
    
    if (m_camera_type == CameraType::THERMAL) {
        processed = processThermalFrame(frame);
    } else {
        // Нормальная обработка для обычной камеры
        if (processed.channels() == 3) {
            cv::cvtColor(processed, processed, cv::COLOR_BGR2GRAY);
        }
        cv::equalizeHist(processed, processed);
    }
    
    return processed;
}

cv::Mat bullet::sensors::EnhancedOpticalTracker::processThermalFrame(const cv::Mat& frame) {
    cv::Mat processed;
    
    if (frame.channels() == 3) {
        cv::cvtColor(frame, processed, cv::COLOR_BGR2GRAY);
    } else {
        processed = frame.clone();
    }
    
    // Нормализация для лучшей видимости
    cv::normalize(processed, processed, 0, 255, cv::NORM_MINMAX);
    
    return processed;
}

void bullet::sensors::EnhancedOpticalTracker::detectHotObjects(const cv::Mat& frame, std::vector<cv::Rect>& hot_objects) {
    hot_objects = detectThermalTargets(frame);
}

void bullet::sensors::EnhancedOpticalTracker::initializeKalmanFilter() {
    // 4 состояния: x, y, vx, vy
    // 2 измерения: x, y
    m_kalman_filter = cv::KalmanFilter(4, 2, 0);
    
    // Матрица перехода
    m_kalman_filter.transitionMatrix = (cv::Mat_<float>(4, 4) << 
        1, 0, 1, 0,
        0, 1, 0, 1,
        0, 0, 1, 0,
        0, 0, 0, 1);
    
    // Матрица измерения
    m_kalman_filter.measurementMatrix = (cv::Mat_<float>(2, 4) << 
        1, 0, 0, 0,
        0, 1, 0, 0);
    
    // Шум процесса
    m_kalman_filter.processNoiseCov = cv::Mat::eye(4, 4, CV_32F) * m_config.kalman_process_noise;
    
    // Шум измерения
    m_kalman_filter.measurementNoiseCov = cv::Mat::eye(2, 2, CV_32F) * m_config.kalman_measurement_noise;
}

std::vector<cv::Rect> bullet::sensors::EnhancedOpticalTracker::mergeDetections(
    const std::vector<cv::Rect>& detections1,
    const std::vector<cv::Rect>& detections2,
    const std::vector<cv::Rect>& detections3) {
    
    std::vector<cv::Rect> merged = detections1;
    merged.insert(merged.end(), detections2.begin(), detections2.end());
    merged.insert(merged.end(), detections3.begin(), detections3.end());
    
    // Удаление дубликатов
    std::vector<cv::Rect> result;
    for (const auto& detection : merged) {
        bool is_duplicate = false;
        for (const auto& existing : result) {
            if ((detection & existing).area() > 0.5 * std::min(detection.area(), existing.area())) {
                is_duplicate = true;
                break;
            }
        }
        if (!is_duplicate) {
            result.push_back(detection);
        }
    }
    
    return result;
}

// Геттеры и сеттеры
std::vector<targeting::Target> bullet::sensors::EnhancedOpticalTracker::getActiveTargets() const {
    return m_targets;
}

bool bullet::sensors::EnhancedOpticalTracker::isThermalCamera() const {
    return m_camera_type == CameraType::THERMAL;
}

bool bullet::sensors::EnhancedOpticalTracker::isInitialized() const {
    return m_initialized;
}

bullet::sensors::CameraType bullet::sensors::EnhancedOpticalTracker::getCameraType() const {
    return m_camera_type;
}

void bullet::sensors::EnhancedOpticalTracker::setConfig(const TrackerConfig& config) {
    m_config = config;
}

bullet::sensors::TrackerConfig bullet::sensors::EnhancedOpticalTracker::getConfig() const {
    return m_config;
}

bullet::sensors::EnhancedOpticalTracker::PerformanceMetrics bullet::sensors::EnhancedOpticalTracker::getPerformanceMetrics() const {
    return m_performance;
}

void bullet::sensors::EnhancedOpticalTracker::updatePerformanceMetrics(const PerformanceMetrics& metrics) {
    m_performance = metrics;
    m_performance_history.push_back(metrics);
    
    if (m_performance_history.size() > 100) {
        m_performance_history.pop_front();
    }
}

cv::Mat bullet::sensors::EnhancedOpticalTracker::getLastFrame() const {
    return m_previous_frame;
}

} // namespace sensors
} // namespace bullet 