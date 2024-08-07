#pragma once
#include "Cache.h"
#include "Settings.h"

class Utility : public Singleton<Utility>
{
public:
    bool ExceptionName(std::string exception_name)
    {
        auto                     settings   = Settings::GetSingleton();
        std::vector<std::string> exceptions = settings->JSONSettings["Names"];

        if (std::count(exceptions.begin(), exceptions.end(), exception_name)) {
            logger::debug("{} is a restricted name", exception_name);
            return true;
        }
        else {
            return false;
        }
    };

    // Editor IDs appear to not be loaded, even with PO3 Tweaks, I'll maybe revisit this idea at some point

    /*bool isRestrictedContainer(std::string a_contEDID)
    {
        auto                     settings   = Settings::GetSingleton();
        std::vector<std::string> exceptions

     * * =
     * settings->JSONSettings["ContainerIDs"];        

        if (std::count(exceptions.begin(), exceptions.end(), a_contEDID.c_str())) {
 logger::debug("{}
     * is
     * a
     * restricted container", a_contEDID);
            return true;
        }
        else {
            logger::debug("{} is not restricted", a_contEDID);
 return

     *
     * * false;
        }
    }

    bool isRestrictedNameOrID(std::string a_name, std::string a_contEDID)
    {
        if (ExceptionName(a_name)) {
            return true;
 }


     * * if
     * (isRestrictedContainer(a_contEDID)) {
            return true;
        }
        else {
            return false;
        }
    }*/

    bool isRestrictedCell()
    {
        auto                     settings   = Settings::GetSingleton();
        std::vector<std::string> exceptions = settings->JSONSettings["Cells"];
        RE::PlayerCharacter*     player     = Cache::GetPlayerSingleton();
        auto                     EDID       = player->GetParentCell()->GetFormEditorID();

        if (std::count(exceptions.begin(), exceptions.end(), EDID)) {
            logger::debug("{} is a restricted Cell", player->GetParentCell()->GetFormEditorID());
            return true;
        }
        else {
            return false;
        }
    };

    bool isRestrictedLoc()
    {
        auto                     settings   = Settings::GetSingleton();
        std::vector<std::string> exceptions = settings->JSONSettings["LocationKeys"];
        RE::PlayerCharacter*     player     = Cache::GetPlayerSingleton();

        if (player->GetCurrentLocation() != nullptr) {
            logger::debug("location is {}", player->GetParentCell()->GetFormEditorID());
            return player->GetCurrentLocation()->HasAnyKeywordByEditorID(exceptions);
        }
        else {
            logger::debug("no location found");
            return false;
        }
    }

    bool isAnyException()
    {
        if (isRestrictedCell()) {
            logger::debug("restricted cell");
            return true;
        }
        if (isRestrictedLoc()) {
            logger::debug("restricted location type keyword");
            return true;
        }
        else
            return false;
    }

    inline int MinChance()
    {
        const auto settings = Settings::GetSingleton();
        if (settings->useChanceGlobal) {
            if (settings->compareValue < settings->MinChanceGlobal->value) {
                settings->compareValue = static_cast<int>(settings->MaxChanceGlobal->value);
            }
            return static_cast<int>(settings->MinChanceGlobal->value);
        }
        else
            return settings->minNumber;
    }

    inline int MaxChance()
    {
        const auto settings = Settings::GetSingleton();
        if (settings->useChanceGlobal) {
            if (settings->compareValue > settings->MaxChanceGlobal->value) {
                settings->compareValue = static_cast<int>(settings->MaxChanceGlobal->value);
            }
            return static_cast<int>(settings->MaxChanceGlobal->value);
        }
        else
            return settings->maxNumber;
    }

    inline int GetRandomChance(int a_min, int a_max)
    {
        static std::random_device       rd;
        static std::mt19937             gen(rd());
        std::uniform_int_distribution<> distrib(a_min, a_max);
        logger::debug("random number is {} between {} and {}", distrib(gen), a_min, a_max);
        return distrib(gen);
    }

