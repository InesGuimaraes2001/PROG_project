#include "SVGElements.hpp"

namespace svg {

Ellipse::Ellipse(const Color &fill, const Point &center, const Point &radius)
    : fill(fill), center(center), radius(radius) {}

void Ellipse::draw(PNGImage &img) const {
    img.draw_ellipse(center, radius, fill);
}

std::unique_ptr<SVGElement> Ellipse::clone() const {
    return std::unique_ptr<SVGElement>(new Ellipse(*this));
}

void Ellipse::translate(const Point& t) {
    center = center.translate(t);
}

void Ellipse::rotate(const Point& origin, int degrees) {
    center = center.rotate(origin, degrees);
}

void Ellipse::scale(const Point& origin, int factor) {
    center = center.scale(origin, factor);
    radius = radius.scale(Point{0, 0}, factor); // Scale radius
}

Circle::Circle(const Color &fill, const Point &center, int radius)
    : Ellipse(fill, center, Point{radius, radius}) {}

void Circle::draw(PNGImage &img) const {
    // Ensure the circle is within the image boundaries
    if (getCenter().x - getRadius().x >= 0 && getCenter().x + getRadius().x < img.width() &&
        getCenter().y - getRadius().y >= 0 && getCenter().y + getRadius().y < img.height()) {
        Ellipse::draw(img);
    }
}

std::unique_ptr<SVGElement> Circle::clone() const {
    return std::unique_ptr<SVGElement>(new Circle(*this));
}

Rectangle::Rectangle(const Color &fill, const Point &topLeft, int width, int height)
    : fill(fill), topLeft(topLeft), width(width), height(height) {}

void Rectangle::draw(PNGImage &img) const {
    // Ensure the rectangle is within the image boundaries
    if (topLeft.x >= 0 && topLeft.x + width < img.width() &&
        topLeft.y >= 0 && topLeft.y + height < img.height()) {
        for (int y = topLeft.y; y < topLeft.y + height; ++y) {
            for (int x = topLeft.x; x < topLeft.x + width; ++x) {
                img.at(x, y) = fill;
            }
        }
    }
}

std::unique_ptr<SVGElement> Rectangle::clone() const {
    return std::unique_ptr<SVGElement>(new Rectangle(*this));
}

void Rectangle::translate(const Point& t) {
    topLeft = topLeft.translate(t);
}

void Rectangle::rotate(const Point& origin, int degrees) {
    topLeft = topLeft.rotate(origin, degrees);
    // Rotating a rectangle requires more complex handling if width and height need to be rotated as well
}

void Rectangle::scale(const Point& origin, int factor) {
    topLeft = topLeft.scale(origin, factor);
    width *= factor;
    height *= factor;
}

Line::Line(const Color &stroke, const Point &start, const Point &end)
    : stroke(stroke), start(start), end(end) {}

void Line::draw(PNGImage &img) const {
    // Ensure the line is within the image boundaries
    if (start.x >= 0 && start.x < img.width() && end.x >= 0 && end.x < img.width() &&
        start.y >= 0 && start.y < img.height() && end.y >= 0 && end.y < img.height()) {
        img.draw_line(start, end, stroke);
    }
}

std::unique_ptr<SVGElement> Line::clone() const {
    return std::unique_ptr<SVGElement>(new Line(*this));
}

void Line::translate(const Point& t) {
    start = start.translate(t);
    end = end.translate(t);
}

void Line::rotate(const Point& origin, int degrees) {
    start = start.rotate(origin, degrees);
    end = end.rotate(origin, degrees);
}

void Line::scale(const Point& origin, int factor) {
    start = start.scale(origin, factor);
    end = end.scale(origin, factor);
}

Polygon::Polygon(const Color &fill, const std::vector<Point> &points)
    : fill(fill), points(points) {}

void Polygon::draw(PNGImage &img) const {
    // Ensure the polygon is within the image boundaries
    for (const auto& point : points) {
        if (point.x < 0 || point.x >= img.width() || point.y < 0 || point.y >= img.height()) {
            return;
        }
    }
    img.draw_polygon(points, fill);
}

std::unique_ptr<SVGElement> Polygon::clone() const {
    return std::unique_ptr<SVGElement>(new Polygon(*this));
}

void Polygon::translate(const Point& t) {
    for (auto& point : points) {
        point = point.translate(t);
    }
}

void Polygon::rotate(const Point& origin, int degrees) {
    for (auto& point : points) {
        point = point.rotate(origin, degrees);
    }
}

void Polygon::scale(const Point& origin, int factor) {
    for (auto& point : points) {
        point = point.scale(origin, factor);
    }
}

Polyline::Polyline(const Color &stroke, const std::vector<Point> &points)
    : stroke(stroke), points(points) {}

void Polyline::draw(PNGImage &img) const {
    // Ensure the polyline is within the image boundaries
    for (const auto& point : points) {
        if (point.x < 0 || point.x >= img.width() || point.y < 0 || point.y >= img.height()) {
            return;
        }
    }
    for (size_t i = 1; i < points.size(); ++i) {
        img.draw_line(points[i - 1], points[i], stroke);
    }
}

std::unique_ptr<SVGElement> Polyline::clone() const {
    return std::unique_ptr<SVGElement>(new Polyline(*this));
}

void Polyline::translate(const Point& t) {
    for (auto& point : points) {
        point = point.translate(t);
    }
}

void Polyline::rotate(const Point& origin, int degrees) {
    for (auto& point : points) {
        point = point.rotate(origin, degrees);
    }
}

void Polyline::scale(const Point& origin, int factor) {
    for (auto& point : points) {
        point = point.scale(origin, factor);
    }
}

Group::Group(const std::vector<SVGElement *> &elements) {
    for (auto element : elements) {
        this->elements.push_back(std::unique_ptr<SVGElement>(element));
    }
}

Group::Group(const Group& other) {
    for (const auto& element : other.elements) {
        elements.push_back(element->clone());
    }
}

Group& Group::operator=(const Group& other) {
    if (this != &other) {
        elements.clear();
        for (const auto& element : other.elements) {
            elements.push_back(element->clone());
        }
    }
    return *this;
}

Group::~Group() {}

void Group::draw(PNGImage &img) const {
    for (const auto& element : elements) {
        element->draw(img);
    }
}

std::unique_ptr<SVGElement> Group::clone() const {
    return std::unique_ptr<SVGElement>(new Group(*this));
}

void Group::addElement(SVGElement *element) {
    elements.push_back(std::unique_ptr<SVGElement>(element));
}

void Group::translate(const Point& t) {
    for (auto& element : elements) {
        element->translate(t);
    }
}

void Group::rotate(const Point& origin, int degrees) {
    for (auto& element : elements) {
        element->rotate(origin, degrees);
    }
}

void Group::scale(const Point& origin, int factor) {
    for (auto& element : elements) {
        element->scale(origin, factor);
    }
}

Use::Use(const SVGElement *element, const Point &transform)
    : element(element->clone()), transform(transform) {}

Use::Use(const Use& other)
    : element(other.element->clone()), transform(other.transform) {}

Use& Use::operator=(const Use& other) {
    if (this != &other) {
        element = other.element->clone();
        transform = other.transform;
    }
    return *this;
}

Use::~Use() {}

void Use::draw(PNGImage &img, const Point &offset) const {
    Point new_transform = transform.translate(offset);
    //element->draw(img, new_transform); <- error
}

//std::unique_ptr<SVGElement> Use::clone() const {
   //return std::unique_ptr<SVGElement>(new Use(*this)); <- error
//}

void Use::translate(const Point& t) {
    transform = transform.translate(t);
}

void Use::rotate(const Point& origin, int degrees) {
    transform = transform.rotate(origin, degrees);
}

void Use::scale(const Point& origin, int factor) {
    transform = transform.scale(origin, factor);
}

} // namespace svg
