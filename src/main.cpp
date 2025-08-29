#include <Geode/Geode.hpp>

using namespace geode::prelude;

const size_t TIMELAPSE_INTERVAL = 200;

#include <Geode/modify/LevelEditorLayer.hpp>
class $modify(LTLevelEditorLayer, LevelEditorLayer) {
    struct Fields {
        std::vector<std::string> m_timelapseObjects;
        size_t m_timelapseIndex = 1;
        CCAction* m_timelapseAction = nullptr;
        float m_lastY = -9999999.f;
        std::vector<std::pair<float, float>> m_timelapsePositions;
    };

    void reimplCenterCameraOnObject(GameObject* obj) {
        auto fields = m_fields.self();

        auto winSize = CCDirector::get()->getWinSize();
        auto scale = m_objectLayer->getScale();
        auto x = (winSize.width * 0.5f) - (obj->getPositionX() * scale);
        auto y = (winSize.height * 0.5f) - (obj->getPositionY() * scale);

        if(std::abs(fields->m_lastY - y) > 300.f) {
            fields->m_lastY = y;
        }

        //cancel previous action if exists
        /*if (fields->m_timelapseAction) {
            m_objectLayer->stopAction(fields->m_timelapseAction);
            fields->m_timelapseAction = nullptr;
        }*/

        //create a ccaction that takes a second to move to setPosition
        //fields->m_timelapseAction = CCMoveTo::create(0.5f, ccp(x, fields->m_lastY));
        //m_objectLayer->runAction(fields->m_timelapseAction);

        m_objectLayer->setPosition(ccp(x, y));
        m_editorUI->constrainGameLayerPosition(-100, -100);
    }

    void reimplCenterCameraOnObjectCoords(float x, float y) {
        auto fields = m_fields.self();

        auto winSize = CCDirector::get()->getWinSize();
        auto scale = m_objectLayer->getScale();
        x = (winSize.width * 0.5f) - (x * scale);
        y = (winSize.height * 0.5f) - (y * scale);

        //cancel previous action if exists
        if (fields->m_timelapseAction) {
            m_objectLayer->stopAction(fields->m_timelapseAction);
            fields->m_timelapseAction = nullptr;
        }

        //create a ccaction that takes a second to move to setPosition
        CCEaseInOut* easeAction = CCEaseInOut::create(
            CCMoveTo::create(5.f, ccp(x, y)), 2.0f
        );
        fields->m_timelapseAction = easeAction;
        m_objectLayer->runAction(fields->m_timelapseAction);
        m_editorUI->constrainGameLayerPosition(-100, -100);

        fields->m_timelapseAction = easeAction;
    }

    void reimplCenterCameraOnObjectCoords(std::pair<float, float> coords) {
        this->reimplCenterCameraOnObjectCoords(coords.first, coords.second);
    }

    void initTimelapse(const std::vector<std::string>& objects) {
        auto fields = m_fields.self();
        fields->m_timelapseObjects = objects;

        this->scheduleOnce(schedule_selector(LTLevelEditorLayer::beginTimelapse), 2.5f);

        float minX, maxX, minY, maxY;
        size_t idx = 0;
        for(auto& obj : fields->m_timelapseObjects) {
            if (obj.empty() || !obj.starts_with("1")) continue;

            auto gameObjs = this->createObjectsFromString(obj, true, true);
            if (gameObjs->count() > 0) {
                auto gameObj = static_cast<GameObject*>(gameObjs->objectAtIndex(0));
                if (idx == 0) {
                    minX = maxX = gameObj->getPositionX();
                    minY = maxY = gameObj->getPositionY();
                } else {
                    minX = std::min(minX, gameObj->getPositionX());
                    maxX = std::max(maxX, gameObj->getPositionX());
                    minY = std::min(minY, gameObj->getPositionY());
                    maxY = std::max(maxY, gameObj->getPositionY());
                }
                m_editorUI->deleteObject(gameObj, true);
            }

            idx++;
            if (idx >= TIMELAPSE_INTERVAL) {
                float avgX = (minX + maxX) / 2.f;
                float avgY = (minY + maxY) / 2.f;
                fields->m_timelapsePositions.push_back({avgX, avgY});

                idx = 0;
            }
        }

        for(size_t i = 0; i < 5; i++) {
            m_editorUI->zoomOut(nullptr);
        }
    }

    void beginTimelapse(float dt) {
        log::info("Starting timelapse with {} objects", m_fields.self()->m_timelapseObjects.size());
        this->schedule(schedule_selector(LTLevelEditorLayer::updateTimelapse), 0.01f);
    }

    void updateTimelapse(float dt) {
        auto fields = m_fields.self();
        log::info("Updating timelapse, index: {}", fields->m_timelapseIndex);
        if (fields->m_timelapseIndex < fields->m_timelapseObjects.size()) {
            auto string = fields->m_timelapseObjects[fields->m_timelapseIndex];
            if(string.empty()) {
                return;
            }
            auto objects = this->createObjectsFromString(string, true, true);
            if(objects->count() > 0 && fields->m_timelapseIndex == 1) {
                this->reimplCenterCameraOnObject(static_cast<GameObject*>(objects->objectAtIndex(0)));
            }

            if(fields->m_timelapseIndex % TIMELAPSE_INTERVAL == (0)) {
                this->reimplCenterCameraOnObjectCoords(fields->m_timelapsePositions[(fields->m_timelapseIndex / TIMELAPSE_INTERVAL) + 1]);
            }

            if(fields->m_timelapseIndex)
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