    inline static std::chrono::duration<double> GetTimer()
    {
        auto settings = Settings::GetSingleton();
        if (settings->useDelayRange) {
            auto delay = settings->GetRandomDouble(settings->minTime, settings->maxTime);
            logger::debug("random time delay is {}", delay);
            settings->thread_delay = std::chrono::duration<double>(delay);
            return settings->thread_delay;
        }
        else {
            settings->thread_delay = std::chrono::duration<double>(settings->delay_timer);
            return settings->thread_delay;
        }
    }

    void RemoveAllItems(RE::TESObjectREFR* a_refToRemoveFrom, RE::TESObjectREFR* a_refToGiveItems)
    {
        auto inv_map = a_refToRemoveFrom->GetHandle().get()->GetInventoryCounts();
        for (auto& items : inv_map) {
            if (items.first->GetFormType() != RE::FormType::LeveledItem) {
                a_refToRemoveFrom->GetHandle().get()->RemoveItem(items.first, items.second, RE::ITEM_REMOVE_REASON::kRemove, nullptr, a_refToGiveItems);
                logger::debug("removed {}", items.first->GetName());
            }
            else
                return;
        }
    }

    void PlayMeme(RE::BGSSoundDescriptorForm* sound)
    {
        auto                 settings = Settings::GetSingleton();
        RE::BSSoundHandle    handle;
        auto                 am = RE::BSAudioManager::GetSingleton();
        RE::PlayerCharacter* p  = Cache::GetPlayerSingleton();
        if (settings->toggle_meme_sound) {
            am->BuildSoundDataFromDescriptor(handle, sound->soundDescriptor);
            handle.SetVolume(1.5f);
            handle.SetObjectToFollow(p->Get3D());
            handle.Play();
        }
        else
            return;
    }

    bool LocationCheck(std::string_view locKeyword)
    {
        RE::PlayerCharacter* player = Cache::GetPlayerSingleton();

        if (player->GetCurrentLocation() != nullptr) {
            return player->GetCurrentLocation()->HasKeywordString(locKeyword);
            // logger::debug("current location is: {}", player->GetCurrentLocation()->GetName());
        }
        else {
            logger::debug("no location found");
            return false;
        }
    }

    bool LocPlayerOwned()
    {
        RE::PlayerCharacter* player = Cache::GetPlayerSingleton();

        if (player->GetCurrentLocation() != nullptr) {
            if (player->GetCurrentLocation()->HasKeywordString("LocTypePlayerHouse")) {
                // logger::debug("current location is: {}", player->GetCurrentLocation()->GetName());
                return true;
            }
            else
                return false;
        }
        else {
            // logger::debug("current location is: {}", player->GetCurrentLocation()->GetName());
            return false;
        }
    }

    static bool IsPermanent(RE::MagicItem* item)
    {
        switch (item->GetSpellType()) {
        case RE::MagicSystem::SpellType::kDisease:
        case RE::MagicSystem::SpellType::kAbility:
        case RE::MagicSystem::SpellType::kAddiction: {
            return true;
        }
        default: {
            return false;
        }
        }
    }

    inline static void ApplySpell(RE::Actor* caster, RE::Actor* target, RE::SpellItem* spell)
    {
        if (IsPermanent(spell)) {
            target->AddSpell(spell);
        }
        else {
            caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->CastSpellImmediate(spell, false, target, 1.0F, false, 0.0F, nullptr);
        }
    }

    inline static void ApplyStress(RE::Actor* target)
    {
        RE::PlayerCharacter* player   = Cache::GetPlayerSingleton();
        auto                 settings = Settings::GetSingleton();
        Utility::ApplySpell(player, target, settings->StressSpell);
        logger::debug("applied {} to {}", settings->StressSpell->GetName(), target->AsReference()->GetName());
    }
};
