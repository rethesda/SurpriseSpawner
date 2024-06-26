#include "Cache.h"
#include "Events.h"
#include "Logging.h"
#include "SKSE/Interfaces.h"
#include "Settings.h"

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        auto settings = Settings::GetSingleton();
        settings->LoadSettings();
        settings->LoadForms();
        auto manager = Events::LootActivateEvent::GetSingleton();
        manager->RegisterActivateEvents();
        auto menuManager = Events::MenuEvent::GetSingleton();
        menuManager->RegisterMenuEvents();
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    InitializeLogging();

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto version{ plugin->GetVersion() };

    logger::info("{} {} is loading...", plugin->GetName(), version);
    Init(skse);
    Cache::CacheAddLibAddresses();

    if (const auto messaging{ SKSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener))
        return false;

    logger::info("{} has finished loading.", plugin->GetName());

    return true;
}
