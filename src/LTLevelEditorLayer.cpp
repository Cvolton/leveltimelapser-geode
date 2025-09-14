#include "LTLevelEditorLayer.hpp"

using namespace geode::prelude;

void LTLevelEditorLayer::initTimelapse(const std::vector<std::string>& objects, float delayPerObject) {
    auto fields = m_fields.self();
    fields->m_timelapseObjects = objects;
    fields->m_timelapseDelay = delayPerObject;

    this->scheduleOnce(schedule_selector(LTLevelEditorLayer::beginTimelapse), 1.f);
}

void LTLevelEditorLayer::beginTimelapse(float dt) {
    auto fields = m_fields.self();
    log::debug("Starting timelapse with {} objects", fields->m_timelapseObjects.size());
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
    if (fields->m_timelapseIndex < fields->m_timelapseObjects.size()) {
        this->createObjectsFromString(fields->m_timelapseObjects[fields->m_timelapseIndex], true, true);
        fields->m_timelapseIndex++;
    } else {
        log::debug("Timelapse complete");
        this->unschedule(schedule_selector(LTLevelEditorLayer::updateTimelapse));
    }
}