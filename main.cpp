#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <algorithm>

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <template_image> <source_image> <output_image>\n";
        return 1;
    }

    cv::Mat img1 = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    cv::Mat img2 = cv::imread(argv[2], cv::IMREAD_GRAYSCALE);
    cv::Mat img2_color = cv::imread(argv[2], cv::IMREAD_COLOR);

    if (img1.empty() || img2.empty() || img2_color.empty()) {
        std::cerr << "Couldn't properly load images!" << std::endl;
        return 1;
    }

    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    std::vector<cv::KeyPoint> kp1, kp2;
    cv::Mat des1, des2;
    sift->detectAndCompute(img1, cv::noArray(), kp1, des1);
    sift->detectAndCompute(img2, cv::noArray(), kp2, des2);

    cv::BFMatcher bf;
    std::vector<std::vector<cv::DMatch>> matches;
    bf.knnMatch(des1, des2, matches, 2);

    std::vector<cv::DMatch> good;
    for (const auto &m_n : matches) {
        if (m_n.size() >= 2 && m_n[0].distance < 0.75f * m_n[1].distance)
            good.push_back(m_n[0]);
    }

    if (good.size() < 4) return 1;

    std::vector<cv::Point2f> src_pts, dst_pts;
    for (const auto &m : good) {
        src_pts.push_back(kp1[m.queryIdx].pt);
        dst_pts.push_back(kp2[m.trainIdx].pt);
    }

    cv::Mat M = cv::findHomography(src_pts, dst_pts, cv::RANSAC, 5.0);

    std::vector<cv::Point2f> pts = { {0,0}, {0,(float)img1.rows}, {(float)img1.cols,(float)img1.rows}, {(float)img1.cols,0} };
    std::vector<cv::Point2f> dst;
    cv::perspectiveTransform(pts, dst, M);

    std::vector<cv::Point> dst_int;
    for (auto &p : dst) dst_int.push_back(cv::Point((int)p.x, (int)p.y));

    cv::Rect roi = cv::boundingRect(dst_int);
    roi.x = std::max(roi.x, 0);
    roi.y = std::max(roi.y, 0);
    roi.width = std::min(roi.width, img2_color.cols - roi.x);
    roi.height = std::min(roi.height, img2_color.rows - roi.y);

    cv::Mat cropped = img2_color(roi);
    cv::imwrite(argv[3], cropped);

    std::cout << "Saved best chunk at x=" << roi.x << ", y=" << roi.y << ", w=" << roi.width << ", h=" << roi.height << std::endl;
    std::cout << "Saved to " << argv[3] << std::endl;

    return 0;
}