#ifndef _svg_SVGElements_hpp_
#define _svg_SVGElements_hpp_

#include "Color.hpp"
#include "Point.hpp"
#include "PNGImage.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace svg {

class SVGElement {
public:
    SVGElement() {}
    virtual ~SVGElement() {}
    virtual void draw(PNGImage &img) const = 0;
    virtual std::unique_ptr<SVGElement> clone() const = 0;
    virtual void translate(const Point& t) = 0;
    virtual void rotate(const Point& origin, int degrees) = 0;
    virtual void scale(const Point& origin, int factor) = 0;
};

class Ellipse : public SVGElement {
public:
    Ellipse(const Color &fill, const Point &center, const Point &radius);
    void draw(PNGImage &img) const override;
    std::unique_ptr<SVGElement> clone() const override;
    void translate(const Point& t) override;
    void rotate(const Point& origin, int degrees) override;
    void scale(const Point& origin, int factor) override;

    // Add getter methods
    Point getCenter() const { return center; }
    Point getRadius() const { return radius; }

protected:
    Color fill;
    Point center;
    Point radius;
};

class Circle : public Ellipse {
public:
    Circle(const Color &fill, const Point &center, int radius);
    void draw(PNGImage &img) const override;
    std::unique_ptr<SVGElement> clone() const override;
};

class Rectangle : public SVGElement {
public:
    Rectangle(const Color &fill, const Point &topLeft, int width, int height);
    void draw(PNGImage &img) const override;
    std::unique_ptr<SVGElement> clone() const override;
    void translate(const Point& t) override;
    void rotate(const Point& origin, int degrees) override;
    void scale(const Point& origin, int factor) override;

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
    std::unique_ptr<SVGElement> clone() const override;
    void translate(const Point& t) override;
    void rotate(const Point& origin, int degrees) override;
    void scale(const Point& origin, int factor) override;

private:
    Color stroke;
    Point start;
    Point end;
};

class Polygon : public SVGElement {
public:
    Polygon(const Color &fill, const std::vector<Point> &points);
    void draw(PNGImage &img) const override;
    std::unique_ptr<SVGElement> clone() const override;
    void translate(const Point& t) override;
    void rotate(const Point& origin, int degrees) override;
    void scale(const Point& origin, int factor) override;

private:
    Color fill;
    std::vector<Point> points;
};

class Polyline : public SVGElement {
public:
    Polyline(const Color &stroke, const std::vector<Point> &points);
    void draw(PNGImage &img) const override;
    std::unique_ptr<SVGElement> clone() const override;
    void translate(const Point& t) override;
    void rotate(const Point& origin, int degrees) override;
    void scale(const Point& origin, int factor) override;

private:
    Color stroke;
    std::vector<Point> points;
};

class Group : public SVGElement {
public:
    Group(const std::vector<SVGElement *> &elements);
    Group(const Group& other);
    Group& operator=(const Group& other);
    ~Group();
    void draw(PNGImage &img) const override;
    std::unique_ptr<SVGElement> clone() const override;
    void addElement(SVGElement *element);
    void translate(const Point& t) override;
    void rotate(const Point& origin, int degrees) override;
    void scale(const Point& origin, int factor) override;

private:
    std::vector<std::unique_ptr<SVGElement>> elements;
};

class Use : public SVGElement {
public:
    Use(const SVGElement *element, const Point &transform);
    Use(const Use& other);
    Use& operator=(const Use& other);
    ~Use();
    void draw(PNGImage &img, const Point &offset) const;
    //std::unique_ptr<SVGElement> clone() const override; -> commented, therefore, undefined
    void translate(const Point& t) override;
    void rotate(const Point& origin, int degrees) override;
    void scale(const Point& origin, int factor) override;

private:
    std::unique_ptr<SVGElement> element;
    Point transform;
};

void readSVG(const std::string &svg_file, Point &dimensions, std::vector<SVGElement *> &svg_elements);
void convert(const std::string &svg_file, const std::string &png_file);

} // namespace svg

#endif
