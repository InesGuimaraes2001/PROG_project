#ifndef __svg_SVGElements_hpp__
#define __svg_SVGElements_hpp__

#include "Color.hpp"
#include "Point.hpp"
#include "PNGImage.hpp"
#include <vector>
#include <string>
#include <map>
#include <memory>

namespace svg
{
    // Custom clamp function declaration
    template <typename T>
    T clamp(const T& value, const T& low, const T& high);

    class SVGElement
    {
    public:
        SVGElement();
        virtual ~SVGElement();
        virtual void draw(PNGImage &img) const = 0;
        void setTransformations(const std::vector<std::string> &trans);
        void applyTransformations(Point &point) const;

    protected:
        std::vector<std::string> transformations;
    };

    void convert(const std::string &svg_file, const std::string &png_file);
    void readSVG(const std::string &svg_file, Point &dimensions, std::vector<SVGElement *> &svg_elements);

    class Ellipse : public SVGElement
    {
    public:
        Ellipse(const Color &fill, const Point &center, const Point &radius);
        void draw(PNGImage &img) const override;

    private:
        Color fill;
        Point center;
        Point radius;
    };

    class Circle : public Ellipse
    {
    public:
        Circle(const Color &fill, const Point &center, int radius);
    };

    class Polyline : public SVGElement
    {
    public:
        Polyline(const Color &stroke, const std::vector<Point> &points);
        void draw(PNGImage &img) const override;

    private:
        Color stroke;
        std::vector<Point> points;
    };

    class Line : public SVGElement
    {
    public:
        Line(const Color &stroke, const Point &start, const Point &end);
        void draw(PNGImage &img) const override;

    private:
        Color stroke;
        Point start;
        Point end;
    };

    class Polygon : public SVGElement
    {
    public:
        Polygon(const Color &fill, const std::vector<Point> &points);
        void draw(PNGImage &img) const override;

    protected:
        Color fill;
        std::vector<Point> points;
    };

    class Rect : public Polygon
    {
    public:
        Rect(const Color &fill, const Point &topLeft, int width, int height);
        void draw(PNGImage &img) const override;
    };

    class Group : public SVGElement
    {
    public:
        Group(const std::vector<SVGElement *> &elements, const std::vector<std::string> &transformations);
        void draw(PNGImage &img) const override;
        ~Group(); // Add destructor to delete elements

    private:
        std::vector<SVGElement *> elements;
    };

    class Use : public SVGElement
    {
    public:
        Use(const SVGElement *ref, const std::vector<std::string> &transformations);
        void draw(PNGImage &img) const override;

    private:
        const SVGElement *ref;
    };
}

#endif
