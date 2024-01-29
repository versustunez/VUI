# Versus UI Language (VUI)

## What is VUI

VUI started as a UI Language for my Synthesizer VeNo and was first called VeNoUI Lang.

At a later time I started to make more Plugins and VeNoUI was to depended on VeNos Core itself.

So I rewrote VeNoUI to make it more compatible to other Plugins and less dependent on VeNo.
Also, because its no longer dependent on VeNo I changed the name to VersusUI because it's now used in all VstZ plugins.

## Why

I wanted to have an easy way to create UIs for my Plugins and not to have to recompile the Plugins everytime I wanted to
change something.

## Interpreter

---------------

The Interpreter will use the Source and will PreInterpret the source and builds a "VUI::InterpretedComponent".
The InterpretedComponent contains the structure and the base information like BackgroundColor, position and so forth.

For each Editor opening the Interpreter will now walk the InterpretedComponent and create the
corresponding `VUI::Component` out of it.

it will build the structure and will set up all the Dimensions and all. at the end it will return a `VUI::Container`.

the VUI::Container implements the `VUI::Component` interface and that is using `juce::Component` in the background.

so it should work out of the box ;)

## Important

### Instance

VUI provides an "Instance" class. that class is used as a Container to provide access to the current Instance.
The Interpreter has a function to provide that.

Make sure the Instance has access to the `juce Parameters`. this will avoid the usage of some global instanceIDs and
more specific give the instance around.

the Instance will be provided as

```
std::shared_ptr<VUI::Instance>
```

### VUI::Component

The Component class has some helper functions and some virtual ones.

##### As< ClassName >();

This will return the Component as a wanted class or will return nullptr on failure.

##### virtual void OnCreate();

Use that for your Constructor calls.
Because VUI will not call the constructor directly with all information and more like a wrapper.

```c++
Component::CreateWithParameter(std::shared_ptr<VUI::Instance> instance, const std::string& parameter);
Component::CreateWithParameterAndName(std::shared_ptr<VUI::Instance> instance, const std::string& parameter, const std::string& name);
Component::CreateWithName(std::shared_ptr<VUI::Instance> instance, const std::string& name);
Component::Create(std::shared_ptr<VUI::Instance> instance);
```

## Examples

You can see the "example.vui" file in the repo for an example.

and for an example of the Interpreter you can see the [VeNo Source](https://github.com/versustunez/VeNo) or the example
directory.

The transpiler is used here [VeClip](https://github.com/versustunez/VeClip)

