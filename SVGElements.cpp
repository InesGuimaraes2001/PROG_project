#include "SVGElements.hpp"
#include <sstream>
#include <cmath>

namespace svg
{
    SVGElement::SVGElement() {}
    SVGElement::~SVGElement() {}

    void SVGElement::setTransformations(const std::vector<std::string> &trans)
    {
        transformations = trans;
    }

    // Ellipse
    Ellipse::Ellipse(const Color &fill, const Point &center, const Point &radius)
        : fill(fill), center(center), radius(radius)
    {
    }

    void Ellipse::draw(PNGImage &img) const
    {
        img.draw_ellipse(center, radius, fill);
    }

    // Circle
    Circle::Circle(const Color &fill, const Point &center, int radius)
        : Ellipse(fill, center, Point{radius, radius})
    {
    }

    // Polyline
    Polyline::Polyline(const Color &stroke, const std::vector<Point> &points)
        : stroke(stroke), points(points)
    {
    }

    void Polyline::draw(PNGImage &img) const
    {
        for (size_t i = 0; i < points.size() - 1; i++)
        {
            img.draw_line(points[i], points[i + 1], stroke);
        }
    }

    // Line
    Line::Line(const Color &stroke, const Point &start, const Point &end)
        : stroke(stroke), start(start), end(end)
    {
    }

    void Line::draw(PNGImage &img) const
    {
        img.draw_line(start, end, stroke);
    }

    // Polygon
    Polygon::Polygon(const Color &fill, const std::vector<Point> &points)
        : fill(fill), points(points)
    {
    }

    void Polygon::draw(PNGImage &img) const
    {
        img.draw_polygon(points, fill);
    }

    // Rect
    Rect::Rect(const Color &fill, const Point &topLeft, int width, int height)
        : Polygon(fill, {topLeft, topLeft.translate({width, 0}), topLeft.translate({width, height}), topLeft.translate({0, height})})
    {
    }

    // Group
    Group::Group(const std::vector<SVGElement *> &elements, const std::vector<std::string> &transformations)
        : elements(elements)
    {
        this->transformations = transformations;
    }

    void Group::draw(PNGImage &img) const
    {
        for (const SVGElement *element : elements)
        {
            element->draw(img);
        }
    }

    // Use
    Use::Use(const SVGElement *ref, const std::vector<std::string> &transformations)
        : ref(ref)
    {
        this->transformations = transformations;
    }

    void Use::draw(PNGImage &img) const
    {
        ref->draw(img);
    }
}
