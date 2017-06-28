/**
    @file
    @author  Alexander Sherikov
    @copyright 2016-2017 INRIA. Licensed under the Apache License, Version 2.0.
    (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>

#include <osg/Node>
#include <osgDB/ReadFile>

#include <osg/Camera>
#include <osgDB/WriteFile>

#include <osg/ShapeDrawable>

#include <osg/LineWidth>

#include <unistd.h>
#include <math.h>

#include "tools.h"
#include "drawing_functions.h"
#include "configuration.h"
#include "robots.h"

void usage()
{
    printf("Options:\n");
    printf("    -c 'configuration file' (required)\n");
    printf("    -e (exit when the end of input file is reached)\n");
    printf("    -d duration (duration of a sleep between displaying two configurations, ms)\n");
}


int main(int argc, char **argv)
{
    int option;

    bool automatic_exit     = false;
    char *config_file_name  = NULL;
    int sleep_duration = 0;

    while ((option = getopt(argc, argv, "ec:d:")) != -1)
    {
        switch (option)
        {
            case 'c':
                config_file_name = optarg;
                break;
            case 'e':
                automatic_exit = true;
                break;
            case 'd':
                sleep_duration = strtol(optarg, NULL, 10) * 1000;
                break;
            case '?':
            default:
                usage();
                return(0);
        }
    }



    if (NULL == config_file_name)
    {
        usage();
        return(0);
    }
    try
    {
        Configuration config(config_file_name);


        osg::ref_ptr<osg::Group> root = new osg::Group();

        root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
        root->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::ON);
        root->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);


        std::vector<osg::ref_ptr<Robot> > robots;

        for (std::size_t i = 0; i < config.robots_.size(); ++i)
        {
            osg::ref_ptr<Robot> robot = new Robot;
            robot->load(  config.robots_[i].robot_description_file_,
                               config.robots_[i].data_file_);
            root->addChild(robot->robot_group_.get());

            robots.push_back(robot);
        }


        drawGroundGrid(root);
        drawFrame(root);


        // define a group, which contains all simple shapes
        osg::ref_ptr<osg::Group> shapes_group = new osg::Group();
        root->addChild(shapes_group);



        //The final step is to set up and enter a simulation loop.
        osgViewer::Viewer viewer;
        viewer.setSceneData( root );


        // camera position
        osgGA::TrackballManipulator * camera_man = new osgGA::TrackballManipulator();
        camera_man->setHomePosition(config.camera_.look_from_,
                                    config.camera_.look_at_,
                                    config.camera_.up_);
        camera_man->setAllowThrow(false); // disable spinning
        camera_man->setVerticalAxisFixed(true);
        viewer.setCameraManipulator(camera_man);


        // enable screenshots if requested
        osg::ref_ptr<SnapImageDrawCallback> snap_image_draw_callback = new SnapImageDrawCallback(
                config.screenshot_filename_prefix_,
                ".png");
        if (config.enable_screenshots_)
        {
            viewer.getCamera()->setPostDrawCallback (snap_image_draw_callback.get());
        }
        viewer.getCamera()->setClearColor(config.background_color_); // background


        viewer.realize();

        bool stop_simulation = false;

        for (std::ptrdiff_t iteration = 0; !viewer.done(); ++iteration)
        {
            // draw simple shapes
            shapes_group->removeChildren(0, shapes_group->getNumChildren());
            for (std::size_t i = 0; i < config.shapes_.size(); ++i)
            {
                config.shapes_[i]->draw(shapes_group, iteration);
            }

            // draw robots
            for (std::size_t i = 0; i < robots.size(); ++i)
            {
                robots[i]->readStates();
                if (robots[i]->file_stream_.eof() && (true == automatic_exit))
                {
                    stop_simulation = true;
                }
            }

            if (true == stop_simulation)
            {
                break;
            }

            viewer.frame();

            // for some reason this must follow a call to frame().
            if (config.enable_screenshots_)
            {
                snap_image_draw_callback->snapImageOnNextFrame(iteration);
            }

            usleep(sleep_duration);
        }
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
