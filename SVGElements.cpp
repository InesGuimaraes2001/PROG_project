#include "SVGElements.hpp"
#include <sstream>
#include <cmath>
#include <iostream>
#include <algorithm>

namespace svg
{
    // Custom clamp function implementation
    template <typename T>
    T clamp(const T& value, const T& low, const T& high)
    {
        return std::max(low, std::min(value, high));
    }

    SVGElement::SVGElement() {}
    SVGElement::~SVGElement() {}

    void SVGElement::setTransformations(const std::vector<std::string> &trans)
    {
        transformations = trans;
    }

    void SVGElement::applyTransformations(Point &point) const
    {
        std::cout << "Applying transformations to point: (" << point.x << ", " << point.y << ")\n";
        for (const auto &trans : transformations)
        {
            std::istringstream ss(trans);
            std::string type;
            ss >> type;
            if (type == "translate")
            {
                float dx, dy;
                ss >> dx >> dy;
                std::cout << "Applying translate: dx=" << dx << ", dy=" << dy << "\n";
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
                std::cout << "Applying rotate: angle=" << angle << ", cx=" << cx << ", cy=" << cy << "\n";
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
                std::cout << "Applying scale: sx=" << sx << ", sy=" << sy << "\n";
                point = point.scale(Point{0, 0}, static_cast<int>(sx));
            }
        }
        std::cout << "Transformed point: (" << point.x << ", " << point.y << ")\n";
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
        std::cout << "Drawing ellipse at (" << transformed_center.x << ", " << transformed_center.y << ")\n";
        if (transformed_center.x >= 0 && transformed_center.x < img.width() &&
            transformed_center.y >= 0 && transformed_center.y < img.height())
        {
            img.draw_ellipse(transformed_center, radius, fill);
        }
        else
        {
            std::cerr << "Error: Transformed ellipse center is out of bounds!\n";
        }
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
            std::cout << "Drawing line from (" << start.x << ", " << start.y << ") to (" << end.x << ", " << end.y << ")\n";
            start.x = clamp(start.x, 0, img.width() - 1);
            start.y = clamp(start.y, 0, img.height() - 1);
            end.x = clamp(end.x, 0, img.width() - 1);
            end.y = clamp(end.y, 0, img.height() - 1);
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
        std::cout << "Drawing line from (" << transformed_start.x << ", " << transformed_start.y << ") to (" << transformed_end.x << ", " << transformed_end.y << ")\n";
        transformed_start.x = clamp(transformed_start.x, 0, img.width() - 1);
        transformed_start.y = clamp(transformed_start.y, 0, img.height() - 1);
        transformed_end.x = clamp(transformed_end.x, 0, img.width() - 1);
        transformed_end.y = clamp(transformed_end.y, 0, img.height() - 1);
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
        bool out_of_bounds = false;
        for (auto &point : transformed_points)
        {
            applyTransformations(point);
            if (point.x < 0 || point.x >= img.width() || point.y < 0 || point.y >= img.height())
            {
                std::cerr << "Error: Transformed polygon point (" << point.x << ", " << point.y << ") is out of bounds!\n";
                out_of_bounds = true;
            }
            point.x = clamp(point.x, 0, img.width() - 1);
            point.y = clamp(point.y, 0, img.height() - 1);
        }
        if (!out_of_bounds)
        {
            img.draw_polygon(transformed_points, fill);
        }
    }

    // Rect
    Rect::Rect(const Color &fill, const Point &topLeft, int width, int height)
        : Polygon(fill, {topLeft, topLeft.translate({width, 0}), topLeft.translate({width, height}), topLeft.translate({0, height})})
    {
    }

    void Rect::draw(PNGImage &img) const
    {
        Polygon::draw(img); // Call Polygon's draw method directly
    }

    // Group
    Group::Group(const std::vector<SVGElement *> &elements, const std::vector<std::string> &transformations)
        : elements(elements)
    {
        this->transformations = transformations;
    }

    Group::~Group()
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
