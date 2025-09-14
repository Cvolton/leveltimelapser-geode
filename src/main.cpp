#include <Geode/Geode.hpp>
#include "LevelTimelapsePopup.hpp"
#include "LTLevelEditorLayer.hpp"

using namespace geode::prelude;

#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(LTLevelInfoLayer, LevelInfoLayer) {
    void onTimelapse(CCObject* sender) {
        LevelTimelapsePopup::create(m_level)->show();
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
        LevelTimelapsePopup::create(m_level)->show();
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