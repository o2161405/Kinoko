#include <egg/core/ExpHeap.hh>
#include "host/SceneCreatorDynamic.hh"
#include "game/system/KPadDirector.hh"
#include "game/kart/KartObjectManager.hh"
#include <egg/core/SceneManager.hh>
#include <game/kart/KartObject.hh>
#include <game/system/RaceConfig.hh>

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

    const EGG::Vector3f &getPosition() const;
    const EGG::Quatf &getRotation() const;
    const EGG::Vector3f &getInternalVelocity() const;
    const EGG::Vector3f &getExternalVelocity() const;

private:
    KGameInstance(Host::SceneCreatorDynamic *creator);
    const Kart::KartObject *m_player;
    System::KPadHostController *m_controller;
};
