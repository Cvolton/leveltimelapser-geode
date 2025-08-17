#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/LevelEditorLayer.hpp>
class $modify(LTLevelEditorLayer, LevelEditorLayer) {
    struct Fields {
        std::vector<std::string> m_timelapseObjects;
        size_t m_timelapseIndex = 1;
    };

    void initTimelapse(const std::vector<std::string>& objects) {
        auto fields = m_fields.self();
        fields->m_timelapseObjects = objects;

        this->scheduleOnce(schedule_selector(LTLevelEditorLayer::beginTimelapse), 2.5f);
    }

    void beginTimelapse(float dt) {
        log::info("Starting timelapse with {} objects", m_fields.self()->m_timelapseObjects.size());
        this->schedule(schedule_selector(LTLevelEditorLayer::updateTimelapse), 0.01f);
    }

    void updateTimelapse(float dt) {
        auto fields = m_fields.self();
        log::info("Updating timelapse, index: {}", fields->m_timelapseIndex);
        if (fields->m_timelapseIndex < fields->m_timelapseObjects.size()) {
            this->createObjectsFromString(fields->m_timelapseObjects[fields->m_timelapseIndex], true, true);
            fields->m_timelapseIndex++;
        }
    }
};

void timelapseLevel(GJGameLevel* ogLevel) {
    auto decodedLevelString = ZipUtils::decompressString(ogLevel->m_levelString, false, 0);
    std::vector<std::string> objects = utils::string::split(decodedLevelString, ";");

    auto level = GJGameLevel::create();
    level->m_levelString = objects[0] + ";";

    auto editor = LevelEditorLayer::scene(level, false);
    auto transitionFade = CCTransitionFade::create(0.5f, editor);
    CCDirector::sharedDirector()->replaceScene(transitionFade);

    static_cast<LTLevelEditorLayer*>(editor->getChildByType<LevelEditorLayer>(0))->initTimelapse(objects);
}

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(LTLevelInfoLayer, LevelInfoLayer) {
    void onTimelapse(CCObject* sender) {
        timelapseLevel(m_level);
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        
        auto timelapseBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_gpBtn_001.png"),
            this,
            menu_selector(LTLevelInfoLayer::onTimelapse)
        );

        if (auto menu = this->getChildByID("left-side-menu")) {
            menu->addChild(timelapseBtn);
            menu->updateLayout();
        }

        return true;
    }
};

#include <Geode/modify/EditLevelLayer.hpp>
class $modify(LTEditLevelLayer, EditLevelLayer) {
    void onTimelapse(CCObject* sender) {
        timelapseLevel(m_level);
    }

    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level)) return false;

        auto timelapseBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_gpBtn_001.png"),
            this,
            menu_selector(LTEditLevelLayer::onTimelapse)
        );

        if (auto menu = this->getChildByID("level-actions-menu")) {
            menu->addChild(timelapseBtn);
            menu->updateLayout();
        }

        return true;
    }
};