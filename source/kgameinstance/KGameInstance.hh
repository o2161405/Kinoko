#include <egg/core/SceneManager.hh>
#include <game/kart/KartObject.hh>
#include <game/system/RaceConfig.hh>
#include <egg/core/ExpHeap.hh>
#include "host/SceneCreatorDynamic.hh"
#include "game/system/KPadDirector.hh"
#include "game/kart/KartObjectManager.hh"

class KGameInstance final : public EGG::SceneManager {
public:
    static KGameInstance &Instance() {
        static Host::SceneCreatorDynamic creator;
        static KGameInstance instance(&creator);
        return instance;
    }
    ~KGameInstance() override;
    void calc() override;
    void init();
    static void OnInit(System::RaceConfig *config, void *arg);

    EGG::Heap* m_rootHeap;

    const EGG::Vector3f &getPosition() const {
        return m_player->pos();
    }

    const EGG::Quatf &getRotation() const {
        return m_player->fullRot();
    }

    const EGG::Vector3f &getInternalVelocity() const {
        return m_player->intVel();
    }

    const EGG::Vector3f &getExternalVelocity() const {
        return m_player->extVel();
    }

private:
    KGameInstance(Host::SceneCreatorDynamic *creator);
    const Kart::KartObject *m_player;
    System::KPadHostController *m_controller;
};
