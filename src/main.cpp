#include <Geode/Geode.hpp>
#include "LevelTimelapsePopup.hpp"
#include "LTLevelEditorLayer.hpp"

using namespace geode::prelude;

#include <Geode/modify/LevelEditorLayer.hpp>


void timelapseLevel(GJGameLevel* ogLevel) {
    auto decodedLevelString = ZipUtils::decompressString(ogLevel->m_levelString, false, 0);
    std::vector<std::string> objects = utils::string::split(decodedLevelString, ";");

    auto level = GJGameLevel::create();
    level->m_levelString = objects[0] + ";";
    level->m_fastEditorZoom = 0.4f;
    level->m_lastCameraPos = CCPoint(35, 90);

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