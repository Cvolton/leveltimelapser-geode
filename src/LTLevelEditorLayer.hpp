#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

struct LTLevelEditorLayer : public geode::Modify<class LTLevelEditorLayer, LevelEditorLayer> {
    struct Fields {
        std::vector<std::string> m_timelapseObjects;
        size_t m_timelapseIndex = 1;
    };

    void initTimelapse(const std::vector<std::string>& objects);
    void beginTimelapse(float dt);
    void updateTimelapse(float dt);
};