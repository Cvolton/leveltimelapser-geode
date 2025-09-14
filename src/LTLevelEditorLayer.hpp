#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "ObjectStream.hpp"

struct LTLevelEditorLayer : public geode::Modify<class LTLevelEditorLayer, LevelEditorLayer> {
    struct Fields {
        ObjectStream m_timelapseObjects;
        float m_timelapseDelay = 0.05f;
        float m_timelapseAccumulator = 0.f;
    };

    void initTimelapse(ObjectStream objects, float delayPerObject);
    void beginTimelapse(float dt);
    void updateTimelapse(float dt);
    void doTimelapseStep();
};