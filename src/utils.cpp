#include <utils.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <net.h>

#include <algorithm>
#include <cassert>

static FT_Library ft;
static FT_Face face;
static FT_GlyphSlot slot;

static ncnn::Net yolov3;

bool intersects(const Rect &r1, const Rect &r2)
{
    return !(r2.x > r1.x + r1.w - 1 ||
             r2.x + r2.w - 1 < r1.x ||
             r2.y > r1.y + r1.h - 1 ||
             r2.y + r2.h < r1.y);
}

Rect intersect(const Rect &r1, const Rect &r2)
{
    if (!intersects(r1, r2))
        return Rect();

    Rect r;
    r.x = std::max(r1.x, r2.x);
    r.y = std::max(r1.y, r2.y);
    r.w = std::min(r1.x + r1.w, r2.x + r2.w) - r.x;
    r.h = std::min(r1.y + r1.h, r2.y + r2.h) - r.y;
    assert(r.w >= 0 && r.h >= 0);
    return r;
}

void initUtils()
{
    FT_Error err = FT_Init_FreeType(&ft);
    if (err)
    {
        printf("Not loaded FT module");
        exit(-1);
    }

    err = FT_New_Face(ft, FONT_PATH, 0, &face);
    if (err)
    {
        printf("Not loaded %s font file", FONT_PATH);
        exit(-1);
    }

    err = FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);
    if (err)
    {
        printf("Not setted %d font size", FONT_SIZE);
        exit(-1);
    }

    slot = face->glyph;

    yolov3.opt.use_vulkan_compute = false;

    // original pretrained model from https://github.com/eric612/MobileNet-YOLO
    // param : https://drive.google.com/open?id=1V9oKHP6G6XvXZqhZbzNKL6FI_clRWdC-
    // bin : https://drive.google.com/open?id=1DBcuFCr-856z3FRQznWL_S5h-Aj3RawA
    // the ncnn model https://github.com/nihui/ncnn-assets/tree/master/models
    if (yolov3.load_param(YOLOV3_PARAM_FILE))
    {
        printf("Not loaded %s", YOLOV3_PARAM_FILE);
        exit(-1);
    }
    if (yolov3.load_model(YOLOV3_BIN_FILE))
    {
        printf("Not loaded %s", YOLOV3_BIN_FILE);
        exit(-1);
    }
}

void drawOutlineRect(unsigned char *img, int img_w, int img_h, const Rect &rect, const Color &color)
{
    if (!intersects(Rect({ 0, 0, img_w, img_h}), rect))
        return;

    drawRect(img, img_w, img_h, Rect({ rect.x, rect.y, rect.w, 1 }), color);
    drawRect(img, img_w, img_h, Rect({ rect.x, rect.y + rect.h - 1, rect.w, 1 }), color);
    drawRect(img, img_w, img_h, Rect({ rect.x, rect.y, 1, rect.h }), color);
    drawRect(img, img_w, img_h, Rect({ rect.x + rect.w - 1, rect.y, 1, rect.h }), color);
}

void drawRect(unsigned char *img, int img_w, int img_h, const Rect &rect, const Color &color)
{
    Rect r = intersect(Rect({ 0, 0, img_w, img_h}), rect);
    for (int j = r.y; j < r.y + r.h; ++j)
    {
        for (int i = r.x; i < r.x + r.w; ++i)
        {
            int offset = (j * img_w + i) * 3;  // 3 байта на пиксель
            img[offset] = color.r;
            img[offset + 1] = color.g;
            img[offset + 2] = color.b;
        }
    }
}

void drawText(unsigned char *img, int img_w, int img_h, const char *text, int x, int y, const Color &color)
{
    drawText(img, img_w, img_h, text, Rect({x, y, img_w, img_h}), color);
}

