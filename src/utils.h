#pragma once
#include <vector>

struct Color { unsigned char r = 0, g = 0, b = 0; };
struct Rect { int x = 0, y = 0, w = 0, h = 0; };

bool intersects(const Rect &r1, const Rect &r2);
Rect intersect(const Rect &r1, const Rect &r2);

void initUtils();

void drawOutlineRect(unsigned char *img, int img_w, int img_h, const Rect &rect, const Color &color);
void drawRect(unsigned char *img, int img_w, int img_h, const Rect &rect, const Color &color);

// it took a long time. it is drawn with some artifacts, but you can make out the text. enough for this task.
void drawText(unsigned char *img, int img_w, int img_h, const char *text, int x, int y, const Color &color);
void drawText(unsigned char *img, int img_w, int img_h, const char *text, const Rect &rect, const Color &color);

struct ObjectDetection
{
    Rect rect;
    int label;
    float prob;
};

void detectYoloV3(unsigned char *img, int img_w, int img_h, std::vector<ObjectDetection> &objects);
void drawObjectDetections(unsigned char *img, int img_w, int img_h, const std::vector<ObjectDetection> &objects);
