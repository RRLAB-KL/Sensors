The Plugin was tested on Windows 10 and Ubuntu Linux 22.04 LTS using Unreal Editor 5.1.

## Prerequisites

* Install [Unreal Editor](https://www.unrealengine.com/en-US/download) and [set up visual studio](https://docs.unrealengine.com/5.0/en-US/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine/)
* For installation on Linux, have a look [here](https://docs.unrealengine.com/4.26/en-US/SharingAndReleasing/Linux/BeginnerLinuxDeveloper/SettingUpAnUnrealWorkflow/)
* Have a Unreal Project using Unreal Engine 5.1
* Read the [Editor Interface](https://docs.unrealengine.com/5.1/en-US/unreal-editor-interface/) and [Engine Terminology](https://docs.unrealengine.com/5.1/en-US/unreal-engine-terminology/) basics

## Installation

* Download the Sensors plugin
* Move it to the *Plugins* folder of your project, create one if it does not exist
* Open your Unreal Project
* Go to Window->Plugins and activate the Sensors plugin which is located under PROJECT/RRLab. For more info check [this](https://docs.unrealengine.com/5.0/en-US/working-with-plugins-in-unreal-engine/) link
* Restart the Editor
* If you are asked to rebuild now, press Yes. This will compile the Editor and also the plugin.
* If the editor starts again with no error messages, the Plugin is activated

## Setup

To check the content in Unreal, do the following:
* Open the *Content Browser*
* In the upper right corner of the *Content Browser*, click on *Settings*
* Check *Show Plugin Content*
* On the left of the *Content Browser*, you should now see a folder Plugins containing `Sensors Plugin Content` and `Sensors Plugin C++ Classes`

### Export annotated pointclouds

1. Drag a _RayTracingLiDARActor_  in to your scene, it can be found in `Plugins/Sensors Plugin C++ Classes/LiDAR/`
2. Adapt the parameters (**RayTracingLiDARActorParameters**) of the sensor, they can be found in the *Details* panle
3. Make sure to check **UseRGBOutput** and **WriteObjectID**
4. Under the SensorParent category, change the **TickInterval** variable since it defines the frequency of the sensor (writing to a file is very slow in comparison to creating the pointcloud in the simulation)
5.  In the AdapterComponent of the Sensor copy the path of your file to the **FilePath** variable
6.  To set the ObjectID, follow the following steps:
   1. The ObjectID of an Actor is defined as the first entry of the **Tags** array if that is a number, otherwise it is -1
   2.  Select an actor that should have an ObjectID
   3. Under the Category **Actor** click on the arrow to show the advanced settings
   4. Add an element to the **Tags** array
   5. The first entry is interpreted as a number defining the ObjectID
7. Start the simulation
8. After **TickIntervall** seconds, the pointcloud should written to the file you have selected
9. You can check the Output Log or command line for error messages

### Usage of Particle Lidar
1. Navigate to `PluginsSensors Plugin Content"/LiDAR/ParticleLidar` and drag _BP\_ParticleLidar_ into the scene
2. Parameters such as the channel count can be directly adjusted in the Details panel. To disable the particle rendering, open _PS\_LidarParticleSystem_, and disable **Sprite Renderer**
3. The point cloud can either be used in the C++ class _ParticleLidar.cpp_ or in the Blueprint _BP_ParticleLidar.cpp_. It’s also possible to use the prebuild Adapters.

See a real-time demo of the sensor here:
[![Youtube video](http://img.youtube.com/vi/RXa1iCmUg6M/0.jpg)](http://www.youtube.com/watch?v=RXa1iCmUg6M "Simulation of 512 channel LiDAR in Realtime")


### Move sensor along spline
For recording sensor data on a fixed path, there is a [spline](https://docs.unrealengine.com/5.1/en-US/blueprint-spline-components-overview-in-unreal-engine/) utilizing an event-based sensor setup.
1. Drag the _SplineFollow_ class into your scene, it can be found in `Plugins/Sensors Plugin C++ Classes/Utils/`
2. Adjust the spline according to your needs, note the following:
   1. The sensor records at every spline point.
   2. There is no recording at the first and last spline point.
   3. The detection of a spline point is done via collision overlap, so your sensor needs to have a mesh with collision
3. Drag a sensor into your scene
4. In the _SplineFollow_ actor, set the variable _Sensor_ as a reference to your previously spawned sensor.
5. Adjust the _Timestep_ according to your application if needed. Every EventTick, the value _Timestep_ is added to the current time on the spline.
6. Start the simulation.
7. Check the _start_ variable of the _SplineFollow_ actor, the sensor should now move along the spline

## License

Copyright (c) 2023 Robotics Research Lab, University of Kaiserslautern-Landau

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


