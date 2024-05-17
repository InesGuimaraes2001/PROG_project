#include <iostream>
#include <sstream>
#include "SVGElements.hpp"
#include "external/tinyxml2/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

namespace svg
{
    void applyTransformations(XMLElement *elem, SVGElement *svgElement)
    {
        const char *transform = elem->Attribute("transform");
        if (transform)
        {
            std::vector<std::string> transformations;
            std::stringstream ss(transform);
            std::string trans;
            while (std::getline(ss, trans, ' '))
            {
                transformations.push_back(trans);
            }
            svgElement->setTransformations(transformations);
        }
    }

    SVGElement* parseElement(XMLElement* child, std::map<std::string, SVGElement *> &id_map)
    {
        const string elem_type = child->Name();
        SVGElement *element = nullptr;

        if (elem_type == "circle")
        {
            Color fill = parse_color(child->Attribute("fill"));
            Point center{child->IntAttribute("cx"), child->IntAttribute("cy")};
            int radius = child->IntAttribute("r");
            element = new Circle(fill, center, radius);
        }
        else if (elem_type == "ellipse")
        {
            Color fill = parse_color(child->Attribute("fill"));
            Point center{child->IntAttribute("cx"), child->IntAttribute("cy")};
            Point radius{child->IntAttribute("rx"), child->IntAttribute("ry")};
            element = new Ellipse(fill, center, radius);
        }
        else if (elem_type == "rect")
        {
            Color fill = parse_color(child->Attribute("fill"));
            Point topLeft{child->IntAttribute("x"), child->IntAttribute("y")};
            int width = child->IntAttribute("width");
            int height = child->IntAttribute("height");
            element = new Rect(fill, topLeft, width, height);
        }
        else if (elem_type == "line")
        {
            Color stroke = parse_color(child->Attribute("stroke"));
            Point start{child->IntAttribute("x1"), child->IntAttribute("y1")};
            Point end{child->IntAttribute("x2"), child->IntAttribute("y2")};
            element = new Line(stroke, start, end);
        }
        else if (elem_type == "polyline")
        {
            Color stroke = parse_color(child->Attribute("stroke"));
            std::vector<Point> points;
            std::stringstream ss(child->Attribute("points"));
            std::string point;
            while (std::getline(ss, point, ' '))
            {
                size_t comma = point.find(',');
                int x = std::stoi(point.substr(0, comma));
                int y = std::stoi(point.substr(comma + 1));
                points.push_back({x, y});
            }
            element = new Polyline(stroke, points);
        }
        else if (elem_type == "polygon")
        {
            Color fill = parse_color(child->Attribute("fill"));
            std::vector<Point> points;
            std::stringstream ss(child->Attribute("points"));
            std::string point;
            while (std::getline(ss, point, ' '))
            {
                size_t comma = point.find(',');
                int x = std::stoi(point.substr(0, comma));
                int y = std::stoi(point.substr(comma + 1));
                points.push_back({x, y});
            }
            element = new Polygon(fill, points);
        }
        else if (elem_type == "g")
        {
            std::vector<SVGElement *> group_elements;
            for (XMLElement *g_child = child->FirstChildElement(); g_child != nullptr; g_child = g_child->NextSiblingElement())
            {
                SVGElement *g_element = parseElement(g_child, id_map);
                if (g_element)
                {
                    group_elements.push_back(g_element);
                }
            }
            element = new Group(group_elements, {});
        }
        else if (elem_type == "use")
        {
            const char *href = child->Attribute("href");
            if (href && href[0] == '#')
            {
                std::string id(href + 1);
                auto it = id_map.find(id);
                if (it != id_map.end())
                {
                    const SVGElement *ref = it->second;
                    std::vector<std::string> transformations;
                    const char *transform = child->Attribute("transform");
                    if (transform)
                    {
                        std::stringstream ss(transform);
                        std::string trans;
                        while (std::getline(ss, trans, ' '))
                        {
                            transformations.push_back(trans);
                        }
                    }
                    element = new Use(ref, transformations);
                }
            }
        }

        if (element)
        {
            const char *id = child->Attribute("id");
            if (id)
            {
                id_map[id] = element;
            }
            applyTransformations(child, element);
        }
        return element;
    }

    void readSVG(const string &svg_file, Point &dimensions, vector<SVGElement *> &svg_elements)
    {
        XMLDocument doc;
        XMLError r = doc.LoadFile(svg_file.c_str());
        if (r != XML_SUCCESS)
        {
            throw runtime_error("Unable to load " + svg_file);
        }
        XMLElement *xml_elem = doc.RootElement();

        dimensions.x = xml_elem->IntAttribute("width");
        dimensions.y = xml_elem->IntAttribute("height");

        std::map<std::string, SVGElement *> id_map;

        for (XMLElement *child = xml_elem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
        {
            SVGElement *element = parseElement(child, id_map);
            if (element)
            {
                svg_elements.push_back(element);
            }
        }
    }
}
