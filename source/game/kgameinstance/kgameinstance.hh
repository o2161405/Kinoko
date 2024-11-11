#include <egg/core/ExpHeap.hh>
#include "host/SceneCreatorDynamic.hh"
#include "game/system/KPadDirector.hh"
#include "game/kart/KartObjectManager.hh"
#include <egg/core/SceneManager.hh>
#include <game/kart/KartObject.hh>
#include <game/system/RaceConfig.hh>

struct InputState {
    bool accelerate = false;
    bool brake = false;
    bool item = false;
    bool drift = false;
    int stickX = 0;  // Range: -7 to 7
    int stickY = 0;  // Range: -7 to 7
    System::Trick trick = System::Trick::None;
};

class KGameInstance final : public EGG::SceneManager {
public:
    ~KGameInstance() override;

    void calc() override;
    void init();
    static void OnInit(System::RaceConfig *config, void *arg);

    static KGameInstance &Instance() {
        static Host::SceneCreatorDynamic creator;
        static KGameInstance instance(&creator);
        return instance;
    }

    void setInputState(const InputState& state);

    EGG::Heap *getRootHeap() const {
        return m_rootHeap;
    }

    const EGG::Vector3f &getPosition() const;
    const EGG::Quatf &getRotation() const;
    const EGG::Vector3f &getInternalVelocity() const;
    const EGG::Vector3f &getExternalVelocity() const;

private:
    KGameInstance(Host::SceneCreatorDynamic *creator);
    void calcInput();
    
    const Kart::KartObject *m_player;
    System::KPadHostController *m_controller;
    EGG::Heap *m_rootHeap;
    InputState m_currentInputState;
};