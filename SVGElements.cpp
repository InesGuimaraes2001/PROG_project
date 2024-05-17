#include "SVGElements.hpp"
#include <sstream>
#include <cmath>
#include <iostream>
#include <memory>

namespace svg
{
    SVGElement::SVGElement() {}
    SVGElement::~SVGElement() {}

    void SVGElement::setTransformations(const std::vector<std::string> &trans)
    {
        transformations = trans;
    }

    void SVGElement::applyTransformations(Point &point) const
    {
        //std::cout << "Applying transformations to point: (" << point.x << ", " << point.y << ")\n";
        for (const auto &trans : transformations)
        {
            std::istringstream ss(trans);
            std::string type;
            ss >> type;
            if (type == "translate")
            {
                float dx, dy;
                ss >> dx >> dy;
                //std::cout << "Applying translate: dx=" << dx << ", dy=" << dy << "\n";
                point = point.translate(Point{static_cast<int>(dx), static_cast<int>(dy)});
            }
            else if (type == "rotate")
            {
                float angle, cx = 0, cy = 0;
                ss >> angle;
                if (ss.peek() == ',')
                {
                    ss.ignore();
                    ss >> cx >> cy;
                }
                //std::cout << "Applying rotate: angle=" << angle << ", cx=" << cx << ", cy=" << cy << "\n";
                point = point.rotate(Point{static_cast<int>(cx), static_cast<int>(cy)}, static_cast<int>(angle));
            }
            else if (type == "scale")
            {
                float sx, sy = 1;
                ss >> sx;
                if (ss.peek() == ',')
                {
                    ss.ignore();
                    ss >> sy;
                }
                //std::cout << "Applying scale: sx=" << sx << ", sy=" << sy << "\n";
                point = point.scale(Point{0, 0}, static_cast<int>(sx));
            }
        }
        //std::cout << "Transformed point: (" << point.x << ", " << point.y << ")\n";
    }

    // Ellipse
    Ellipse::Ellipse(const Color &fill, const Point &center, const Point &radius)
        : fill(fill), center(center), radius(radius)
    {
    }

    void Ellipse::draw(PNGImage &img) const
    {
        Point transformed_center = center;
        applyTransformations(transformed_center);
        img.draw_ellipse(transformed_center, radius, fill);
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
            Point start = points[i];
            Point end = points[i + 1];
            applyTransformations(start);
            applyTransformations(end);
            img.draw_line(start, end, stroke);
        }
    }

    // Line
    Line::Line(const Color &stroke, const Point &start, const Point &end)
        : stroke(stroke), start(start), end(end)
    {
    }

    void Line::draw(PNGImage &img) const
    {
        Point transformed_start = start;
        Point transformed_end = end;
        applyTransformations(transformed_start);
        applyTransformations(transformed_end);
        img.draw_line(transformed_start, transformed_end, stroke);
    }

    // Polygon
    Polygon::Polygon(const Color &fill, const std::vector<Point> &points)
        : fill(fill), points(points)
    {
    }

    void Polygon::draw(PNGImage &img) const
    {
        std::vector<Point> transformed_points = points;
        for (auto &point : transformed_points)
        {
            applyTransformations(point);
        }
        img.draw_polygon(transformed_points, fill);
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

    Group::~Group() // Add destructor to delete elements
    {
        for (auto element : elements)
        {
            delete element;
        }
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
