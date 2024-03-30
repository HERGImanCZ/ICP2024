#include <iostream>
#include <opencv2/opencv.hpp>

class CyanTracker {
public:
    CyanTracker();
    bool init(void);
    int run(void);
    ~CyanTracker();

private:
    cv::VideoCapture capture;

    void draw_cross_normalized(cv::Mat& img, cv::Point2f center_relative, int size);
    void draw_cross(cv::Mat& img, int x, int y, int size);
    cv::Point2f find_center_normalized(cv::Mat& frame);
    cv::Point2f find_center_normalized_hsv(cv::Mat& frame);
};

CyanTracker::CyanTracker() {}

bool CyanTracker::init() {
    try {
        capture = cv::VideoCapture(cv::CAP_DSHOW);

        if (!capture.isOpened()) {
            std::cerr << "no camera source? Fallback to video..." << std::endl;

            capture = cv::VideoCapture("resources/video.mkv");
            if (!capture.isOpened()) {
                std::cerr << "no source?... " << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        std::cout << "Source: " <<
            ": width=" << capture.get(cv::CAP_PROP_FRAME_WIDTH) <<
            ", height=" << capture.get(cv::CAP_PROP_FRAME_HEIGHT) << '\n';
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << std::endl;
        throw;
    }

    return true;
}

int CyanTracker::run(void) {
    cv::Mat frame;

    try {
        while (true) {
            capture >> frame;

            if (frame.empty())
                throw std::exception("Empty file? Wrong path?");

            auto start = std::chrono::steady_clock::now();

            cv::Mat frame2;
            frame.copyTo(frame2);

            cv::Point2f center_normalized = find_center_normalized_hsv(frame);

            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            std::cout << "Elapsed time: " << elapsed_seconds.count() << "sec" << std::endl;

            draw_cross_normalized(frame2, center_normalized, 25);

            cv::namedWindow("frame");
            cv::imshow("frame", frame);

            cv::namedWindow("frame2");
            cv::imshow("frame2", frame2);

            int key = cv::waitKey(1);
            if (key == 27)
                break;
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

CyanTracker::~CyanTracker() {
    if (capture.isOpened())
        capture.release();
    cv::destroyAllWindows();
    std::cout << "Bye...\n";
}

class App {
public:
    App() {}
    bool init() { return cyanTracker.init(); }
    int run() { return cyanTracker.run(); }
private:
    CyanTracker cyanTracker;
};

int main() {
    App app;
    if (app.init())
        return app.run();
}

void CyanTracker::draw_cross(cv::Mat& img, int x, int y, int size) {
    cv::Point p1(x - size / 2, y);
    cv::Point p2(x + size / 2, y);
    cv::Point p3(x, y - size / 2);
    cv::Point p4(x, y + size / 2);

    cv::line(img, p1, p2, CV_RGB(255, 0, 0), 3);
    cv::line(img, p3, p4, CV_RGB(255, 0, 0), 3);
}

cv::Point2f CyanTracker::find_center_normalized(cv::Mat& frame) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    cv::Point2f center;
    cv::Moments m = cv::moments(gray, true);
    center.x = m.m10 / m.m00;
    center.y = m.m01 / m.m00;

    cv::Point2f center_normalized;
    center_normalized.x = center.x / frame.cols;
    center_normalized.y = center.y / frame.rows;

    std::cout << "Center absolute: " << center << '\n';
    std::cout << "Center normalized: " << center_normalized << '\n';

    return center_normalized;
}

cv::Point2f CyanTracker::find_center_normalized_hsv(cv::Mat& frame) {
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Mat mask;
    cv::inRange(hsv, cv::Scalar(80, 50, 50), cv::Scalar(100, 255, 255), mask);

    cv::Point2f center;
    cv::Moments m = cv::moments(mask, true);
    center.x = m.m10 / m.m00;
    center.y = m.m01 / m.m00;

    cv::Point2f center_normalized;
    center_normalized.x = center.x / frame.cols;
    center_normalized.y = center.y / frame.rows;

    std::cout << "Center absolute: " << center << '\n';
    std::cout << "Center normalized: " << center_normalized << '\n';

    return center_normalized;
}

void CyanTracker::draw_cross_normalized(cv::Mat& img, cv::Point2f center_normalized, int size) {
    center_normalized.x = std::clamp(center_normalized.x, 0.0f, 1.0f);
    center_normalized.y = std::clamp(center_normalized.y, 0.0f, 1.0f);
    size = std::clamp(size, 1, std::min(img.cols, img.rows));

    cv::Point2f center_absolute(center_normalized.x * img.cols, center_normalized.y * img.rows);

    cv::Point2f p1(center_absolute.x - size / 2, center_absolute.y);
    cv::Point2f p2(center_absolute.x + size / 2, center_absolute.y);
    cv::Point2f p3(center_absolute.x, center_absolute.y - size / 2);
    cv::Point2f p4(center_absolute.x, center_absolute.y + size / 2);

    cv::line(img, p1, p2, CV_RGB(255, 0, 0), 3);
    cv::line(img, p3, p4, CV_RGB(255, 0, 0), 3);
}
