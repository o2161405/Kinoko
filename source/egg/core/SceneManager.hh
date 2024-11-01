#pragma once

#include "egg/core/Scene.hh"
#include "egg/core/SceneCreator.hh"

namespace EGG {

/// @brief Manages the scene stack and transitions between scenes.
class SceneManager {
public:
    /*------------*
        Lifecycle
     *------------*/

    SceneManager(SceneCreator *creator = nullptr);
    virtual ~SceneManager();

    /*----------*
        Virtual
     *----------*/

    virtual void calc();
    virtual void calcCurrentScene();

    /*----------*
        Methods
     *----------*/

    void createChildScene(int nextSceneId, Scene *parent);
    void createScene(int nextSceneId, Scene *parent);
    void destroyScene(Scene *scene);

    void changeScene(int nextSceneId);
    void changeSiblingScene(int nextSceneId);
    void changeSiblingScene();
    void incomingCurrentScene();
    void outgoingParentScene(Scene *parent);
    void reinitCurrentScene();

    bool destroyCurrentSceneNoIncoming(bool destroyRootIfNoParent);
    bool destroyToSelectSceneId(int nextSceneId);
    [[nodiscard]] Scene *findParentScene(int id) const;
    void setupNextSceneId();
    void initMemory();

    /*----------*
        Getters
     *----------*/

    [[nodiscard]] Scene *currentScene() const {
        return m_currentScene;
    }

    [[nodiscard]] int currentSceneId() const {
        return m_currentSceneId;
    }

    [[nodiscard]] static Heap *heapForCreateScene() {
        return s_heapForCreateScene;
    }

    /*----------*
        Setters
     *----------*/

    void setNextSceneId(int id) {
        m_nextSceneId = id;
    }

private:
    /*----------*
        Members
     *----------*/

    SceneCreator *m_creator;
    Scene *m_currentScene;
    int m_nextSceneId;
    int m_currentSceneId;
    int m_prevSceneId;

    static Heap *s_heapForCreateScene;
    static u16 s_heapOptionFlg;

    void *m_memorySpace;
    EGG::Heap *m_rootHeap;
};

} // namespace EGG
