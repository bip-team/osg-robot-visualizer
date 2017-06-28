/**
    @file
    @author  Alexander Sherikov
    @copyright 2016-2017 INRIA. Licensed under the Apache License, Version 2.0.
    (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once

#include <iomanip>

// https://groups.google.com/forum/#!topic/osg-users/Sv1WCX4zFXc
class SnapImageDrawCallback : public osg::Camera::DrawCallback
{
    public:
        SnapImageDrawCallback(  const std::string & filename_prefix,
                                const std::string & filename_suffix)
        {
            snap_image_on_next_frame_ = false;
            filename_prefix_ = filename_prefix;
            filename_suffix_ = filename_suffix;
        }


        void snapImageOnNextFrame(const std::size_t index)
        {
            snap_image_on_next_frame_ = true;
            index_ = index;
        }


        virtual void operator () (const osg::Camera& camera) const
        {
            if (snap_image_on_next_frame_)
            {
                int x       = camera.getViewport()->x();
                int y       = camera.getViewport()->y();

                int width   = camera.getViewport()->width();
                int height  = camera.getViewport()->height();

                osg::ref_ptr<osg::Image> image = new osg::Image;


                std::stringstream filename;
                filename    << filename_prefix_
                            << std::setw(10) << std::setfill('0') << index_
                            << filename_suffix_;

                image->readPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE);
                if (osgDB::writeImageFile(*image, filename.str()))
                {
                    std::cout  << "Saved screen image to `"<< filename.str() <<"`"<< std::endl;
                }

                snap_image_on_next_frame_ = false;
            }
        }

    protected:
        std::string     filename_prefix_;
        std::string     filename_suffix_;
        std::size_t     index_;


        mutable bool    snap_image_on_next_frame_;
};



osg::Quat convertRPYtoQuaternion(const osg::Vec3 & rpy)
{
    return (osg::Quat(  rpy.x(), osg::Vec3(1,0,0),
                        rpy.y(), osg::Vec3(0,1,0),
                        rpy.z(), osg::Vec3(0,0,1)));
}
