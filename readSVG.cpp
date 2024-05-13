#include <iostream>
#include "SVGElements.hpp"
#include "external/tinyxml2/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

namespace svg {
void readSVG(const string& svg_file, Point& dimensions, vector<SVGElement*>& svg_elements) {
    XMLDocument doc;
    XMLError r = doc.LoadFile(svg_file.c_str());
    if (r != XML_SUCCESS) {
        throw runtime_error("Unable to load " + svg_file);
    }

    XMLElement* xml_elem = doc.RootElement();

    dimensions.x = xml_elem->IntAttribute("width");
    dimensions.y = xml_elem->IntAttribute("height");

    XMLElement* child = xml_elem->FirstChildElement();
    while (child != nullptr) {
        string element_name = child->Name();
        if (element_name == "ellipse") {
            int cx = child->IntAttribute("cx");
            int cy = child->IntAttribute("cy");
            int rx = child->IntAttribute("rx");
            int ry = child->IntAttribute("ry");
            Color fill = parse_color(child->Attribute("fill"));
            svg_elements.push_back(new Ellipse(fill, Point{cx, cy}, Point{rx, ry}));
        }
        else if (element_name == "polygon") {
            Color fill = parse_color(child->Attribute("fill"));
            vector<Point> points;
            XMLElement* point_elem = child->FirstChildElement("point");
            while (point_elem != nullptr) {
                int x = point_elem->IntAttribute("x");
                int y = point_elem->IntAttribute("y");
                points.emplace_back(Point{x, y});
                point_elem = point_elem->NextSiblingElement("point");
            }
            svg_elements.push_back(new Polygon(fill, points));
        }
        child = child->NextSiblingElement();
    }
}
}
