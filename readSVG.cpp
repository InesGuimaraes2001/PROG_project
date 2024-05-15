#include <iostream>
#include <sstream>
#include <string>
#include "SVGElements.hpp"
#include "external/tinyxml2/tinyxml2.h"

using namespace std;
using namespace tinyxml2;
namespace svg {

vector<Point> parsePoints(const string& points) {
    vector<Point> point_list;
    stringstream ss(points);
    string token;
    while (getline(ss, token, ' ')) {
        stringstream point_stream(token);
        string x_str, y_str;
        getline(point_stream, x_str, ',');
        getline(point_stream, y_str);
        int x = stoi(x_str);
        int y = stoi(y_str);
        point_list.emplace_back(Point{x, y});
    }
    return point_list;
}

void readSVG(const string& svg_file, Point& dimensions, vector<SVGElement*>& svg_elements) {
    XMLDocument doc;
    XMLError r = doc.LoadFile(svg_file.c_str());
    if (r != XML_SUCCESS) {
        throw runtime_error("Unable to load " + svg_file);
    }

    XMLElement* xml_elem = doc.RootElement();
    dimensions.x = xml_elem->IntAttribute("width");
    dimensions.y = xml_elem->IntAttribute("height");

    for (XMLElement* child = xml_elem->FirstChildElement(); child; child = child->NextSiblingElement()) {
        string element_name = child->Name();
        if (element_name == "ellipse") {
            int cx = child->IntAttribute("cx");
            int cy = child->IntAttribute("cy");
            int rx = child->IntAttribute("rx");
            int ry = child->IntAttribute("ry");
            Color fill = parse_color(child->Attribute("fill"));
            svg_elements.push_back(new Ellipse(fill, Point{cx, cy}, Point{rx, ry}));
        } else if (element_name == "circle") {
            int cx = child->IntAttribute("cx");
            int cy = child->IntAttribute("cy");
            int r = child->IntAttribute("r");
            Color fill = parse_color(child->Attribute("fill"));
            svg_elements.push_back(new Circle(fill, Point{cx, cy}, r));
        } else if (element_name == "polygon") {
            string points_attr = child->Attribute("points");
            vector<Point> points = parsePoints(points_attr);
            Color fill = parse_color(child->Attribute("fill"));
            svg_elements.push_back(new Polygon(fill, points));
        } else if (element_name == "line") {
            int x1 = child->IntAttribute("x1");
            int y1 = child->IntAttribute("y1");
            int x2 = child->IntAttribute("x2");
            int y2 = child->IntAttribute("y2");
            Color stroke = parse_color(child->Attribute("stroke"));
            svg_elements.push_back(new Line(stroke, Point{x1, y1}, Point{x2, y2}));
        } else if (element_name == "rect") {
            int x = child->IntAttribute("x");
            int y = child->IntAttribute("y");
            int width = child->IntAttribute("width");
            int height = child->IntAttribute("height");
            Color fill = parse_color(child->Attribute("fill"));
            svg_elements.push_back(new Rectangle(fill, Point{x, y}, width, height));
            }
    }
}
}