void drawText(unsigned char *img, int img_w, int img_h, const char *text, const Rect &rect, const Color &color)
{
    int x = rect.x;
    int y = rect.y;
    int glyph_vert_advance = int(slot->linearVertAdvance >> 16);

    for (int i = 0; text[i] != 0; ++i)
    {
        if (text[i] == '\n')
        {
            x = rect.x;
            y += glyph_vert_advance;
            continue;
        }

        FT_Error err = FT_Load_Char(face, text[i], FT_LOAD_RENDER);
        if (err)
        {
            assert(false);
            continue;
        }

        int glyph_w = slot->bitmap.width;
        int glyph_h = slot->bitmap.rows;
        int glyph_t = slot->bitmap_top;
        int glyph_l = slot->bitmap_left;

        Rect glyph_area({x + glyph_l, y + glyph_vert_advance - glyph_t, glyph_w, glyph_h});
        Rect drawable_area = intersect(intersect(Rect({0, 0, img_w, img_h}), rect),
                                       glyph_area);
        if (drawable_area.w == 0 || drawable_area.h == 0)
        {
            x += slot->advance.x >> 6;
            continue;
        }

        for (int img_y = drawable_area.y; img_y < drawable_area.y + drawable_area.h; ++img_y)
        {
            for (int img_x = drawable_area.x; img_x < drawable_area.x + drawable_area.w; ++img_x)
            {
                int glyph_offset_x = img_x - drawable_area.x;
                int glyph_offset_y = img_y - drawable_area.y;
                unsigned char *pixel = slot->bitmap.buffer + (glyph_offset_y * glyph_w + glyph_offset_x);

                int offset = (img_y * img_w + img_x) * 3;  // 3 байта на пиксель
                img[offset] = (pixel[0] == 0) ? img[offset] :
                              (pixel[0] == 255) ? color.r :
                               std::min(color.r, (unsigned char)(img[offset] + (pixel[0] / 255.0) * color.r));
                img[offset + 1] = (pixel[1] == 0) ? img[offset + 1] :
                                  (pixel[1] == 255) ? color.g :
                                   std::min(color.g, (unsigned char)(img[offset + 1] + (pixel[1] / 255.0) * color.g));
                img[offset + 2] = (pixel[2] == 0) ? img[offset + 2] :
                                  (pixel[2] == 255) ? color.b :
                                   std::min(color.b, (unsigned char)(img[offset + 2] + (pixel[2] / 255.0) * color.b));
            }
        }

        x += slot->advance.x >> 6;
    }
}

// NCNN_PROJECT_DIR/examples/yolov3.cpp
void detectYoloV3(unsigned char *img, int img_w, int img_h, std::vector<ObjectDetection>& objects)
{
    const int target_size = 352;

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(img, ncnn::Mat::PIXEL_RGB, img_w, img_h, target_size, target_size);

    const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
    const float norm_vals[3] = {0.007843f, 0.007843f, 0.007843f};
    in.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = yolov3.create_extractor();

    ex.input("data", in);

    ncnn::Mat out;
    ex.extract("detection_out", out);

    //     printf("%d %d %d\n", out.w, out.h, out.c);
    objects.clear();
    for (int i = 0; i < out.h; i++)
    {
        const float* values = out.row(i);

        ObjectDetection object;
        object.label = values[0];
        object.prob = values[1];
        object.rect.x = values[2] * img_w;
        object.rect.y = values[3] * img_h;
        object.rect.w = values[4] * img_w - object.rect.x;
        object.rect.h = values[5] * img_h - object.rect.y;

        objects.push_back(object);
    }
}

void drawObjectDetections(unsigned char *img, int img_w, int img_h, const std::vector<ObjectDetection> &objects)
{
    static const char* class_names[] = {"background",
                                        "aeroplane", "bicycle", "bird", "boat",
                                        "bottle", "bus", "car", "cat", "chair",
                                        "cow", "diningtable", "dog", "horse",
                                        "motorbike", "person", "pottedplant",
                                        "sheep", "sofa", "train", "tvmonitor"
                                       };

    for (const ObjectDetection &o : objects)
    {
        drawOutlineRect(img, img_w, img_h, o.rect, Color({0, 64, 0}));
        drawRect(img, img_w, img_h, Rect({o.rect.x, o.rect.y, o.rect.w, 24}), Color({0, 64, 0}));

        std::string label = std::string(class_names[o.label]) + ' ' + std::to_string(o.prob);
        drawText(img, img_w, img_h, label.c_str(), Rect({o.rect.x, o.rect.y, o.rect.w, 24}), Color({192, 192, 192}));
    }
}
