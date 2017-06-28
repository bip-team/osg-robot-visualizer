# converts Collada (*.dae) files in the current directory to Wavefront (*.obj) files
# must be executed from blender

import os
import bpy

for fn in os.listdir('.'):
    if os.path.isfile(fn):
        if fn.endswith(".dae"):
            name = fn.split(".")[0]
            bpy.ops.wm.collada_import(filepath=fn)
            bpy.ops.export_scene.obj(filepath=name + ".obj")
            bpy.ops.object.delete()
