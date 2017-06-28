This is a robot visualization tool based on OpenSceneGraph and configurable
with YAML files. Its key features are simplicity, minimal dependencies, and
flexibility.

You can see a demo on YouTube:

[![demo](https://img.youtube.com/vi/X9HEi2JE-CU/0.jpg)](https://www.youtube.com/watch?v=X9HEi2JE-CU)

A scene is described with a YAML file (see '`data_files/hrp4_stairs.yaml`'),
which includes:

    1. General parameters (background color, screenshot parameters)

    2. A section where robot(s) are described, for each robot the following
       data must be specified:
        - a unique name;
        - a path to YAML file, which contains a list of bodies with
          corresponding mesh files (see hrp4_description.yaml);
        - a data file containing poses of the bodies to replay during the
          simulation.

    3. A section with camera parameters.

    4. A section where additional shapes are described: boxes, spheres,
       cylinders, arrows. Some values describing the shapes can be read from
       files as well.

Poses of the bodies are represented by 6D XYZ-RPY vectors and are read from a
data file line by line: each line must contain poses of all bodies in the same
order as they are defined.

Note: The tool was tested with OpenSceneGraph version which has no support for
COLLADA (.dae) files. COLLADA files can be converted to WaveFront (.obj) format
using '`util/dae_to_obj.py`' script.
