#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
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

void parseElement(XMLElement* child, std::vector<std::unique_ptr<SVGElement>>& elements, std::unordered_map<std::string, std::unique_ptr<SVGElement>>& element_map) {
    string element_name = child->Name();
    if (element_name == "ellipse") {
        int cx = child->IntAttribute("cx");
        int cy = child->IntAttribute("cy");
        int rx = child->IntAttribute("rx");
        int ry = child->IntAttribute("ry");
        Color fill = parse_color(child->Attribute("fill"));
        elements.push_back(std::unique_ptr<SVGElement>(new Ellipse(fill, Point{cx, cy}, Point{rx, ry})));
    } else if (element_name == "circle") {
        int cx = child->IntAttribute("cx");
        int cy = child->IntAttribute("cy");
        int r = child->IntAttribute("r");
        Color fill = parse_color(child->Attribute("fill"));
        elements.push_back(std::unique_ptr<SVGElement>(new Circle(fill, Point{cx, cy}, r)));
    } else if (element_name == "polygon") {
        string points_attr = child->Attribute("points");
        vector<Point> points = parsePoints(points_attr);
        Color fill = parse_color(child->Attribute("fill"));
        elements.push_back(std::unique_ptr<SVGElement>(new Polygon(fill, points)));
    } else if (element_name == "line") {
        int x1 = child->IntAttribute("x1");
        int y1 = child->IntAttribute("y1");
        int x2 = child->IntAttribute("x2");
        int y2 = child->IntAttribute("y2");
        Color stroke = parse_color(child->Attribute("stroke"));
        elements.push_back(std::unique_ptr<SVGElement>(new Line(stroke, Point{x1, y1}, Point{x2, y2})));
    } else if (element_name == "rect") {
        int x = child->IntAttribute("x");
        int y = child->IntAttribute("y");
        int width = child->IntAttribute("width");
        int height = child->IntAttribute("height");
        Color fill = parse_color(child->Attribute("fill"));
        elements.push_back(std::unique_ptr<SVGElement>(new Rectangle(fill, Point{x, y}, width, height)));
    } else if (element_name == "use") {
        const char* href = child->Attribute("href");
        if (href) {
            string ref(href);
            if (ref[0] == '#') {
                ref = ref.substr(1);
            }
            auto it = element_map.find(ref);
            if (it != element_map.end()) {
                int x = child->IntAttribute("x");
                int y = child->IntAttribute("y");
                elements.push_back(std::unique_ptr<SVGElement>(new Use(it->second.get(), Point{x, y})));
            }
        }
    } else if (element_name == "g") {
        vector<std::unique_ptr<SVGElement>> group_elements;
        for (XMLElement* grandchild = child->FirstChildElement(); grandchild; grandchild = grandchild->NextSiblingElement()) {
            parseElement(grandchild, group_elements, element_map);
        }
        std::vector<SVGElement*> raw_group_elements;
        for (auto& elem : group_elements) {
            raw_group_elements.push_back(elem.release());
        }
        elements.push_back(std::unique_ptr<SVGElement>(new Group(raw_group_elements)));
    }
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

    unordered_map<string, std::unique_ptr<SVGElement>> element_map;

    std::vector<std::unique_ptr<SVGElement>> unique_svg_elements;

    for (XMLElement* child = xml_elem->FirstChildElement(); child; child = child->NextSiblingElement()) {
        const char* id_attr = child->Attribute("id");
        if (id_attr) {
            string id = id_attr;
            parseElement(child, unique_svg_elements, element_map);
            element_map[id] = std::move(unique_svg_elements.back());
            unique_svg_elements.pop_back();
        } else {
            parseElement(child, unique_svg_elements, element_map);
        }
    }

    for (auto& entry : unique_svg_elements) {
        svg_elements.push_back(entry.release());
    }
}

} // namespace svg
