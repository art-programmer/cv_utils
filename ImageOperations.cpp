#include "cv_utils.h"

#include <opencv2/core/core.hpp>
#include <vector>

using namespace std;
using namespace cv;

namespace cv_utils
{
  
  template<typename Func> Mat drawValuesOnImage(const vector<double> &values, const int IMAGE_WIDTH, const int IMAGE_HEIGHT, const Func &func)
  {
    Mat image(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
      for (int x = 0; x < IMAGE_WIDTH; x++) {
	int color = round(func(values[y * IMAGE_WIDTH + x]));
	image.at<uchar>(y, x) = max(min(color, 255), 0);
      }
    }
    return image;
  }
  
  vector<double> calcBoxIntegrationMask(const vector<double> &values, const int IMAGE_WIDTH, const int IMAGE_HEIGHT)
  {
    vector<double> mask = values;
    for (int y = 0; y < IMAGE_HEIGHT; y++)
      for (int x = 1; x < IMAGE_WIDTH; x++)
	mask[y * IMAGE_WIDTH + x] += mask[y * IMAGE_WIDTH + (x - 1)];
    for (int x = 0; x < IMAGE_WIDTH; x++)
      for (int y = 1; y < IMAGE_HEIGHT; y++)
	mask[y * IMAGE_WIDTH + x] += mask[(y - 1) * IMAGE_WIDTH + x];
    return mask;
  }

  double calcBoxIntegration(const vector<double> &mask, const int IMAGE_WIDTH, const int IMAGE_HEIGHT, const int pixel_1, const int pixel_2)
  {
    int x_1 = pixel_1 % IMAGE_WIDTH;
    int y_1 = pixel_1 / IMAGE_WIDTH;
    int x_2 = pixel_2 % IMAGE_WIDTH;
    int y_2 = pixel_2 / IMAGE_WIDTH;
    int min_x = min(x_1, x_2) - 1;
    int min_y = min(y_1, y_2) - 1;
    int max_x = min(max(x_1, x_2), IMAGE_WIDTH - 1);
    int max_y = min(max(y_1, y_2), IMAGE_HEIGHT - 1);
    double value_1 = (min_x >= 0 && min_y >= 0) ? mask[min_y * IMAGE_WIDTH + min_x] : 0;
    double value_2 = min_x >= 0 ? mask[max_y * IMAGE_WIDTH + min_x] : 0;
    double value_3 = min_y >= 0 ? mask[min_y * IMAGE_WIDTH + max_x] : 0;
    double value_4 = mask[max_y * IMAGE_WIDTH + max_y];
    return (value_1 + value_4) - (value_2 + value_3);
  }
  
  void calcWindowMeansAndVars(const std::vector<double> &values, const int IMAGE_WIDTH, const int IMAGE_HEIGHT, const int WINDOW_SIZE, vector<double> &means, vector<double> &vars)
  {
    vector<double> sum_mask = calcBoxIntegrationMask(values, IMAGE_WIDTH, IMAGE_HEIGHT);
    vector<double> values2(IMAGE_WIDTH * IMAGE_HEIGHT);
    transform(values.begin(), values.end(), values2.begin(), [](const double &x) { return pow(x, 2); });
    vector<double> sum2_mask = calcBoxIntegrationMask(values2, IMAGE_WIDTH, IMAGE_HEIGHT);
    vector<double> means(IMAGE_WIDTH * IMAGE_HEIGHT);
    vector<double> vars(IMAGE_WIDTH * IMAGE_HEIGHT);
    for (int pixel = 0; pixel < IMAGE_WIDTH * IMAGE_HEIGHT; pixel++) {
      int corner_pixel_1 = pixel - (WINDOW_SIZE - 1) / 2 * IMAGE_WIDTH - (WINDOW_SIZE - 1) / 2;
      int corner_pixel_2 = pixel + (WINDOW_SIZE - 1) / 2 * IMAGE_WIDTH + (WINDOW_SIZE - 1) / 2;
      int area = (min(pixel % IMAGE_WIDTH + (WINDOW_SIZE - 1) / 2, IMAGE_WIDTH - 1) - max(pixel % IMAGE_WIDTH - (WINDOW_SIZE - 1) / 2, 0) + 1) * (min(pixel / IMAGE_WIDTH + (WINDOW_SIZE - 1) / 2, IMAGE_HEIGHT - 1) - max(pixel / IMAGE_WIDTH - (WINDOW_SIZE - 1) / 2, 0) + 1);
      double mean = calcBoxIntegration(sum_mask, IMAGE_WIDTH, IMAGE_HEIGHT, corner_pixel_1, corner_pixel_2) / area;
      double var = calcBoxIntegration(sum2_mask, IMAGE_WIDTH, IMAGE_HEIGHT, corner_pixel_1, corner_pixel_2) / area - pow(mean, 2);
      means[pixel] = mean;
      vars[pixel] = var;
    }
  }

  void guidedFilter(const cv::Mat &guidance_image, const cv::Mat &input_image, cv::Mat &output_image, const double radius, const double epsilon)
  {
    if (guidance_image.channels() == 1 && input_image.channels()
  }
  
}

