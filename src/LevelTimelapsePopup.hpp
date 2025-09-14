#include <Geode/Geode.hpp>
#include "LTLevelEditorLayer.hpp"
#include "ObjectStream.hpp"

using namespace geode::prelude;

class LevelTimelapsePopup : public geode::Popup<GJGameLevel*> {
protected:
    GJGameLevel* m_level = nullptr;
    Slider* m_slider = nullptr;
    CCLabelBMFont* m_timeEstimated = nullptr;
    ObjectStream m_objects;

    bool setup(GJGameLevel* level) override {
        this->setTitle("Timelapse Speed");
        m_level = level;

        auto decodedLevelString = ZipUtils::decompressString(level->m_levelString, false, 0);
        m_objects = ObjectStream(decodedLevelString);

        auto button = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Go"), [this](CCMenuItemSpriteExtra* item) {
            this->timelapseLevel(m_level);
        });
        button->setScale(button->m_baseScale = 0.95f);
        m_buttonMenu->addChildAtPosition(button, Anchor::Bottom, CCPoint(0, 26));

        m_slider = Slider::create(this, menu_selector(LevelTimelapsePopup::onSliderChange));
        m_slider->setValue(0.5f);
        m_buttonMenu->addChildAtPosition(m_slider, Anchor::Center, CCPoint(0, 0));

        m_timeEstimated = CCLabelBMFont::create("Estimated Time: 0s", "bigFont.fnt");
        m_timeEstimated->setScale(0.4f);
        m_buttonMenu->addChildAtPosition(m_timeEstimated, Anchor::Center, CCPoint(0, 20));

        this->onSliderChange(nullptr);

        return true;
    }

    float delayPerObject() {
        float value = m_slider->getValue();
        return 0.001f + (0.05f * (1.0f - value));
    }

    void onSliderChange(CCObject* sender) {
        int estimatedTime = static_cast<int>(static_cast<float>(m_objects.getObjectCount()) * this->delayPerObject());
        m_timeEstimated->setString(("Estimated Time: " + std::to_string(estimatedTime) + "s").c_str());
    }

    void timelapseLevel(GJGameLevel* ogLevel) {
        auto level = GJGameLevel::create();
        level->m_levelString = m_objects.getNextObject() + ";";
        level->m_fastEditorZoom = 0.4f;
        level->m_lastCameraPos = CCPoint(35, 90);

        auto editor = LevelEditorLayer::scene(level, false);
        auto transitionFade = CCTransitionFade::create(0.5f, editor);
        CCDirector::sharedDirector()->replaceScene(transitionFade);

        static_cast<LTLevelEditorLayer*>(editor->getChildByType<LevelEditorLayer>(0))->initTimelapse(std::move(m_objects), this->delayPerObject());
    }

public:
    static LevelTimelapsePopup* create(GJGameLevel* level) {
        auto ret = new LevelTimelapsePopup();
        if (ret->initAnchored(240.f, 160.f, level)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};