#include "SVGElements.hpp"

namespace svg {

Ellipse::Ellipse(const Color &fill, const Point &center, const Point &radius)
    : fill(fill), center(center), radius(radius) {}

void Ellipse::draw(PNGImage &img) const {
    img.draw_ellipse(center, radius, fill);
}

Circle::Circle(const Color &fill, const Point &center, int radius)
    : Ellipse(fill, center, Point{radius, radius}) {}

void Circle::draw(PNGImage &img) const {
    Ellipse::draw(img);
}

Rectangle::Rectangle(const Color &fill, const Point &topLeft, int width, int height)
    : fill(fill), topLeft(topLeft), width(width), height(height) {}

void Rectangle::draw(PNGImage &img) const {
    // Draw the top side of the rectangle
    img.draw_line(Point{topLeft.x, topLeft.y}, Point{topLeft.x + width, topLeft.y}, fill);

    // Draw the bottom side of the rectangle
    img.draw_line(Point{topLeft.x, topLeft.y + height}, Point{topLeft.x + width, topLeft.y + height}, fill);

    // Draw the left side of the rectangle
    img.draw_line(Point{topLeft.x, topLeft.y}, Point{topLeft.x, topLeft.y + height}, fill);

    // Draw the right side of the rectangle
    img.draw_line(Point{topLeft.x + width, topLeft.y}, Point{topLeft.x + width, topLeft.y + height}, fill);
}


Line::Line(const Color &stroke, const Point &start, const Point &end)
    : stroke(stroke), start(start), end(end) {}

void Line::draw(PNGImage &img) const {
    img.draw_line(start, end, stroke);
}

Polygon::Polygon(const Color &fill, const std::vector<Point> &points)
    : fill(fill), points(points) {}

void Polygon::draw(PNGImage &img) const {
    img.draw_polygon(points, fill);
}

} // namespace svg
