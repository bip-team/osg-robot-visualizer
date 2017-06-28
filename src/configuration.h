/**
    @file
    @author  Alexander Sherikov
    @copyright 2016-2017 INRIA. Licensed under the Apache License, Version 2.0.
    (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once

#include "yaml-cpp/yaml.h"

namespace YAML
{
    template<>
    struct convert<osg::Vec3>
    {
        static Node encode(const osg::Vec3 & rhs)
        {
            Node node;
            node.push_back(rhs.x());
            node.push_back(rhs.y());
            node.push_back(rhs.z());
            return node;
        }

        static bool decode(const Node& node, osg::Vec3& rhs)
        {
            if(!node.IsSequence() || node.size() != 3)
            {
                return false;
            }
            rhs.x() = node[0].as<double>();
            rhs.y() = node[1].as<double>();
            rhs.z() = node[2].as<double>();
            return true;
        }
    };

    template<>
    struct convert<osg::Vec4>
    {
        static Node encode(const osg::Vec4 & rhs)
        {
            Node node;
            node.push_back(rhs.x());
            node.push_back(rhs.y());
            node.push_back(rhs.z());
            node.push_back(rhs.w());
            return node;
        }

        static bool decode(const Node& node, osg::Vec4& rhs)
        {
            if(!node.IsSequence() || node.size() != 4)
            {
                return false;
            }
            rhs.x() = node[0].as<double>();
            rhs.y() = node[1].as<double>();
            rhs.z() = node[2].as<double>();
            rhs.w() = node[3].as<double>();
            return true;
        }
    };
}



class SimpleShape : public osg::Referenced
{
    protected:
        void readVector(std::stringstream &stream,
                        osg::Vec3 & vector) const
        {
            stream >> vector.x();
            stream >> vector.y();
            stream >> vector.z();
        }


    public:
        osg::Vec3 position_;

        osg::Vec4 color_;

        std::ptrdiff_t first_iter_;
        std::ptrdiff_t last_iter_;


    public:
        SimpleShape()
        {
            position_ = osg::Vec3(0., 0., 0.);
            color_ = osg::Vec4(0., 0., 0., 1.);
            first_iter_ = 0;
            last_iter_ = -1;
        }

        void read(const YAML::Node & node, const std::string & path_to_config)
        {
            (void) path_to_config;
            position_ = node["position"].as<osg::Vec3>();
            color_ = node["color"].as<osg::Vec4>();
            first_iter_ = node["first_iter"].as<std::ptrdiff_t>();
            last_iter_ = node["last_iter"].as<std::ptrdiff_t>();
        }

        virtual void draw(  osg::ref_ptr<osg::Group>,
                            const std::ptrdiff_t) = 0;
};


class Arrow : public SimpleShape
{
    private:
        std::ifstream   file_stream_;
        bool            read_vector_from_file_;


    public:
        double vector_normalize_;
        osg::Vec3 vector_;


    public:
        Arrow()
        {
            vector_ = osg::Vec3(0., 0., 0.);
        }

        void read(const YAML::Node & node, const std::string & path_to_config)
        {
            SimpleShape::read(node, path_to_config);

            vector_normalize_ = node["vector_normalize"].as<double>();

            if (node["vector"].IsSequence())
            {
                vector_ = node["vector"].as<osg::Vec3>();
                read_vector_from_file_ = false;
            }
            else
            {
                file_stream_.open(path_to_config + node["vector"].as<std::string>());
                read_vector_from_file_ = true;
            }
        }


        void draw(  osg::ref_ptr<osg::Group> group,
                    const std::ptrdiff_t iteration)
        {
            if ((iteration >= first_iter_) && ((last_iter_ == -1) || (last_iter_ >= iteration)))
            {
                if (read_vector_from_file_)
                {
                    std::string       line;

                    if (getline(file_stream_, line))
                    {
                        std::stringstream stream;

                        stream.str(line);
                        stream.clear();
                        readVector(stream, vector_);
                    }
                }
                drawArrow(group, position_, vector_/vector_normalize_, color_);
            }
        }
};


class Box : public SimpleShape
{
    public:
        osg::Vec3 width_;
        osg::Vec3 rpy_;


    public:
        Box()
        {
            width_ = osg::Vec3(0., 0., 0.);
            rpy_ = osg::Vec3(0., 0., 0.);
        }

        void read(const YAML::Node & node, const std::string & path_to_config)
        {
            SimpleShape::read(node, path_to_config);

            rpy_ = node["rpy"].as<osg::Vec3>();
            width_ = node["width"].as<osg::Vec3>();
        }


        void draw(  osg::ref_ptr<osg::Group> group,
                    const std::ptrdiff_t iteration)
        {
            if ((iteration >= first_iter_) && ((last_iter_ == -1) || (last_iter_ >= iteration)))
            {
                drawBox(group, position_, rpy_, width_, color_);
            }
        }
};


class Cube : public SimpleShape
{
    public:
        double width_;
        osg::Vec3 rpy_;


    public:
        Cube()
        {
            width_ = 0.;
            rpy_ = osg::Vec3(0., 0., 0.);
        }

        void read(const YAML::Node & node, const std::string & path_to_config)
        {
            SimpleShape::read(node, path_to_config);

            rpy_ = node["rpy"].as<osg::Vec3>();
            width_ = node["width"].as<double>();
        }


        void draw(  osg::ref_ptr<osg::Group> group,
                    const std::ptrdiff_t iteration)
        {
            if ((iteration >= first_iter_) && ((last_iter_ == -1) || (last_iter_ >= iteration)))
            {
                osg::Vec3 width = osg::Vec3(width_, width_, width_);
                drawBox(group, position_, rpy_, width, color_);
            }
        }
};


class Sphere : public SimpleShape
{
    public:
        double radius_;


    public:
        Sphere()
        {
            radius_ = 0.;
        }

        void read(const YAML::Node & node, const std::string & path_to_config)
        {
            SimpleShape::read(node, path_to_config);

            radius_ = node["radius"].as<double>();
        }


        void draw(  osg::ref_ptr<osg::Group> group,
                    const std::ptrdiff_t iteration)
        {
            if ((iteration >= first_iter_) && ((last_iter_ == -1) || (last_iter_ >= iteration)))
            {
                drawSphere(group, position_, radius_, color_);
            }
        }
};


class Cylinder : public SimpleShape
{
    public:
        double length_;
        double radius_;
        osg::Vec3 rpy_;


    public:
        Cylinder()
        {
            length_ = 0.0;
            radius_ = 0.0;
            rpy_ = osg::Vec3(0., 0., 0.);
        }

        void read(const YAML::Node & node, const std::string & path_to_config)
        {
            SimpleShape::read(node, path_to_config);

            rpy_ = node["rpy"].as<osg::Vec3>();
            length_ = node["length"].as<double>();
            radius_ = node["radius"].as<double>();
        }


        void draw(  osg::ref_ptr<osg::Group> group,
                    const std::ptrdiff_t iteration)
        {
            if ((iteration >= first_iter_) && ((last_iter_ == -1) || (last_iter_ >= iteration)))
            {
                drawCylinder(group, position_, rpy_, radius_, length_, color_);
            }
        }
};


class Configuration
{
    private:
        template<class ShapeType_t>
        void readShapeIfTypeMatches(const YAML::Node &node,
                                    const std::string &type,
                                    const std::string & path_to_config)
        {
            if (type == node["type"].as<std::string>())
            {
                osg::ref_ptr<ShapeType_t> shape = new ShapeType_t();
                shape->read(node, path_to_config);
                shapes_.push_back(shape);
            }
        }


    public:
        struct RobotDescription
        {
            std::string name_;
            std::string robot_description_file_;
            std::string data_file_;
        };


        struct CameraPosition
        {
            osg::Vec3 look_from_;
            osg::Vec3 look_at_;
            osg::Vec3 up_;

            CameraPosition()
            {
                look_from_ = osg::Vec3(0,-5,2);
                look_at_ = osg::Vec3(0,1,0);
                up_ = osg::Vec3(0,0,0);
            }

            void read(const YAML::Node &node)
            {
                look_from_ = node["look_from"].as<osg::Vec3>();
                look_at_ = node["look_at"].as<osg::Vec3>();
                up_ = node["up"].as<osg::Vec3>();
            }
        };


    public:
        bool                                    enable_screenshots_;
        std::string                             screenshot_filename_prefix_;
        std::vector<RobotDescription>           robots_;
        std::vector<osg::ref_ptr<SimpleShape> > shapes_;
        CameraPosition                          camera_;
        osg::Vec4                               background_color_;


    public:
        Configuration(const std::string & filename)
        {
            std::size_t found = filename.find_last_of("/");
            std::string path_to_config = "";
            if (std::string::npos != found)
            {
                path_to_config = filename.substr(0, found) + "/";
            }



            YAML::Node config = YAML::LoadFile(filename);


            if (config["enable_screenshots"])
            {
                enable_screenshots_ = config["enable_screenshots"].as<bool>();
            }
            else
            {
                enable_screenshots_ = false;
            }


            if (config["screenshot_filename_prefix"])
            {
                screenshot_filename_prefix_ = path_to_config + config["screenshot_filename_prefix"].as<std::string>();
            }


            if (config["camera"])
            {
                camera_.read(config["camera"]);
            }


            if (config["background_color"])
            {
                background_color_ = config["background_color"].as<osg::Vec4>();
            }


            if (config["shapes"])
            {
                YAML::Node shapes = config["shapes"];

                for (std::size_t i = 0; i < shapes.size(); ++i)
                {
                    readShapeIfTypeMatches<Cube>(shapes[i], "cube", path_to_config);
                    readShapeIfTypeMatches<Box>(shapes[i], "box", path_to_config);
                    readShapeIfTypeMatches<Sphere>(shapes[i], "sphere", path_to_config);
                    readShapeIfTypeMatches<Arrow>(shapes[i], "arrow", path_to_config);
                    readShapeIfTypeMatches<Cylinder>(shapes[i], "cylinder", path_to_config);
                }
            }


            if (!config["robots"])
            {
                throw std::runtime_error(std::string("In ") + __func__ + "() // " + ("At least one robot must be specified"));
            }

            YAML::Node robots = config["robots"];

            for (std::size_t i = 0; i < robots.size(); ++i)
            {
                RobotDescription robot;

                robot.data_file_ = path_to_config + robots[i]["data_file"].as<std::string>();
                robot.name_ = robots[i]["name"].as<std::string>();
                robot.robot_description_file_ = path_to_config + robots[i]["robot_description"].as<std::string>();

                robots_.push_back(robot);
            }
        }
};
