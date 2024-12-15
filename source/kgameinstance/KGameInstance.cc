#include "kgameinstance/KGameInstance.hh"

struct KRaceParams {
    Course course;
    Character character;
    Vehicle vehicle;
    bool driftIsAuto;
};

KRaceParams g_raceParams = {Course::SNES_Mario_Circuit_3, Character::Daisy, Vehicle::Mach_Bike, false};

static void *s_memorySpace = nullptr;

KGameInstance::KGameInstance(Host::SceneCreatorDynamic *creator) : EGG::SceneManager(creator) {}
KGameInstance::~KGameInstance() = default;

void KGameInstance::OnInit(System::RaceConfig *config, void *arg) {
    ASSERT(arg);
    const KRaceParams *params = reinterpret_cast<KRaceParams *>(arg);

    config->raceScenario().course = params->course;
    auto &player = config->raceScenario().players[0];
    player.character = params->character;
    player.vehicle = params->vehicle;
    player.driftIsAuto = params->driftIsAuto;

    // Setting the player's type to Local is required
    player.type = System::RaceConfig::Player::Type::Local;
}


void KGameInstance::init() {
    // If all of Kinoko is linked, the root heap must be created first
    // Assume that s_memorySpace is type void *, and s_rootHeap is type EGG::Heap *
    constexpr size_t MEMORY_SPACE_SIZE = 0x1000000;

    // You can alternatively pass 0 instead of an opt
    Abstract::Memory::MEMiHeapHead::OptFlag opt;
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::ZeroFillAlloc);

#ifdef BUILD_DEBUG
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::DebugFillAlloc);
#endif

    s_memorySpace = malloc(MEMORY_SPACE_SIZE);
    m_rootHeap = EGG::ExpHeap::create(s_memorySpace, MEMORY_SPACE_SIZE, opt);
    m_rootHeap->setName("EGGRoot");
    m_rootHeap->becomeCurrentHeap();

    System::RaceConfig::RegisterInitCallback(OnInit, &g_raceParams);

    // Creates the root scene, which creates the race scene
    // WARN: This will only initialize correctly because of the earlier callback registration!
    changeScene(0);

    // Initializes the pointers for later use
    // WARN: These are only guaranteed because of the earlier changeScene call!
    m_player = Kart::KartObjectManager::Instance()->object(0);
    m_controller = System::KPadDirector::Instance()->hostController();
}

void KGameInstance::calcInput(bool accelerate_, bool brake_, bool item_, bool drift_,
    int rawStickX_, int rawStickY_, int trick_) {

    bool accelerate = accelerate_;
    bool brake = brake_;
    bool item = item_;
    bool drift = drift_;

    s8 rawStickX = static_cast<s8>(rawStickX_);
    s8 rawStickY = static_cast<s8>(rawStickY_);
    System::Trick trick = static_cast<System::Trick>(trick_);

    u16 buttons = (accelerate) | (brake << 1) | (item << 2) | (drift << 3);
    ASSERT(m_controller->setInputsRawStickZeroCenter(buttons, rawStickX, rawStickY, trick));
}

void KGameInstance::calc() {
    calcCurrentScene();
}
