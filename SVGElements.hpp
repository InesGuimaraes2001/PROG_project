#ifndef _svg_SVGElements_hpp_
#define _svg_SVGElements_hpp_

#include "Color.hpp"
#include "Point.hpp"
#include "PNGImage.hpp"

#include <vector>

namespace svg {

class SVGElement {
public:
    SVGElement() {}
    virtual ~SVGElement() {}
    virtual void draw(PNGImage &img) const = 0;
};

class Ellipse : public SVGElement {
public:
    Ellipse(const Color &fill, const Point &center, const Point &radius);
    void draw(PNGImage &img) const override;

private:
    Color fill;
    Point center;
    Point radius;
};

class Circle : public Ellipse {
public:
    Circle(const Color &fill, const Point &center, int radius);
    void draw(PNGImage &img) const override;
};

class Rectangle : public SVGElement {
public:
    Rectangle(const Color &fill, const Point &topLeft, int width, int height);
    void draw(PNGImage &img) const override;

private:
    Color fill;
    Point topLeft;
    int width;
    int height;
};

class Line : public SVGElement {
public:
    Line(const Color &stroke, const Point &start, const Point &end);
    void draw(PNGImage &img) const override;

private:
    Color stroke;
    Point start;
    Point end;
};

class Polygon : public SVGElement {
public:
    Polygon(const Color &fill, const std::vector<Point> &points);
    void draw(PNGImage &img) const override;

private:
    Color fill;
    std::vector<Point> points;
};

void readSVG(const std::string &svg_file, Point &dimensions, std::vector<SVGElement *> &svg_elements);
void convert(const std::string &svg_file, const std::string &png_file);

} // namespace svg

#endif
