/**
    @file
    @author  Alexander Sherikov
    @copyright 2016-2017 INRIA. Licensed under the Apache License, Version 2.0.
    (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once


void drawGroundGrid(osg::ref_ptr<osg::Group> group,
                    const double grid_size = 10,
                    const double grid_step = 0.5)
{
    osg::ref_ptr<osg::Geode>        geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry>     geometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array>    points = new osg::Vec3Array;


    for(double i = -grid_size; i <= grid_size; i += grid_step)
    {
        points->push_back(osg::Vec3f(i,-grid_size,0));
        points->push_back(osg::Vec3f(i,grid_size,0));

        points->push_back(osg::Vec3f(grid_size,i,0));
        points->push_back(osg::Vec3f(-grid_size,i,0));
    }


    osg::ref_ptr<osg::DrawArrays> da = new osg::DrawArrays( osg::PrimitiveSet::LINES,
                                                            0,
                                                            points->size());

    geometry->setVertexArray(points.get());
    geometry->addPrimitiveSet(da.get());

    geode->addDrawable(geometry.get());
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    group->addChild(geode.get());
}



void drawFrame( osg::ref_ptr<osg::Group> group,
                const double length = 1)
{
    osg::ref_ptr<osg::Geode>        geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry>     geometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array>    points = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec4Array>    colors = new osg::Vec4Array;

    osg::ref_ptr<osg::LineWidth>  linewidth = new osg::LineWidth();
    linewidth->setWidth(3.0f);
    geode->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);


    // X blue
    colors->push_back(osg::Vec4(0.0f,0.0f,1.0f,1.0f));
    colors->push_back(osg::Vec4(0.0f,0.0f,1.0f,1.0f));
    points->push_back(osg::Vec3f(0,0,0));
    points->push_back(osg::Vec3f(length,0,0));

    // Y green
    colors->push_back(osg::Vec4(0.0f,1.0f,0.0f,1.0f));
    colors->push_back(osg::Vec4(0.0f,1.0f,0.0f,1.0f));
    points->push_back(osg::Vec3f(0,0,0));
    points->push_back(osg::Vec3f(0,length,0));

    // Z red
    colors->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
    colors->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
    points->push_back(osg::Vec3f(0,0,0));
    points->push_back(osg::Vec3f(0,0,length));


    osg::ref_ptr<osg::DrawArrays> da = new osg::DrawArrays( osg::PrimitiveSet::LINES,
                                                            0,
                                                            points->size());

    geometry->setVertexArray(points.get());
    geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    geometry->addPrimitiveSet(da.get());

    geode->addDrawable(geometry.get());
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    group->addChild(geode.get());
}



void drawArrow( osg::ref_ptr<osg::Group> group,
                const osg::Vec3 &base,
                const osg::Vec3 &direction,
                const osg::Vec4 &color)
{
    double head_radius = 0.015;
    double head_length = 0.06;
    osg::Vec3 head_position;

    double body_radius = 0.005;
    double body_length = 0.0;
    osg::Vec3 body_position;

    if (head_length > direction.length())
    {
        head_length = direction.length();
        body_length = 0.0;
    }
    else
    {
        body_length = direction.length() - head_length;
    }
    body_position = osg::Vec3(0., 0., (direction.length() - head_length)/2);
    head_position = osg::Vec3(0., 0., direction.length() - head_length);


    osg::ref_ptr<osg::Cone> cone_shape = new osg::Cone( head_position, head_radius, head_length);
    cone_shape->setDataVariance(osg::Object::DYNAMIC);

    osg::ref_ptr<osg::Cylinder> cylinder_shape = new osg::Cylinder( body_position, body_radius, body_length);
    cylinder_shape->setDataVariance(osg::Object::DYNAMIC);

    osg::ref_ptr<osg::CompositeShape> composite_shape = new osg::CompositeShape();
    composite_shape->addChild(cone_shape);
    composite_shape->addChild(cylinder_shape);

    osg::ref_ptr<osg::ShapeDrawable> composite_drawable = new osg::ShapeDrawable(composite_shape);
    composite_drawable->setColor( color );

    osg::ref_ptr<osg::Geode> composite_geode = new osg::Geode();
    composite_geode->addDrawable(composite_drawable);

    osg::ref_ptr<osg::PositionAttitudeTransform> composite_transform = new osg::PositionAttitudeTransform;
    composite_transform->setPosition(base);

    osg::Vec3 normalized_direction = direction;
    normalized_direction.normalize();
    osg::Vec3 rotation_axis = osg::Vec3(0., 0., 1.) ^ normalized_direction;
    if (osg::Vec3(0., 0., 0.) == rotation_axis)
    {
        rotation_axis = osg::Vec3(0., 0., 1.) ^ osg::Vec3(0., 1., 0.);
    }
    composite_transform->setAttitude(osg::Quat( acos(osg::Vec3(0., 0., 1.) * normalized_direction),
                                                rotation_axis));
    composite_transform->addChild(composite_geode);

    group->addChild(composite_transform);
}



void drawBox(   osg::ref_ptr<osg::Group> group,
                const osg::Vec3 &position,
                const osg::Vec3 &rpy,
                const osg::Vec3 &width,
                const osg::Vec4 &color)
{
    osg::ref_ptr<osg::Box> box_shape = new osg::Box( osg::Vec3(0,0,0), width.x(), width.y(), width.z());
    box_shape->setDataVariance(osg::Object::DYNAMIC);

    osg::ref_ptr<osg::ShapeDrawable> box_drawable = new osg::ShapeDrawable(box_shape);
    box_drawable->setColor( color );

    osg::ref_ptr<osg::Geode> box_geode = new osg::Geode();
    box_geode->addDrawable(box_drawable);
    //box_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::ref_ptr<osg::PositionAttitudeTransform> box_transform = new osg::PositionAttitudeTransform;
    box_transform->setPosition(position);
    box_transform->setAttitude(convertRPYtoQuaternion(rpy));
    box_transform->addChild(box_geode);

    group->addChild(box_transform);
}



void drawSphere(osg::ref_ptr<osg::Group> group,
                const osg::Vec3 &position,
                const double radius,
                const osg::Vec4 &color)
{
    osg::ref_ptr<osg::Sphere> sphere_shape = new osg::Sphere( osg::Vec3(0,0,0), radius);
    sphere_shape->setDataVariance(osg::Object::DYNAMIC);

    osg::ref_ptr<osg::ShapeDrawable> sphere_drawable = new osg::ShapeDrawable(sphere_shape);
    sphere_drawable->setColor( color );

    osg::ref_ptr<osg::Geode> sphere_geode = new osg::Geode();
    sphere_geode->addDrawable(sphere_drawable);
    //sphere_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::ref_ptr<osg::PositionAttitudeTransform> sphere_transform = new osg::PositionAttitudeTransform;
    sphere_transform->setPosition(position);
    sphere_transform->addChild(sphere_geode);

    group->addChild(sphere_transform);
}



void drawCylinder(osg::ref_ptr<osg::Group> group,
                const osg::Vec3 &position,
                const osg::Vec3 &rpy,
                const double radius,
                const double length,
                const osg::Vec4 &color)
{
    osg::ref_ptr<osg::Cylinder> cylinder_shape = new osg::Cylinder( osg::Vec3(0,0,0), radius, length);
    cylinder_shape->setDataVariance(osg::Object::DYNAMIC);


    osg::ref_ptr<osg::ShapeDrawable> cylinder_drawable = new osg::ShapeDrawable(cylinder_shape);
    cylinder_drawable->setColor( color );

    osg::ref_ptr<osg::Geode> cylinder_geode = new osg::Geode();
    cylinder_geode->addDrawable(cylinder_drawable);
    //cylinder_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::ref_ptr<osg::PositionAttitudeTransform> cylinder_transform = new osg::PositionAttitudeTransform;
    cylinder_transform->setPosition(position);
    cylinder_transform->setAttitude(convertRPYtoQuaternion(rpy));
    cylinder_transform->addChild(cylinder_geode);

    group->addChild(cylinder_transform);
}
