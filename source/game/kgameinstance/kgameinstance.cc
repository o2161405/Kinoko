#include "game/kgameinstance/kgameinstance.hh"

struct KRaceParams {
    Course course;
    Character character;
    Vehicle vehicle;
    bool driftIsAuto;
};

KRaceParams g_raceParams = {Course::SNES_Mario_Circuit_3, Character::Daisy, Vehicle::Mach_Bike, false};
static void *s_memorySpace = nullptr;

namespace {
    template<typename T>
    T clamp(T value, T min, T max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
}

KGameInstance::KGameInstance(Host::SceneCreatorDynamic *creator)
    : EGG::SceneManager(creator), m_player(nullptr), m_controller(nullptr), m_rootHeap(nullptr) {}

KGameInstance::~KGameInstance() = default;

void KGameInstance::OnInit(System::RaceConfig *config, void *arg) {
    ASSERT(arg);
    const KRaceParams *params = reinterpret_cast<KRaceParams *>(arg);

    config->raceScenario().course = params->course;
    auto &player = config->raceScenario().players[0];
    player.character = params->character;
    player.vehicle = params->vehicle;
    player.driftIsAuto = params->driftIsAuto;
    player.type = System::RaceConfig::Player::Type::Local;
}

void KGameInstance::init() {
    constexpr size_t MEMORY_SPACE_SIZE = 0x1000000;

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
    changeScene(0);
    m_player = Kart::KartObjectManager::Instance()->object(0);
    m_controller = System::KPadDirector::Instance()->hostController();
}

void KGameInstance::setInputState(const InputState& state) {
    m_currentInputState = state;
}

void KGameInstance::calc() {
    calcInput();
    calcCurrentScene();
}

void KGameInstance::calcInput() {
    u16 buttons = 0;
    buttons |= (m_currentInputState.accelerate ? 1 : 0);
    buttons |= (m_currentInputState.brake ? 1 : 0) << 1;
    buttons |= (m_currentInputState.item ? 1 : 0) << 2;
    buttons |= (m_currentInputState.drift ? 1 : 0) << 3;

    // Clamp stick values to valid range (-7 to 7)
    s8 stickX = static_cast<s8>(clamp(m_currentInputState.stickX, -7, 7));
    s8 stickY = static_cast<s8>(clamp(m_currentInputState.stickY, -7, 7));

    // Set the inputs on the controller
    ASSERT(m_controller->setInputsRawStickZeroCenter(buttons, stickX, stickY, m_currentInputState.trick));
}

const EGG::Vector3f &KGameInstance::getPosition() const {
    return m_player->pos();
}

const EGG::Quatf &KGameInstance::getRotation() const {
    return m_player->fullRot();
}

const EGG::Vector3f &KGameInstance::getInternalVelocity() const {
    return m_player->intVel();
}

const EGG::Vector3f &KGameInstance::getExternalVelocity() const {
    return m_player->extVel();
}