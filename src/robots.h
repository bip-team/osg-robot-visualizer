/**
    @file
    @author  Alexander Sherikov
    @copyright 2016-2017 INRIA. Licensed under the Apache License, Version 2.0.
    (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once


class robotDataType : public osg::Referenced
{
    public:
        robotDataType(osg::ref_ptr<osg::Group> robot_group)
        {
            robot_group_ = robot_group;
        }

        void setBodyState(  const std::string name,
                            const osg::Vec3 & position,
                            const osg::Vec3 & rpy)
        {
            body_states_[name] = new osg::PositionAttitudeTransform;
            body_states_[name]->setPosition(position);
            body_states_[name]->setAttitude(convertRPYtoQuaternion(rpy));
        }

        void updateRobotState()
        {
            for (std::size_t i = 0; i < robot_group_->getNumChildren(); ++i)
            {
                osg::ref_ptr<osg::PositionAttitudeTransform>  body_state =
                        dynamic_cast<osg::PositionAttitudeTransform *> (robot_group_->getChild(i));

                if (NULL != body_state)
                {
                    std::map<std::string, osg::ref_ptr<osg::PositionAttitudeTransform> >::iterator it;

                    it = body_states_.find(body_state->getName());
                    if (it != body_states_.end())
                    {
                        body_state->setPosition(it->second->getPosition());
                        body_state->setAttitude(it->second->getAttitude());
                    }
                }
            }
        }

    public:
        std::map<std::string, osg::ref_ptr<osg::PositionAttitudeTransform> >    body_states_;

        osg::ref_ptr<osg::Group> robot_group_;
};



class robotNodeCallback : public osg::NodeCallback
{
    public:
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            osg::ref_ptr<robotDataType> robot_data = dynamic_cast<robotDataType*> (node->getUserData() );

            if(robot_data)
            {
                robot_data->updateRobotState();
            }

            traverse(node, nv);
        }
};



class Robot : public osg::Referenced
{
    protected:
        void readState( std::stringstream &stream,
                        const std::string &id) const
        {
            osg::Vec3  position;
            osg::Vec3  rotation;

            stream >> position.x();
            stream >> position.y();
            stream >> position.z();

            stream >> rotation.x();
            stream >> rotation.y();
            stream >> rotation.z();

            robot_data_->setBodyState(id, position, rotation);
        }


        bool loadRigidBody( const std::string path,
                            const std::string name,
                            osg::ref_ptr<osg::PositionAttitudeTransform> rb_const_transform = NULL,
                            const bool ignore_rotation_in_file = true)
        {
            bool load_successful = false;

            osg::ref_ptr<osgDB::Options> options = new osgDB::Options;

            if (ignore_rotation_in_file)
            {
                options->setOptionString("noRotation");
            }

            osg::ref_ptr<osg::Node> rb_node = osgDB::readNodeFile(path, options.get());

            if (rb_node != NULL)
            {
                osg::ref_ptr<osg::PositionAttitudeTransform> rb_transform = new osg::PositionAttitudeTransform();

                if (rb_const_transform.get() == NULL)
                {
                    rb_transform->addChild(rb_node.get());
                }
                else
                {
                    osg::ref_ptr<osg::PositionAttitudeTransform> rb_const_transform_copy =
                        new osg::PositionAttitudeTransform(*(rb_const_transform.get()));
                    rb_const_transform_copy->addChild(rb_node.get());
                    rb_transform->addChild(rb_const_transform_copy.get());
                }
                rb_transform->setName(name);

                robot_group_->addChild(rb_transform.get());

                load_successful = true;

                body_names_.push_back(name);
            }
            return (load_successful);
        }


    public:
        osg::ref_ptr<osg::Group>    robot_group_;
        osg::ref_ptr<robotDataType> robot_data_;
        std::ifstream               file_stream_;
        std::vector<std::string>    body_names_;


    public:
        void readStates()
        {
            std::string       line;

            if (getline(file_stream_, line))
            {
                std::stringstream stream;

                stream.str(line);
                stream.clear();

                for (std::size_t i = 0; i < body_names_.size(); ++i)
                {
                    readState(stream, body_names_[i]);
                }
            }
        }


        void load(const std::string & robot_description_file,
                  const std::string & data_file)
        {
            std::ifstream file_check(robot_description_file);
            if (file_check.fail())
            {
                throw std::runtime_error(std::string("In ") + __func__ + "() // Cannot open robot description file: " + robot_description_file);
            }
            file_check.close();
            YAML::Node config = YAML::LoadFile(robot_description_file);

            double          scale = 1.0;
            bool            ignore_body_rotation = true;
            std::string     path_to_meshes;


            if (config["ignore_body_rotation"])
            {
                ignore_body_rotation = config["ignore_body_rotation"].as<bool>();
            }

            if (config["scale"])
            {
                scale = config["scale"].as<double>();
            }

            if (config["path_to_meshes"])
            {
                path_to_meshes = config["path_to_meshes"].as<std::string>();

                std::size_t found = robot_description_file.find_last_of("/");
                path_to_meshes = robot_description_file.substr(0, found) + "/" + path_to_meshes + "/";
            }
            else
            {
                throw std::runtime_error(std::string("In ") + __func__ + "() // " + ("Path to meshes must be specified."));
            }


            robot_group_ = new osg::Group();
            body_names_.clear();

            osg::ref_ptr<osg::PositionAttitudeTransform> rb_const_transform = new osg::PositionAttitudeTransform();
            rb_const_transform->setScale(osg::Vec3(scale, scale, scale));


            if (!config["bodies"])
            {
                throw std::runtime_error(std::string("In ") + __func__ + "() // " + ("At least one body must be specified"));
            }

            YAML::Node bodies = config["bodies"];

            for (std::size_t i = 0; i < bodies.size(); ++i)
            {
                loadRigidBody(  path_to_meshes + bodies[i]["mesh_file"].as<std::string>(),
                                bodies[i]["name"].as<std::string>(),
                                rb_const_transform,
                                ignore_body_rotation);
            }

            robot_data_ = new robotDataType(robot_group_.get());

            robot_group_->setUserData(robot_data_);
            robot_group_->setUpdateCallback(new robotNodeCallback);

            file_stream_.open(data_file);
        }
};
