#include "LTLevelEditorLayer.hpp"

using namespace geode::prelude;

void LTLevelEditorLayer::initTimelapse(const std::vector<std::string>& objects) {
    auto fields = m_fields.self();
    fields->m_timelapseObjects = objects;

    this->scheduleOnce(schedule_selector(LTLevelEditorLayer::beginTimelapse), 1.f);
}

void LTLevelEditorLayer::beginTimelapse(float dt) {
    log::debug("Starting timelapse with {} objects", m_fields.self()->m_timelapseObjects.size());
    this->schedule(schedule_selector(LTLevelEditorLayer::updateTimelapse), 0.01f);
}

void LTLevelEditorLayer::updateTimelapse(float dt)  {
    auto fields = m_fields.self();
    if (fields->m_timelapseIndex < fields->m_timelapseObjects.size()) {
        this->createObjectsFromString(fields->m_timelapseObjects[fields->m_timelapseIndex], true, true);
        fields->m_timelapseIndex++;
    }
}