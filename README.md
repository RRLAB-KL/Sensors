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
