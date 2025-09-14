#include "LTLevelEditorLayer.hpp"

using namespace geode::prelude;

void LTLevelEditorLayer::initTimelapse(ObjectStream objects, float delayPerObject) {
    auto fields = m_fields.self();
    fields->m_timelapseObjects = std::move(objects);
    fields->m_timelapseDelay = delayPerObject;

    this->scheduleOnce(schedule_selector(LTLevelEditorLayer::beginTimelapse), 1.f);
}

void LTLevelEditorLayer::beginTimelapse(float dt) {
    auto fields = m_fields.self();
    this->schedule(schedule_selector(LTLevelEditorLayer::updateTimelapse), fields->m_timelapseDelay);
}

void LTLevelEditorLayer::updateTimelapse(float dt)  {
    auto fields = m_fields.self();
    float acc = fields->m_timelapseAccumulator + dt;

    while (acc >= fields->m_timelapseDelay) {
        acc -= fields->m_timelapseDelay;
        this->doTimelapseStep();
    }
    fields->m_timelapseAccumulator = acc;
}

void LTLevelEditorLayer::doTimelapseStep() {
    auto fields = m_fields.self();
    auto object = fields->m_timelapseObjects.getNextObject();
    if (!object.empty()) {
        this->createObjectsFromString(object, true, true);
    } else {
        log::debug("Timelapse complete");
        this->unschedule(schedule_selector(LTLevelEditorLayer::updateTimelapse));
    }
}