
#include "builder.h"

#define MATCH(n) strcmp(name, n) == 0

*CFacility current_fac = nullptr; // Used only during ini file parsing.
*BuilderFacility current_builder_fac = nullptr; // Used only during ini file parsing.
char* current_file[40];


int BuilderFacility::add_prop(BuilderProperty prop) {
    switch(prop.type()) {
    case REQUIRE_FACILITY:
        require_facilities |= (1 << (prop.data[0] - 1));
        break;
    case INCOMPATIBLE_FACILITY:
        incompatible_facilities |= (1 << (prop.data[0] - 1));
        break;
    case REQUIRE_COAST:
        require_coast = 1;
        break;
    case IS_SMAX:
        is_smax = 1;
        break;
    case ALIEN_EXCLUSIVE:
        alien_exclusive = 1;
        break;
    case REQUIRE_POP_SIZE:
        require_pop_size = prop.data[0];
        break;
    case REQUIRE_TRANSCEND_VICTORY_POSSIBLE:
        require_transcend_victory_possible = 1;
        break;
    case REQUIRE_PROJECT_COMPLETED:
        require_project_completed = prop.data[0];
        break;
    case AI_HURRY_ECON_EAGER:
        ai_hurry_econ_eager = 1;
        break;
    case AI_TECH_VALUE_EARLY_ECO:
        ai_tech_value_early_eco = 1;
        break;
    case AI_RIOT_PREVENTION:
        ai_riot_prevention = 1;
        break;
    case DEFENDS_BASE:
        defends_base = 1;
        break;
    case PREQ_TECH_IS_CLIMACTIC:
        preq_tech_is_climactic = 1;
        break;
    case CLEAN_MINERALS:
        clean_minerals += prop.data[0];
        break;
    case NO_CAPTURE_DESTRUCTION:
        no_capture_destruction = 1;
        break;
    case FREE_TECH:
        free_techs += prop.data[0];
        break;
    case SCORCHED_EARTH:
        scorched_earth = 1;
        break;
    case IS_HQ:
        if (HQ_FACILITY != nullptr) {
            char msg[1024] = {};
            snprintf(msg, sizeof(msg),
                "Duplicate is_hq parameter found in %s.\n",
                current_file);
            MessageBoxA(0, msg, MOD_VERSION, MB_ICONWARNING);
            return 0;
        }
        // Properly set the global reference.
        HQ_FACILITY = this;
        // Fall-through is on purpose - this property needs to be saved to the base as well.
    default:
        properties[prop_count] = prop;
        prop_count++;
    }
    return 1;
}

bool BuilderFacility::is_free_mp(bool base_is_hq) {
    for (BuilderProperty prop: properties) {
        switch(prop.type()) {
        case FIRST_BASE_FREE_MP:
            if (base_is_hq)
                return true;
            continue;
        case FIRST_BASE_FREE_MP_DIFFICULTY:
            if (*DiffLevel >= 3)
                return true;
            continue;
        case FIRST_BASE_FREE_MP_SE_POSITIVE:
            switch(prop.data[1]) {
            case SE_ECONOMY:
                if (plr->SE_economy_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_EFFIC:
                if (plr->SE_effic_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_SUPPORT:
                if (plr->SE_support_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_TALENT:
                if (plr->SE_talent_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_MORALE:
                if (plr->SE_morale_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_POLICE:
                if (plr->SE_police_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_GROWTH:
                if (plr->SE_growth_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_PLANET:
                if (plr->SE_planet_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_PROBE:
                if (plr->SE_probe_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_INDUSTRY:
                if (plr->SE_industry_base > (int8_t)prop.data[0])
                    return true;
                continue;
            case SE_RESEARCH:
                if (plr->SE_research_base > (int8_t)prop.data[0])
                    return true;
            }
            continue;
        }
    }
    return false;
}

void BuilderBase::add_facility(BuilderFacility* facility) {
    if (has_fac_parsed(facility->facility_id)) {
        return;
    } else if (facility->facility_id != -1) { // Verify it's a real facility, instead of a "special" one.
        facilities_parsed |= 1 << (facility->facility_id - 1);
    }
    for (BuilderProperty prop: facility->properties) {
        if (prop.type() == NULL_PROPERTY)
            break;
        if (prop.type() > 0 && prop.type() <= BasePropertyLast)
            add_prop(prop);
    }
    // COUNTS_AS has a conditional field for filtering by facility - now that we have another facility, lets check them all.
    // If there's multiple that became valid either at once or in a chain, the recursion will find them all.
    // Note that the property contains the vanilla IDs, not builder Facility Ids. They're off by one.
    int fac_to_build = 0;
    for (int i = 0; i < 16; i++) {
        BuilderProperty prop = properties[i];
        if (prop.type() == NULL_PROPERTY) {
            break;
        } else if (prop.type() == COUNTS_AS && has_fac_parsed(prop.data[1] - 1)) {
            // Earlier requirement is now fulfilled.
            fac_to_build = prop.data[0] - 1;
        } else if (fac_to_build) {
            properties[i-1] = prop; // Move up the list into a new empty slot.
            prop.data[3] = NULL_PROPERTY; // "move up" implies the old one goes away
        }
    }
    if (fac_to_build) {
        add_facility(&builder_facilities[fac_to_build]);
    }
}

void BuilderBase::add_prop(BuilderProperty prop) {
    uint8_t data[3] = {prop.data[0], prop.data[1], prop.data[2]};
    const BuilderPropertyType type = prop.type();
    if (type <= NULL_PROPERTY || type > BasePropertyLast)
        return;
    switch(type) {
    case PREVENT_POP_LOSS:
        prevent_pop_loss = 1;
        return;
    case REPAIR:
        if (data[2] == TRIAD_LAND)
            repair_land = 1;
        else if (data[2] == TRIAD_SEA)
            repair_sea = 1;
        else if (data[2] == TRIAD_AIR)
            repair_air = 1;
        return;
    case TELEPORT:
        teleporter = 1;
        return;
    case GARRISON_IGNORE_NEGATIVE_SE_MORALE:
        if (data[0] & 1)
            garrison_ignore_negative_se_morale_native = 1;
        if (data[0] & 2)
            garrison_ignore_negative_se_morale_non_native = 1;
        return;
    case SENSOR:
        sensor = 1;
        return;
    case SUBMERSION:
        submersion = 1;
        return;
    case SUPPRESS_PSYCH:
        suppress_psych = 1;
        return;
    case FULL_SATELLITE_EFFECT:
        full_satellite_effect = 1;
        return;
    case CAN_BUILD_SATELLITE:
        can_build_satellite = 1;
        return;
    case IS_HQ:
        is_hq = 1;
        return;
    case LINK_ALIEN_ARTIFACT:
        link_alien_artifact = 1;
        return;
    case ALIEN_VICTORY:
        alien_victory = 1;
        return;
    case HANDLE_EVENT_GROWTH:
        handle_event_growth = 1;
        return;
    case HANDLE_EVENT_ENERGY:
        handle_event_energy = 1;
        return;
    case HANDLE_EVENT_NETBONUS:
        handle_event_netbonus = 1;
        return;
    case HANDLE_EVENT_BLIGHT:
        handle_event_blight = 1;
        return;
    case HANDLE_EVENT_PROMETHEUS:
        handle_event_prometheus = 1;
        return;
    case LINK_INFINITE_ARTIFACTS:
        link_infinite_artifacts = 1;
        return;
    case POP_BOOM:
        pop_boom = 1;
        return;
    case NUKE_TARGET:
        nuke_target = 1;
        return;
    case HALF_MAINTENANCE:
        half_maintenance = 1;
        return;
    case PREVENT_DRONE_RIOT:
        prevent_drone_riot = 1;
        return;
    case LONGEVITY_VACCINE_EFFECTS:
        is_longevity_vaccine = 1;
        return;

    case DEFENSE:
        if (data[2] == TRIAD_LAND)
            land_defense += ((uint16_t)data[1] << 8) + data[0];
        else if (data[2] == TRIAD_SEA)
            sea_defense += ((uint16_t)data[1] << 8) + data[0];
        else if (data[2] == TRIAD_AIR)
            air_defense += ((uint16_t)data[1] << 8) + data[0];
        return;
    case RESOURCE_PERCENT:
        nutrient_percent += (int8_t)data[0];
        mineral_percent += (int8_t)data[1];
        energy_percent += (int8_t)data[2];
        return;
    case CLP_COEFF:
        credits_percent += data[0];
        labs_percent += data[1];
        psych_percent += data[2];
        return;
    case PROTECT_RANGE_DROP_PODS:
        protect_range_drop_pod = clamp(data[0], protect_range_drop_pod, (uint8_t)8);
        return;
    case MISSILE_DEFENSE:
        protect_range_missile = clamp(data[2], protect_range_missile, (uint8_t)8);
        missile_defense = clamp(missile_defense + data[0], 0, 250);
        return;
    case NUKE_DEFENSE:
        protect_range_nuke = clamp(data[2], protect_range_nuke, (uint8_t)8);
        protect_chance_nuke = clamp(100 - ((100 - protect_chance_nuke) * (100 - data[0]) / 100), 0, 100);
        return;
    case POP_LIMIT:
        pop_limit = clamp(pop_limit + data[0], 0, MaxBasePopSize); // MaxBasePopSize = 127 by default.
        return;
    case ECO_DAMAGE_REDUCTION_TERRAFORM:
        eco_damage_reduction_terraform = clamp(data[0] + eco_damage_reduction_terraform, 0, 100);
        return;
    case ECO_DAMAGE_REDUCTION_MINERAL:
        eco_damage_reduction_mineral  = clamp(data[0] + eco_damage_reduction_mineral, 0, 255);
        return;
    case SE:
        switch((SocialEffect)data[2]) {
        case SE_ECONOMY:
            SE_economy = clamp(SE_economy + (int8_t)data[0], -10, 10);
            break;
        case SE_EFFIC:
            SE_efficiency = clamp(SE_efficiency + (int8_t)data[0], -10, 10);
            break;
        case SE_GROWTH:
            SE_growth = clamp(SE_growth + (int8_t)data[0], -10, 10);
            break;
        case SE_INDUSTRY:
            SE_industry = clamp(SE_industry + (int8_t)data[0], -10, 10);
            break;
        case SE_PLANET:
            SE_planet = clamp(SE_planet + (int8_t)data[0], -10, 10);
            break;
        case SE_POLICE:
            SE_police = clamp(SE_police + (int8_t)data[0], -10, 10);
            break;
        case SE_PROBE:
            SE_probe = clamp(SE_probe + (int8_t)data[0], -10, 10);
            break;
        case SE_SUPPORT:
            SE_support = clamp(SE_support + (int8_t)data[0], -10, 10);
            break;
        }
        return;
    case BASE_SQ_INCOME:
        base_sq_nutrients = clamp(data[0] + base_sq_nutrients, 0, 250);
        base_sq_minerals = clamp(data[1] + base_sq_minerals, 0, 250);
        base_sq_energy = clamp(data[2] + base_sq_energy, 0, 250);
        return;
    case FOREST_INCOME:
        forest_nutrients = clamp(data[0] + forest_nutrients, 0, 250);
        forest_minerals = clamp(data[1] + forest_minerals, 0, 250);
        forest_energy = clamp(data[2] + forest_energy, 0, 250);
        return;
    case IMPROVED_OCEAN_INCOME:
        improved_ocean_nutrients = clamp(data[0] + improved_ocean_nutrients, 0, 250);
        improved_ocean_minerals = clamp(data[1] + improved_ocean_minerals, 0, 250);
        improved_ocean_energy = clamp(data[2] + improved_ocean_energy, 0, 250);
        return;
    case IMPROVED_LAND_INCOME:
        improved_land_nutrients = clamp(data[0] + improved_land_nutrients, 0, 250);
        improved_land_minerals = clamp(data[1] + improved_land_minerals, 0, 250);
        improved_land_energy = clamp(data[2] + improved_land_energy, 0, 250);
        return;
    case FUNGUS_INCOME:
        fungus_nutrients = clamp(data[0] + fungus_nutrients, 0, 250);
        fungus_minerals = clamp(data[1] + fungus_minerals, 0, 250);
        fungus_energy = clamp(data[2] + fungus_energy, 0, 250);
        return;
    case SQUARE_INCOME: // except fungus
        square_nutrients = clamp(data[0] + square_nutrients, 0, 250);
        square_minerals = clamp(data[1] + square_minerals, 0, 250);
        square_energy = clamp(data[2] + square_energy, 0, 250);
        return;
    case CLP_INCOME:
        flat_credits = clamp(data[0] + flat_credits, 0, 250);
        flat_labs = clamp(data[1] + flat_labs, 0, 250);
        flat_psych = clamp(data[2] + flat_psych, 0, 250);
        return;
    case BASE_FINAL_LABS_MULT:
        final_labs_mult = clamp(final_labs_mult * data[0] / 100, 0, 250);
        return;
    case NME_CAP:
        yield_cap_nutrient = clamp(data[0] + yield_cap_nutrient, 0, 250);
        yield_cap_mineral = clamp(data[1] + yield_cap_mineral, 0, 250);
        yield_cap_energy = clamp(data[2] + yield_cap_energy, 0, 250);
        return;
    case BASE_SIZE_INDICATOR_BORDER_COLOR:
        base_pop_indicator_border_color = data[0];
        base_pop_indicator_border_thickness = data[1];
        return;
    case RETOOL_MAX_PENALTY_CLASS:
        retool_penalty = min(retool_penalty, data[0]);
        return;
    case PROTOTYPE_PENALTY_MULT:
        prototype_penalty_mult = clamp((prototype_penalty_mult * data[0] / 100), 0, 100);
        return;
    case PLANET_FUNG_BONUS:
        planet_fung_bonus[data[2]][data[1]] = clamp(planet_fung_bonus[data[2]][data[1]] + data[0], 0, 200);
        return;
    case VIRTUAL_POLICE:
        virtual_police = clamp(data[0] + virtual_police, 0, 250);
        return;
    case EFFICIENCY_FLOOR_FLAT:
        efficency_flat = clamp(data[0] + efficency_flat, 0, 250);
        return;
    case EFFICIENCY_FLOOR_PERCENT:
        efficency_percent = clamp(data[0] + efficency_percent, 0, 100);
        return;
    case MORALE_FLOOR:
        // Bits 9, 10, 11 and 12 indicate requiring native, non-native, defensive, and offensive specifically when set, and always valid when not.
        if (!(data[1] & 2) && !(data[1] & 4))
            garrison_morale_floor_offensive_native = clamp(garrison_morale_floor_offensive_native, data[0], (uint8_t)6);
        if (!(data[1] & 1) && !(data[1] & 4))
            garrison_morale_floor_offensive_non_native = clamp(garrison_morale_floor_offensive_non_native, data[0], (uint8_t)6);
        if (!(data[1] & 2) && !(data[1] & 8))
            garrison_morale_floor_defensive_native = clamp(garrison_morale_floor_defensive_native, data[0], (uint8_t)6);
        if (!(data[1] & 1) && !(data[1] & 8))
            garrison_morale_floor_defensive_non_native = clamp(garrison_morale_floor_defensive_non_native, data[0], (uint8_t)6);
        return;
    case MORALE_MOD:
        // Bits 9, 10, 11 and 12 indicate requiring native, non-native, defensive, and offensive specifically when set, and always valid when not.
        if (!(data[1] & 2) && !(data[1] & 4))
            garrison_morale_boost_offensive_native = clamp(garrison_morale_floor_offensive_native + data[0], 0, 6);
        if (!(data[1] & 1) && !(data[1] & 4))
            garrison_morale_boost_offensive_non_native = clamp(garrison_morale_boost_offensive_non_native + data[0], 0, 6);
        if (!(data[1] & 2) && !(data[1] & 8))
            garrison_morale_boost_defensive_native = clamp(garrison_morale_boost_defensive_native + data[0], 0, 6);
        if (!(data[1] & 1) && !(data[1] & 8))
            garrison_morale_boost_defensive_non_native = clamp(garrison_morale_boost_defensive_non_native + data[0], 0, 6);
        return;


    case PROBE_PLAGUE_RESISTANCE:
        probe_plague_resistance *= ((float)data[0] / (float)data[1]);
        return;
    case HOMED_UNIT_MIND_CONTROL_RESISTANCE:
        homed_unit_mc_resist *= ((float)data[0] / (float)data[1]);
        return;
    case MIND_CONTROL_RESISTANCE:
        mind_control_resistance *= ((float)data[0] / (float)data[1]);
        return;
    case MULTIPLICATIVE_CREDITS:
        credits_multiplicative *= ((float)data[0] / (float)data[1]);
        return;
    case MULTIPLICATIVE_LABS:
        labs_multiplicative *= ((float)data[0] / (float)data[1]);
        return;
    case MULTIPLICATIVE_PSYCH:
        psych_multiplicative *= ((float)data[0] / (float)data[1]);
        return;
    case SATELLITE_PROD_MULT:
        satellite_production_mult *= ((float)data[0] / (float)data[1]);
        return;
    case STOCKPILE_ENERGY_BONUS:
        stockpile_energy_bonus *= ((float)data[0] / (float)data[1]);
        return;

    case MORALE:
        if (data[2] == TRIAD_LAND)
            land_morale += data[0];
        else if (data[2] == TRIAD_SEA)
            sea_morale += data[0];
        else if (data[2] == TRIAD_AIR)
            air_morale += data[0];
        else if (data[2] == 3)
            lifecycle += data[0];
        else if (data[2] == 4)
            probe_morale += data[0];
        return;
    case DRONES:
        drones += (int8_t)data[0];
        return;
    case TALENTS:
        talents += (int8_t)data[0];
        return;
    case COUNTS_AS:
        if (!data[1] || has_fac_parsed(data[1])) { // data[1] optionally contains a requirement to activate the counts_as (used for the Virtual World)
            add_facility(&builder_facilities[data[0]]);
        } else {
            properties[property_count] = prop; // Save for later evaluation.
            property_count++;
        }
        return;

    default:
        if (type >= ComplexPropertyFirst && type <= ComplexPropertyLast) { // Complex properties need to be stored, to be calculated on demand.
            properties[property_count] = prop;
            property_count++;
        }
        return;
    }
}

void BuilderBase::income_from_facility_count(int* credits, int* labs, int* psych) {
    for (BuilderProperty prop: properties) {
        if (prop.type() == NULL_PROPERTY)
            break;
        if (prop.type() == FACILITY_COUNT) {
            int facility_id = prop.data[0];
            if (prop.data[1] == 0) {
                for (int i = 0; i < *BaseCount; i++) {
                    if (has_fac_built((FacilityId)facility_id, i)) {
                        labs++;
                    }
                }
            } else if (prop.data[1] == 1) {
                for (int i = 0; i < *BaseCount; i++) {
                    if (has_fac_built((FacilityId)facility_id, i)) {
                        credits++;
                    }
                }
            } else if (prop.data[1] == 2) {
                for (int i = 0; i < *BaseCount; i++) {
                    if (has_fac_built((FacilityId)facility_id, i)) {
                        psych++;
                    }
                }
            }
        }
    }
}

int BuilderBase::cost_calc(int cost, int triad_or_native_or_probe) {
    // triad: 0=LAND, 1=SEA, 2=AIR, 3=NATIVE, 4=PROBE
    for (BuilderProperty prop: properties) {
        if (prop.type() == NULL_PROPERTY)
            break;
        if (prop.type() != COST_MULT)
            continue;
        if (prop.data[2] && prop.data[2] != triad_or_native_or_probe)
            continue;
        cost = (cost * prop.data[0] + 99) / 100;
    }
    return cost;
}

bool BuilderBase::check_available(int facility_id) {
    BuilderFacility* fac = &builder_facilities[facility_id];
    for (BuilderProperty prop: fac->properties) {
        if (prop.type() == REQUIRE_FACILITY) {
            if (has_fac_parsed(prop.data[0]))
                continue;
            bool satisfied = false;
            for (BuilderProperty our_prop: properties) {
                if (our_prop.type() == PREQ_COUNT_AS && prop.data[0] == our_prop.data[0])
                    satisfied = 1;
            }
            if (!satisfied)
                return false;
        }
        else if (prop.type() == INCOMPATIBLE_FACILITY) {
            if (has_fac_parsed(prop.data[0]))
                return false;
            for (BuilderProperty our_prop: properties) {
                if (our_prop.type() == PREQ_COUNT_AS && prop.data[0] == our_prop.data[0])
                    return false;
            }
        }
    }
    return true;
}

int BuilderFaction::terraform_rate(int rate, FormerItem terraform) { // Returns multiplier
    assert(terraform >= 0 && terraform <= 18);  // -1 is Thinker specific and makes no sense in this context; 19 is FORMER_MONOLITH, which isn't normally available and which we have no bits to spare for.
    int bit = 1 << (terraform + 5); // Right-most 5 bits are the terraform rate, in 10%.
    rate *= 10;
    for (BuilderProperty prop: properties) {
        if (prop.type() == NULL_PROPERTY)
            break;
        if (prop.type() == TERRAFORM_RATE) {
            uint32_t data = (prop.data[2] << 16) + (prop.data[1] << 8) + prop.data[0];
            if (data & bit)
                rate += (prop.data[0] & 0b00011111);

        }
    }
    return rate / 10;
}

void BuilderFaction::add_project_property(BuilderProperty prop) {
    const uint8_t data[3] = {prop.data[0], prop.data[1], prop.data[3]};
    const BuilderPropertyType type = prop.type();
    if (type <= NULL_PROPERTY || type > BasePropertyLast)
        return;
    switch(type) {
    case ACTIVE_PROBE_MORALE:
        active_probe_morale = clamp(data[0] + active_probe_morale, 0, 6);
        return;
    case TERRAFORM_UNLOCK:
        terraform_unlock |= (data[2] << 16) + (data[1] << 8) + data[0];
        return;
    case COUNCIL_VOTES_MODIFIER:
        council_votes_mod *= (float)data[0] / (float)data[1];
        return;
    case COUNTS_AS_INFILTRATION:
        infiltration = 1;
        return;
    case FUNGUS_REPAIR_AS_NATIVE:
        fungus_repair_as_native = 1;
        return;
    case FUNGUS_COMBAT_AS_NATIVE:
        fungus_combat_as_native = 1;
        return;
    case FUNGUS_MOVE_AS_NATIVE:
        fungus_move_as_native = 1;
        return;
    case PSI_DEFENSE:
        psi_defense = clamp(data[0] + psi_defense, 0, 250);
        return;
    case PSI_ATTACK:
        psi_attack = clamp(data[0] + psi_attack, 0, 250);
        return;
    case UNIT_SPEED:
        switch(data[1]) {
        case TRIAD_LAND: // IIRC this isn't entirely functional to go all the way to 250.
            land_speed = clamp(data[0] + land_speed, 0, 250);
            return;
        case TRIAD_SEA:
            sea_speed = clamp(data[0] + sea_speed, 0, 250);
            return;
        case TRIAD_AIR:
            air_speed = clamp(data[0] + air_speed, 0, 250);
            return;
        default:
            return;
        };
    case TECH_SHARE:
        tech_share = min(tech_share, data[0]);
        return;
    case SE_FACTION:
        switch((SocialEffect)data[1]) {
        case SE_ECONOMY:
            se_economy = clamp((int8_t)data[0] + se_economy, -10, 10);
            return;
        case SE_EFFIC:
            se_efficiency = clamp((int8_t)data[0] + se_efficiency, -10, 10);
            return;
        case SE_SUPPORT:
            se_support = clamp((int8_t)data[0] + se_support, -10, 10);
            return;
        case SE_TALENT:
            se_talent = clamp((int8_t)data[0] + se_talent, -10, 10);
            return;
        case SE_MORALE:
            se_morale = clamp((int8_t)data[0] + se_morale, -10, 10);
            return;
        case SE_POLICE:
            se_police = clamp((int8_t)data[0] + se_police, -10, 10);
            return;
        case SE_GROWTH:
            se_growth = clamp((int8_t)data[0] + se_growth, -10, 10);
            return;
        case SE_PLANET:
            se_planet = clamp((int8_t)data[0] + se_planet, -10, 10);
            return;
        case SE_PROBE:
            se_probe = clamp((int8_t)data[0] + se_probe, -10, 10);
            return;
        case SE_INDUSTRY:
            se_industry = clamp((int8_t)data[0] + se_industry, -10, 10);
            return;
        case SE_RESEARCH:
            se_research = clamp((int8_t)data[0] + se_research, -10, 10);
            return;
        default:
            return;
            };
    case PROBE_IMMUNITY:
        probe_immunity = max(probe_immunity, data[0]);
        return;
    case REPAIR_SPEED:
        repair_speed = clamp(data[0] + repair_speed, 0, 10);
        return;
    case LIFT_REPAIR_LIMIT:
        lift_repair_limit = 1;
        return;
    case UNIT_UPGRADE_COST_MULT:
        unit_upgrade_cost_mult *= (float)data[0]/(float)data[1];
        return;
    case IMPUNITY:
        impunity |= ((uint16_t)data[1] << 8) + data[0];
        return;
    case POP_FLOOR:
        pop_floor += data[0];
        return;
    case DRONES_UNDER_POP_FLOOR:
        drones_while_pop_floor += (int8_t)data[0];
        return;
    case ORBITAL_INSERTION:
        orbital_insertion = 1;
        return;
    case MIND_CONTROL_MULTIPLIER:
        mind_control_multiplier *= (float)data[0] / (float)data[1];
        return;
    case ALL_PROBES_ALGORITHMIC_ENHANCEMENT:
        algorithmic_enhancement = 1;
        return;
    case LONGEVITY_VACCINE_EFFECTS:
        longevity_vaccine = 1;
        return;
    case VOICE_OF_PLANET_EFFECTS:
        voice_of_planet = 1;
        return;
    case TERRAFORM_RATE:
        // We only have 1 complex property here at the moment.
        properties[property_count] = prop;
        property_count++;
        return;
    default:;
    };
}

void rebuild_builder_base(int base_id) {
    BASE* b = &Bases[base_id];
    BuilderBase* base = &builder_bases[base_id];
    builder_bases[base_id] = builder_base_templates[b->faction_id];
    for (int c = 0; c < MaxFacilityNum; c++) {
        if (b->has_fac_built((FacilityId)(c + 1)))
            base->add_facility(&builder_facilities[c]);
    }
}

void reset_builder_factions() {
    // Invocation only expected on game start/load.
    BuilderBase* base;
    BuilderBase* base_template;
    BuilderFaction* faction;
    BuilderFacility* project;
    BuilderProperty tmp;
    for (int i = 1; i < MaxPlayerNum; i++) {
        builder_factions[i] = builder_factions[0];
        builder_factions[i].faction_id = i;
        builder_base_templates[i] = builder_base_templates[0];
        builder_base_templates[i].faction_id = i;
        builder_base_templates[i].add_facility(&special_facilities[0]);  // Add the "everyone always has this facility" facility.
        builder_base_templates[i].add_facility(&special_facilities[i]);  // Add the "faction always has this facility" facility.
    };
    for (int i = 0; i <= SP_ID_Last - SP_ID_First; i++) {
        if (SecretProjects[i] < 0) {
            continue; // Project not built.
        };
        project = &builder_projects[i];
        base = &builder_bases[SecretProjects[i]];
        base_template = &builder_base_templates[base->faction_id];
        faction = &builder_factions[base->faction_id];
        for (BuilderProperty prop: project->properties) {
            if (prop.type() == NULL_PROPERTY) {
                break;
            } else if (prop.type() >= FacilityTypeGlobalFirst && prop.type() < FacilityTypeGlobalUniqueFirst){
                tmp = prop;
                tmp.data[3] -= GlobalPropertyTypeOffset;  // Transform Global prop into non-global prop.
                base_template->add_prop(tmp);
            } else if (prop.type() >= FacilityTypeGlobalUniqueFirst && prop.type() <= FacilityTypeGlobalLast) {
                faction->add_project_property(prop);
            }
        }
    }
    for (int i = 0; i < *BaseCount; i++) {
        rebuild_builder_base(SecretProjects[i]);
    }
    // Now that we've used the templates, add the project properties:
    for (int i = 0; i <= SP_ID_Last - SP_ID_First; i++) {
        if (SecretProjects[i] < 0) {
            continue;
        };
        project = &builder_projects[i];
        base = &builder_bases[SecretProjects[i]];
        for (BuilderProperty prop: project->properties) {
            if (prop.type() == NULL_PROPERTY) {
                break;
            } else if (prop.type() > NULL_PROPERTY && prop.type() <= BasePropertyLast){
                base->add_prop(prop);
            } else if (prop.type() == LONGEVITY_VACCINE_EFFECTS) { // Special casing because I'm not generalizing that.
                base->add_prop(prop);
            }
        }
    }
}

void reset_builder_faction(int faction_id) {
    // Invocation expected on losing any secret projects previously owned.
    BuilderBase* base_template = &builder_base_templates[faction_id];
    BuilderFaction* faction = &builder_factions[faction_id];
    BuilderBase* base;
    BuilderFacility* project;
    BuilderProperty tmp;
    *faction = builder_factions[0];
    *base_template = builder_base_templates[0];
    base_template->add_facility(&special_facilities[0]);
    base_template->add_facility(&special_facilities[faction_id]);

    for (int i = 0; i <= SP_ID_Last - SP_ID_First; i++) {
        if (SecretProjects[i] < 0) {
            continue; // Project not built.
        };
        base = &builder_bases[SecretProjects[i]];
        if (base->faction_id != faction_id)
            continue;
        project = &builder_projects[i];
        for (BuilderProperty prop: project->properties) {
            if (prop.type() == NULL_PROPERTY) {
                break; // We've had all properties.
            } else if (prop.type() >= FacilityTypeGlobalFirst && prop.type() < FacilityTypeGlobalUniqueFirst){
                tmp = prop;
                tmp.data[3] -= GlobalPropertyTypeOffset;  // Transform Global prop into non-global prop.
                base_template->add_prop(tmp);
            } else if (prop.type() >= FacilityTypeGlobalUniqueFirst && prop.type() <= FacilityTypeGlobalLast) {
                faction->add_project_property(prop);
            }
        }
    }

    for (int i = 0; i < *BaseCount; i++) {
        if (builder_bases[SecretProjects[i]].faction_id == faction_id)
            rebuild_builder_base(SecretProjects[i]);
    }

    for (int i = 0; i <= SP_ID_Last - SP_ID_First; i++) {
        if (SecretProjects[i] < 0) {
            continue;
        };
        project = &builder_projects[i];
        base = &builder_bases[SecretProjects[i]];
        if (base->faction_id != faction_id)
            continue;
        for (BuilderProperty prop: project->properties) {
            if (prop.type() == NULL_PROPERTY) {
                break;
            } else if (prop.type() > NULL_PROPERTY && prop.type() <= BasePropertyLast){
                base->add_prop(prop);
            }
        }
    }
}

void delete_builder_base(int base_id) {
    // Mostly copied from mod_base_kill()
    if (base_id < *BaseCount - 1) {
        memmove(&builder_bases[base_id], &builder_bases[base_id + 1], (*BaseCount - base_id - 1) * sizeof(BuilderBase));
    }
}

BuilderBase* init_builder_base(int base_id) {
    BuilderBase* base = &builder_bases[base_id];
    BASE* b = &Bases[base_id];
    *base = builder_base_templates[b->faction_id];
    // Potential edit: Process CLEAN_MINERALS for pre-built facilities?
    return base;
}

void construct_facility(int base_id, int facility_id) {
    BuilderBase* base = &builder_bases[base_id];
    BuilderBase* current;
    BuilderFacility* facility;
    BuilderFaction* builder_faction;
    int faction_id = base->faction_id;

    if (facility_id < SP_ID_First) {
        facility = &builder_facilities[facility_id - 1];
    } else {
        facility = &builder_projects[facility_id - SP_ID_First];
        builder_faction = &builder_factions[faction_id];
        for (BuilderProperty prop: facility->properties) {
            if (prop.type() == NULL_PROPERTY) {
                break;
            } else if (prop.type() >= FacilityTypeGlobalFirst && prop.type() < FacilityTypeGlobalUniqueFirst){
                BuilderProperty tmp = prop;
                tmp.data[3] -= GlobalPropertyTypeOffset;  // Transform Global prop into non-global prop.
                builder_base_templates[faction_id].add_prop(tmp);
                for (int i = 0; i < *BaseCount; i++) {
                    current = &builder_bases[i];
                    if (current->faction_id == faction_id)
                        current->add_prop(tmp);
                }
            } else if (prop.type() >= FacilityTypeGlobalUniqueFirst && prop.type() <= FacilityTypeGlobalLast) {
                builder_faction->add_project_property(prop);
            }
        }
    }
    base->add_facility(facility); // For projects, this just adds the local stuff.
}

/*
Builder mod initialization.
1. Read config files
*/
void builder_init() {

}

// Option handlers
int builder_opt_handler(void* user, const char* section, const char* name, const char* value) {
    char buf[INI_MAX_LINE];
    // Make sure we're changing the correct facility
    if (!current_fac || !MATCH(current_fac->name, section)) {
        current_builder_fac = nullptr;
        for (int i = 1; i <= MaxFacilityNum; i++) {
            if (MATCH(Facility[i].name, section)) {
                current_fac = &Facility[i];
                current_builder_fac = &builder_facilities[i-1]; // Builder starts at 0, base game at 1.
                break;
            }
        }
    }
    if (!current_builder_fac) {
        char msg[1024] = {};
        snprintf(msg, sizeof(msg),
            "Unknown facility name '%s' found in %s.\n",
            section, current_file);
        MessageBoxA(0, msg, MOD_VERSION, MB_ICONWARNING);
        return 0;
    }
    strcpy_n(buf, INI_MAX_LINE, value);
    if (MATCH("base_prevent_pop_loss")) {
        return b_ini_parse_boolean(PREVENT_POP_LOSS);
    } else if (MATCH("global_base_prevent_pop_loss")) {
        return b_ini_parse_boolean(GLOBAL_PREVENT_POP_LOSS);
    } else if (MATCH("base_defense")) {
        return b_ini_parse_defense(value);
    } else if (MATCH("global_base_defense")) {
        return b_ini_parse_defense(value, true);
    } else if (MATCH("base_morale_floor")) {
        return b_ini_parse_garrison_morale(MORALE_FLOOR, value);
    } else if (MATCH("global_base_morale_floor")) {
        return b_ini_parse_garrison_morale(GLOBAL_MORALE_FLOOR, value);
    } else if (MATCH("garrison_morale_mod")) {
        return b_ini_parse_garrison_morale(MORALE_MOD, value);
    } else if (MATCH("global_garrison_morale_mod")) {
        return b_ini_parse_garrison_morale(GLOBAL_MORALE_MOD, value);
    } else if (MATCH("morale")) {
        return b_ini_parse_morale(value);
    } else if (MATCH("global_morale")) {
        return b_ini_parse_morale(value, true);
    } else if (MATCH("probe_plague_resistance")) {
        return b_ini_parse_two_integer(PROBE_PLAGUE_RESISTANCE, value, 1);
    } else if (MATCH("global_probe_plague_resistance")) {
        return b_ini_parse_two_integer(GLOBAL_PROBE_PLAGUE_RESISTANCE, value, 1);
    } else if (MATCH("remote_unit_mind_control_resistance")) {
        return b_ini_parse_two_integer(HOMED_UNIT_MIND_CONTROL_RESISTANCE, value, 1);
    } else if (MATCH("global_remote_unit_mind_control_resistance")) {
        return b_ini_parse_two_integer(GLOBAL_HOMED_UNIT_MIND_CONTROL_RESISTANCE, value, 1);
    } else if (MATCH("repair")) {
        return b_ini_parse_repair(value);
    } else if (MATCH("global_repair")) {
        return b_ini_parse_repair(value, true);
    } else if (MATCH("prevent_drop_pod_range")) {
        return b_ini_parse_integer(PROTECT_RANGE_DROP_PODS, value, 0, 8);
    } else if (MATCH("global_prevent_drop_pod_range")) {
        return b_ini_parse_integer(GLOBAL_PROTECT_RANGE_DROP_PODS, value, 0, 8);
    } else if (MATCH("is_teleporter")) {
        return b_ini_parse_boolean(TELEPORT);
    } else if (MATCH("global_is_teleporter")) {
        return b_ini_parse_boolean(GLOBAL_TELEPORT);
    } else if (MATCH("nuke_defense")) {
        return b_ini_parse_nuke_missile_defense(NUKE_DEFENSE, value);
    } else if (MATCH("global_nuke_defense")) {
        return b_ini_parse_nuke_missile_defense(GLOBAL_NUKE_DEFENSE, value);
    } else if (MATCH("missile_defense")) {
        return b_ini_parse_nuke_missile_defense(MISSILE_DEFENSE, value);
    } else if (MATCH("global_missile_defense")) {
        return b_ini_parse_nuke_missile_defense(GLOBAL_MISSILE_DEFENSE, value);
    } else if (MATCH("is_sensor")) {
        return b_ini_parse_boolean(SENSOR);
    } else if (MATCH("global_is_sensor")) {
        return b_ini_parse_boolean(GLOBAL_SENSOR);
    } else if (MATCH("submersion")) {
        return b_ini_parse_boolean(SUBMERSION);
    } else if (MATCH("global_submersion")) {
        return b_ini_parse_boolean(GLOBAL_SUBMERSION);
    } else if (MATCH("population_limit")) {
        return b_ini_parse_integer(POP_LIMIT, value, 0, MaxBasePopSize);
    } else if (MATCH("global_population_limit")) {
        return b_ini_parse_integer(GLOBAL_POP_LIMIT, value, 0, MaxBasePopSize);
    } else if (MATCH("drones")) {
        return b_ini_parse_integer(DRONES, value, -MaxBasePopSize, MaxBasePopSize);
    } else if (MATCH("global_drones")) {
        return b_ini_parse_integer(GLOBAL_DRONES, value, -MaxBasePopSize, MaxBasePopSize);
    } else if (MATCH("talents")) {
        return b_ini_parse_integer(TALENTS, value, -MaxBasePopSize, MaxBasePopSize);
    } else if (MATCH("global_talents")) {
        return b_ini_parse_integer(GLOBAL_TALENTS, value, -MaxBasePopSize, MaxBasePopSize);
    } else if (MATCH("suppress_psych")) {
        return b_ini_parse_boolean(SUPPRESS_PSYCH);
    } else if (MATCH("global_suppress_psych")) {
        return b_ini_parse_boolean(GLOBAL_SUPPRESS_PSYCH);
    } else if (MATCH("terraform_eco_damage_reduction")) {
        return b_ini_parse_integer(ECO_DAMAGE_REDUCTION_TERRAFORM, value, 0, 100);
    } else if (MATCH("global_terraform_eco_damage_reduction")) {
        return b_ini_parse_integer(GLOBAL_ECO_DAMAGE_REDUCTION_TERRAFORM, value, 0, 100);
    } else if (MATCH("mineral_eco_damage_reduction")) {
        return b_ini_parse_integer(ECO_DAMAGE_REDUCTION_MINERAL, value, 0, 100);
    } else if (MATCH("global_mineral_eco_damage_reduction")) {
        return b_ini_parse_integer(GLOBAL_ECO_DAMAGE_REDUCTION_MINERAL, value, 0, 100);
    } else if (MATCH("base_nme")) {
        return b_ini_parse_three_integer(BASE_SQ_INCOME, value);
    } else if (MATCH("global_base_nme")) {
        return b_ini_parse_three_integer(GLOBAL_BASE_SQ_INCOME, value);
    } else if (MATCH("resource_percent")) {
        return b_ini_parse_three_integer(RESOURCE_PERCENT, value);
    } else if (MATCH("global_resource_percent")) {
        return b_ini_parse_three_integer(GLOBAL_RESOURCE_PERCENT, value);
    } else if (MATCH("forest_nme")) {
        return b_ini_parse_three_integer(FOREST_INCOME, value);
    } else if (MATCH("global_forest_nme")) {
        return b_ini_parse_three_integer(GLOBAL_FOREST_INCOME, value);
    } else if (MATCH("improved_ocean_nme")) {
        return b_ini_parse_three_integer(IMPROVED_OCEAN_INCOME, value);
    } else if (MATCH("global_improved_ocean_nme")) {
        return b_ini_parse_three_integer(GLOBAL_IMPROVED_OCEAN_INCOME, value);
    } else if (MATCH("improved_land_nme")) {
        return b_ini_parse_three_integer(IMPROVED_LAND_INCOME, value);
    } else if (MATCH("global_improved_land_nme")) {
        return b_ini_parse_three_integer(GLOBAL_IMPROVED_LAND_INCOME, value);
    } else if (MATCH("fungus_nme")) {
        return b_ini_parse_three_integer(FUNGUS_INCOME, value);
    } else if (MATCH("global_fungus_nme")) {
        return b_ini_parse_three_integer(GLOBAL_FUNGUS_INCOME, value);
    } else if (MATCH("square_nme")) {
        return b_ini_parse_three_integer(SQUARE_INCOME, value);
    } else if (MATCH("global_square_nme")) {
        return b_ini_parse_three_integer(GLOBAL_SQUARE_INCOME, value);
    } else if (MATCH("base_clp")) {
        return b_ini_parse_three_integer(CLP_INCOME, value);
    } else if (MATCH("global_base_clp")) {
        return b_ini_parse_three_integer(GLOBAL_CLP_INCOME, value);
    } else if (MATCH("coeff_clp")) {
        return b_ini_parse_three_integer(CLP_COEFF, value);
    } else if (MATCH("global_coeff_clp")) {
        return b_ini_parse_three_integer(GLOBAL_CLP_COEFF, value);
    } else if (MATCH("nme_cap")) {
        return b_ini_parse_three_integer(NME_CAP, value);
    } else if (MATCH("global_nme_cap")) {
        return b_ini_parse_three_integer(GLOBAL_NME_CAP, value);
    } else if (MATCH("base_final_labs_mult")) {
        return b_ini_parse_integer(BASE_FINAL_LABS_MULT, value, 0);
    } else if (MATCH("global_base_final_labs_mult")) {
        return b_ini_parse_integer(GLOBAL_BASE_FINAL_LABS_MULT, value, 0);
    } else if (MATCH("full_satellite_value")) {
        return b_ini_parse_boolean(FULL_SATELLITE_EFFECT);
    } else if (MATCH("global_full_satellite_value")) {
        return b_ini_parse_boolean(GLOBAL_FULL_SATELLITE_EFFECT);
    } else if (MATCH("can_build_satellite")) {
        return b_ini_parse_boolean(CAN_BUILD_SATELLITE);
    } else if (MATCH("global_can_build_satellite")) {
        return b_ini_parse_boolean(GLOBAL_CAN_BUILD_SATELLITE);
    } else if (MATCH("base_size_indicator_border_color")) {
        return b_ini_parse_two_integer(BASE_SIZE_INDICATOR_BORDER_COLOR, value);
    } else if (MATCH("global_base_size_indicator_border_color")) {
        return b_ini_parse_two_integer(GLOBAL_BASE_SIZE_INDICATOR_BORDER_COLOR, value);
    } else if (MATCH("mind_control_resistance")) {
        return b_ini_parse_two_integer(MIND_CONTROL_RESISTANCE, value, 1);
    } else if (MATCH("global_mind_control_resistance")) {
        return b_ini_parse_two_integer(GLOBAL_MIND_CONTROL_RESISTANCE, value, 1);
    } else if (MATCH("link_alien_artifact")) {
        return b_ini_parse_boolean(LINK_ALIEN_ARTIFACT);
    } else if (MATCH("global_link_alien_artifact")) {
        return b_ini_parse_boolean(GLOBAL_LINK_ALIEN_ARTIFACT);
    } else if (MATCH("retool_max_penalty")) {
        return b_ini_parse_integer(RETOOL_MAX_PENALTY_CLASS, value, 0, 3);
    } else if (MATCH("global_retool_max_penalty")) {
        return b_ini_parse_integer(GLOBAL_RETOOL_MAX_PENALTY_CLASS, value, 0, 3);
    } else if (MATCH("prototype_penalty_mult")) {
        return b_ini_parse_integer(PROTOTYPE_PENALTY_MULT, value);
    } else if (MATCH("global_prototype_penalty_mult")) {
        return b_ini_parse_integer(GLOBAL_PROTOTYPE_PENALTY_MULT, value);
    } else if (MATCH("mult_credits")) {
        return b_ini_parse_two_integer(MULTIPLICATIVE_CREDITS, value, 1);
    } else if (MATCH("global_mult_credits")) {
        return b_ini_parse_two_integer(GLOBAL_MULTIPLICATIVE_CREDITS, value, 1);
    } else if (MATCH("mult_labs")) {
        return b_ini_parse_two_integer(MULTIPLICATIVE_LABS, value, 1);
    } else if (MATCH("global_mult_labs")) {
        return b_ini_parse_two_integer(GLOBAL_MULTIPLICATIVE_LABS, value, 1);
    } else if (MATCH("mult_psych")) {
        return b_ini_parse_two_integer(MULTIPLICATIVE_PSYCH, value, 1);
    } else if (MATCH("global_mult_psych")) {
        return b_ini_parse_two_integer(GLOBAL_MULTIPLICATIVE_PSYCH, value, 1);
    } else if (MATCH("planet_fung_bonus")) {
        return b_ini_parse_manifold_harmonics(value);
    } else if (MATCH("global_planet_fung_bonus")) {
        return b_ini_parse_manifold_harmonics(value, true);
    } else if (MATCH("link_infinite_artifacts")) {
        return b_ini_parse_boolean(LINK_INFINITE_ARTIFACTS);
    } else if (MATCH("global_link_infinite_artifacts")) {
        return b_ini_parse_boolean(GLOBAL_LINK_INFINITE_ARTIFACTS);
    } else if (MATCH("commerce_as_income")) {
        return b_ini_parse_string(COMMERCE_TO_INCOME, parse_clp, value);
    } else if (MATCH("global_commerce_as_income")) {
        return b_ini_parse_string(GLOBAL_COMMERCE_TO_INCOME, parse_clp, value);
    } else if (MATCH("force_pop_boom")) {
        return b_ini_parse_boolean(POP_BOOM);
    } else if (MATCH("global_force_pop_boom")) {
        return b_ini_parse_boolean(GLOBAL_POP_BOOM);
    } else if (MATCH("half_maintenance")) {
        return b_ini_parse_boolean(HALF_MAINTENANCE);
    } else if (MATCH("global_half_maintenance")) {
        return b_ini_parse_boolean(GLOBAL_HALF_MAINTENANCE);
    } else if (MATCH("virtual_police")) {
        return b_ini_parse_integer(VIRTUAL_POLICE, value, 0, 3);
    } else if (MATCH("global_virtual_police")) {
        return b_ini_parse_integer(GLOBAL_VIRTUAL_POLICE, value, 0, 3);
    } else if (MATCH("satellite_production_mult")) {
        return b_ini_parse_two_integer(SATELLITE_PROD_MULT, value, 1);
    } else if (MATCH("global_satellite_production_mult")) {
        return b_ini_parse_two_integer(GLOBAL_SATELLITE_PROD_MULT, value, 1);
    } else if (MATCH("prevent_drone_riot")) {
        return b_ini_parse_boolean(PREVENT_DRONE_RIOT);
    } else if (MATCH("global_prevent_drone_riot")) {
        return b_ini_parse_boolean(GLOBAL_PREVENT_DRONE_RIOT);
    } else if (MATCH("stockpile_energy_bonus")) {
        return b_ini_parse_two_integer(STOCKPILE_ENERGY_BONUS, value, 1);
    } else if (MATCH("global_stockpile_energy_bonus")) {
        return b_ini_parse_two_integer(GLOBAL_STOCKPILE_ENERGY_BONUS, value, 1);
    } else if (MATCH("efficiency_floor_flat")) {
        return b_ini_parse_integer(EFFICIENCY_FLOOR_FLAT, value);
    } else if (MATCH("global_efficiency_floor_flat")) {
        return b_ini_parse_integer(GLOBAL_EFFICIENCY_FLOOR_FLAT, value);
    } else if (MATCH("efficiency_floor_percent")) {
        return b_ini_parse_integer(EFFICIENCY_FLOOR_PERCENT, value, 0, 100);
    } else if (MATCH("global_efficiency_floor_percent")) {
        return b_ini_parse_integer(GLOBAL_EFFICIENCY_FLOOR_PERCENT, value, 0, 100);
    } else if (MATCH("cost_mult")) {
        return b_ini_parse_cost_mult(value);
    } else if (MATCH("global_cost_mult")) {
        return b_ini_parse_cost_mult(value, true);
    } else if (MATCH("preq_count_as")) {
        return b_ini_parse_string(PREQ_COUNT_AS, find_facility_id, value);
    } else if (MATCH("global_preq_count_as")) {
        return b_ini_parse_string(GLOBAL_PREQ_COUNT_AS, find_facility_id, value);
    } else if (MATCH("fac_count")) {
        return b_ini_parse_two_string(FACILITY_COUNT, find_facility_id, parse_clp, value);
    } else if (MATCH("global_fac_count")) {
        return b_ini_parse_two_string(GLOBAL_FACILITY_COUNT, find_facility_id, parse_clp, value);
    } else if (MATCH("active_probes_morale")) { // ########## End of Globalizable properties - Project Only Properties start here
        return b_ini_parse_integer(ACTIVE_PROBE_MORALE, value, 0, 6);
    } else if (MATCH("terraform_rate")) {
        return b_ini_parse_terraform_rate(value);
    } else if (MATCH("terraform_unlock")) {
        return b_ini_parse_terraform_unlock(value);
    } else if (MATCH("council_votes_increase")) {
        return b_ini_parse_two_integer(COUNCIL_VOTES_MODIFIER, value);
    } else if (MATCH("counts_as_infiltration")) {
        return b_ini_parse_boolean(COUNTS_AS_INFILTRATION);
    } else if (MATCH("fungus_repair_as_native")) {
        return b_ini_parse_boolean(FUNGUS_REPAIR_AS_NATIVE);
    } else if (MATCH("fungus_move_as_native")) {
        return b_ini_parse_boolean(FUNGUS_MOVE_AS_NATIVE);
    } else if (MATCH("fungus_combat_as_native")) {
        return b_ini_parse_boolean(FUNGUS_COMBAT_AS_NATIVE);
    } else if (MATCH("psi_defense")) {
        return b_ini_parse_integer(PSI_DEFENSE, value);
    } else if (MATCH("psi_attack")) {
        return b_ini_parse_integer(PSI_ATTACK, value);
    } else if (MATCH("land_speed")) {
        return b_ini_parse_movement(value, TRIAD_LAND);
    } else if (MATCH("sea_speed")) {
        return b_ini_parse_movement(value, TRIAD_SEA);
    } else if (MATCH("air_speed")) {
        return b_ini_parse_movement(value, TRIAD_AIR);
    } else if (MATCH("tech_share")) {
        return b_ini_parse_integer(TECH_SHARE, value);
    } else if (MATCH("se_economy")) {
        return b_ini_parse_se_global(SE_ECONOMY, value);
    } else if (MATCH("se_efficiency")) {
        return b_ini_parse_se_global(SE_EFFIC, value);
    } else if (MATCH("se_support")) {
        return b_ini_parse_se_global(SE_SUPPORT, value);
    } else if (MATCH("se_morale")) {
        return b_ini_parse_se_global(SE_MORALE, value);
    } else if (MATCH("se_police")) {
        return b_ini_parse_se_global(SE_POLICE, value);
    } else if (MATCH("se_growth")) {
        return b_ini_parse_se_global(SE_GROWTH, value);
    } else if (MATCH("se_planet")) {
        return b_ini_parse_se_global(SE_PLANET, value);
    } else if (MATCH("se_probe")) {
        return b_ini_parse_se_global(SE_PROBE, value);
    } else if (MATCH("se_industry")) {
        return b_ini_parse_se_global(SE_INDUSTRY, value);
    } else if (MATCH("se_research")) {
        return b_ini_parse_se_global(SE_RESEARCH, value);
    } else if (MATCH("base_se_economy")) {
        return b_ini_parse_base_se(SE_ECONOMY, value);
    } else if (MATCH("base_se_efficiency")) {
        return b_ini_parse_base_se(SE_EFFIC, value);
    } else if (MATCH("base_se_support")) {
        return b_ini_parse_base_se(SE_SUPPORT, value);
    } else if (MATCH("base_se_police")) {
        return b_ini_parse_base_se(SE_POLICE, value);
    } else if (MATCH("base_se_growth")) {
        return b_ini_parse_base_se(SE_GROWTH, value);
    } else if (MATCH("base_se_planet")) {
        return b_ini_parse_base_se(SE_PLANET, value);
    } else if (MATCH("base_se_probe")) {
        return b_ini_parse_base_se(SE_PROBE, value);
    } else if (MATCH("base_se_industry")) {
        return b_ini_parse_base_se(SE_INDUSTRY, value);
    } else if (MATCH("probe_immunity")) {
        return b_ini_parse_integer(PROBE_IMMUNITY, value, 1, 2);
    } else if (MATCH("repair_speed")) {
        return b_ini_parse_integer(REPAIR_SPEED, value, 0, 10);
    } else if (MATCH("lift_repair_limit")) {
        return b_ini_parse_boolean(LIFT_REPAIR_LIMIT);
    } else if (MATCH("unit_upgrade_cost_mult")) {
        return b_ini_parse_two_integer(UNIT_UPGRADE_COST_MULT, value);
    } else if (MATCH("impunity")) {
        return b_ini_parse_impunity(value);
    } else if (MATCH("pop_floor")) {
        return b_ini_parse_integer(POP_FLOOR, value, 0, MaxBasePopSize);
    } else if (MATCH("drones_under_pop_floor")) {
        return b_ini_parse_integer(DRONES_UNDER_POP_FLOOR, value, -MaxBasePopSize, MaxBasePopSize);
    } else if (MATCH("orbital_insertion")) {
        return b_ini_parse_boolean(ORBITAL_INSERTION);
    } else if (MATCH("mind_control_mult")) {
        return b_ini_parse_two_integer(MIND_CONTROL_MULTIPLIER, value);
    } else if (MATCH("all_probes_have_algo_enhance")) {
        return b_ini_parse_boolean(ALL_PROBES_ALGORITHMIC_ENHANCEMENT);
    } else if (MATCH("longevity_vaccine_effects")) {
        return b_ini_parse_boolean(LONGEVITY_VACCINE_EFFECTS);
    } else if (MATCH("voice_of_planet_effects")) {
        return b_ini_parse_boolean(VOICE_OF_PLANET_EFFECTS);
    } else if (MATCH("project_time_warp_enabled")) {
        return b_ini_parse_boolean(PROJECT_TIME_WARP_ENABLED);
    } else if (MATCH("transcendence_victory")) {
        return b_ini_parse_boolean(TRANSCENDENCE_VICTORY);
    }
}

int ini_opt_error(const char* section, const char* name) {
    static bool unknown_option = false;
    char msg[1024] = {};
    if (!unknown_option) {
        snprintf(msg, sizeof(msg),
            "Unknown configuration option detected in %s.\n"
            "Game might not work as intended.\n"
            "Header: %s\n"
            "Option: %s\n",
            current_file, section, name);
        MessageBoxA(0, msg, MOD_VERSION, MB_OK | MB_ICONWARNING);
    }
    unknown_option = true;
    return 0;
}

// Generalized parsing functions
int b_ini_parse_boolean(BuilderPropertyType type) {
    // Value is ignored - Falsy is done by not providing the field.
    BuilderProperty prop = {0, 0, 0, (uint8_t)type};
    current_builder_fac->add_prop(prop);
    return 1
}

// Supports both signed and insigned, low/high values should restrict these to 8-bit representations to work correctly.
// BuilderProperty always stores as unsigned, just c-style casting will fix that.
int b_ini_parse_integer(BuilderPropertyType type, const char* value, int low = -128, int high = 255, int field = 0) {
    BuilderProperty prop = {0, 0, 0, (uint8_t)type};
    prop.data[field] = (uint8_t)clamp(atoi(value), low, high);
    current_builder_fac->add_prop(prop);
    return 1;
}

// Floats are also parsed as two integers - the only differentiation is when the Properties are processed.
int b_ini_parse_two_integer(BuilderPropertyType type, const char* value, int low = 0, int high = 255) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    if (comma_count(buf, INI_MAX_LINE, 1, 1) == -1) {return 0;}
    uint8_t a = clamp(atoi(strtok(buf, ",")), low, high);
    uint8_t b = clamp(atoi(strtok(NULL, ",")), low, high);
    BuilderProperty prop = {a, b, 0, (uint8_t)type};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_three_integer(BuilderPropertyType type, const char* value, int low = 0, int high = 255) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    if (comma_count(buf, INI_MAX_LINE, 2, 2) == -1) {return 0;}
    uint8_t a = clamp(atoi(strtok(buf, ",")), low, high);
    uint8_t b = clamp(atoi(strtok(NULL, ",")), low, high);
    uint8_t c = clamp(atoi(strtok(NULL, ",")), low, high);
    BuilderProperty prop = {a, b, c, (uint8_t)type};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_string(BuilderPropertyType type, string_transformer* func, const char* value) {
    int result = func(value);
    if (result == 100) {return 0;}
    BuilderProperty prop = {result, 0, 0, (uint8_t)type};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_two_string(BuilderPropertyType type, string_transformer* func, string_transformer* second_func, const char* value) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    if (comma_count(buf, INI_MAX_LINE, 1, 1) == -1) {return 0;}
    int first = func(strtok(buf, ","));
    if (first == 100) {return 0;}
    int second = func(strtok(NULL, ","));
    if (second == 100) {return 0;}
    BuilderProperty prop = {first, second, 0, (uint8_t)type};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_garrison_morale(BuilderPropertyType type, const char* value) {
    // Compatible with MORALE_MOD and MORALE_FLOOR
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    int commas = comma_count(buf, INI_MAX_LINE, 0, 3);
    if (commas == -1) {return 0;}
    uint8_t filter = 0;
    uint8_t floor = clamp(atoi(strtok(buf, ",")), 0, 6);
    for (; commas; commas--) {
        filter |= garrison_morale_parse(strtok(NULL, ","));
    }
    BuilderProperty prop = {floor, filter, 0, (uint8_t)type};
    current_builder_fac->add_prop(prop);
    return 1;
}

// Specific parsing functions
int b_ini_parse_se_global(SocialEffect category, const char* value) {
    BuilderProperty prop = {(uint8_t)clamp(atoi(value), -10, 10), (uint8_t)category, 0, (uint8_t)SE_FACTION};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_base_se(SocialEffect category, const char* value) {
    BuilderProperty prop = {(uint8_t)clamp(atoi(value), -10, 10), (uint8_t)category, 0, (uint8_t)SE};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_count_as(const char* value, bool global = false) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    int commas = comma_count(buf, INI_MAX_LINE, 0, 1);
    uint8_t counts, filter;
    if (commas == -1) {
        return 0;
    } else if (commas) {
        counts = find_facility_id(strtok(buf, ","));
        filter = find_facility_id(strtok(NULL, ","));
        if (!filter) {
            return 0;
        }
    } else {
        counts = find_facility_id(value);
        filter = 0;
    }
    if (!counts) {
        return 0;
    }
    BuilderProperty prop = {counts, filter, 0, (uint8_t)(global ? COUNTS_AS: GLOBAL_COUNTS_AS)};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_repair(const char* value, bool global = false) {
    BuilderProperty prop = {0, 0, triad_parse(value), (uint8_t)(global ? REPAIR: GLOBAL_REPAIR)};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_morale(const char* value, bool global = false) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    if (comma_count(buf, INI_MAX_LINE, 1, 1) == -1) {return 0;}
    uint8_t triad = triad_parse(strtok(buf, ","), true);
    if (triad == 100) {return 0;}
    BuilderProperty prop = {clamp(atoi(strtok(NULL, ",")), 0, 6), 0, triad, (uint8_t)(global ? MORALE: GLOBAL_MORALE)};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_movement(const char* value, Triad triad) {
    BuilderProperty prop = {clamp(atoi(value), 0, 6), (uint8_t)triad, 0, (uint8_t)UNIT_SPEED};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_defense(const char* value, bool global = false) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    if (comma_count(buf, INI_MAX_LINE, 1, 1) == -1) {return 0;}
    uint16_t val = clamp(atoi(strtok(buf, ",")), 0, 65535);
    uint8_t triad = triad_parse(strtok(NULL, ","));
    if (triad == 100) {return 0;}
    BuilderProperty prop = {val % 256, val / 256, triad, (uint8_t)(global ? DEFENSE: GLOBAL_DEFENSE)};
    current_builder_fac->add_prop(prop);
    return 1
}

int b_ini_parse_cost_mult(const char* value, bool global = false) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    if (comma_count(buf, INI_MAX_LINE, 1, 1) == -1) {return 0;}
    uint8_t triad = triad_parse(strtok(buf, ","), true);
    if (triad == 100) {return 0;}
    uint8_t val = clamp(atoi(strtok(NULL, ",")), 0, 65535);
    BuilderProperty prop = {val, 0, triad, (uint8_t)(global ? COST_MULT: GLOBAL_COST_MULT)};
    current_builder_fac->add_prop(prop);
    return 1
}

int b_ini_parse_nuke_missile_defense(BuilderPropertyType type, const char* value) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    if (comma_count(buf, INI_MAX_LINE, 1, 1) == -1) {return 0;}
    uint8_t range = clamp(atoi(strtok(buf, ",")), 0, 8);
    uint8_t magnitude = clamp(atoi(strtok(NULL, ",")), 0, 255);
    BuilderProperty prop = {range, 0, magnitude, (uint8_t)type};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_manifold_harmonics(const char* value, bool is_global) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    if (comma_count(buf, INI_MAX_LINE, 2, 2) == -1) {return 0;}
    uint8_t planet = clamp(atoi(strtok(buf, ",")), 0, 3);
    uint8_t resource = parse_nme(strtok(NULL, ","));
    if (resource == 100) {return 0;}
    uint8_t bonus = clamp(atoi(strtok(NULL, ",")), 0, 100);
    BuilderProperty prop = {planet, (uint8_t)resource, bonus, (uint8_t)(global ? PLANET_FUNG_BONUS: GLOBAL_PLANET_FUNG_BONUS)};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_terraform_rate(const char* value) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    int commas = comma_count(buf, INI_MAX_LINE, 1, 19);
    if (commas == -1) {return 0;}
    uint32_t flags = 0;
    uint8_t rate = clamp(atoi(strtok(buf, ",")), 1, 31); // Real effect is 10xthis
    char* entry = strtok(NULL, ",");
    while (entry) {
        flags |= parse_terraform(entry);
        entry = strtok(NULL, ",");
    }
    flags = (flags << 5) + rate;
    BuilderProperty prop = {(uint8_t)flags, (uint8_t)flags / 8, (uint8_t)flags / 16, (uint8_t)TERRAFORM_RATE};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_terraform_unlock(const char* value) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    int commas = comma_count(buf, INI_MAX_LINE, 0, 18);
    if (commas == -1) {return 0;}
    uint32_t flags = 0;
    for (char* entry = strtok(buf, ","); entry; entry = strtok(NULL, ",")) {
        flags |= parse_terraform(entry);
    }
    BuilderProperty prop = {(uint8_t)flags, (uint8_t)flags >> 8, (uint8_t)flags >> 16, (uint8_t)TERRAFORM_UNLOCK};
    current_builder_fac->add_prop(prop);
    return 1;
}

int b_ini_parse_impunity(const char* value) {
    char buf[INI_MAX_LINE];
    strcpy_n(buf, INI_MAX_LINE, value);
    int commas = comma_count(buf, INI_MAX_LINE, 0, 15);
    if (commas == -1) {return 0;}
    uint16_t flags = 0;
    for (char* entry = strtok(buf, ","); entry; entry = strtok(NULL, ",")) {
        flags |= parse_social_model(entry);
    }
    BuilderProperty prop = {(uint8_t)flags, (uint8_t)flags >> 8, 0, (uint8_t)IMPUNITY};
    current_builder_fac->add_prop(prop);
    return 1;
}

// Parsing utils
int comma_count(const char* test_value, int array_length, int low, int high) {
    /* -1 is the magic "error" value, since a string can validly have 0 commas (1 required argument, other arguments optional). */
    int cnt = 0;
    for (int i = 0; i < array_length && test_value[i] != "\0"; i++) {
        if (test_value[i] == ',')
            cnt++;
    }
    if (cnt < low || cnt > high) {
        char msg[1024] = {};
        snprintf(msg, sizeof(msg),
            "Errorous value detected in %s.\n"
            "Value: %s\n"
            "Value should have had %s - %s commas, had %s.\n",
            current_file, test_value, low, high, cnt);
        MessageBoxA(0, msg, MOD_VERSION, MB_OK | MB_ICONWARNING);
        return -1;
    }
    return cnt;
}

uint8_t triad_parse(const char* name, bool extended = false) {
    if (MATCH("LAND")) {
        return (uint8_t)TRIAD_LAND;
    } else if (MATCH("SEA")) {
        return (uint8_t)TRIAD_SEA;
    } else if (MATCH("AIR")) {
        return (uint8_t)TRIAD_AIR;
    } else if (extended && MATCH("NATIVE")) {
        return (uint8_t)TRIAD_AIR;
    } else if (extended && MATCH("PROBE")) {
        return (uint8_t)TRIAD_AIR;
    }
    char msg[1024] = {};
    snprintf(msg, sizeof(msg),
        "Errorous value detected in %s.\n"
        "Cannot parse Triad from %s\n",
        current_file, name);
    MessageBoxA(0, msg, MOD_VERSION, MB_OK | MB_ICONWARNING);
    return 100;
}

uint8_t garrison_morale_parse(const char* name) {
    // Bits 1, 2, 3 and 4 indicate requiring native, non-native, defender, and attacker specifically when set, and always valid when not.
    if (MATCH("NATIVE")) {
        return 1;
    } else if (MATCH("NON_NATIVE")) {
        return 2;
    } else if (MATCH("DEFENDER")) {
        return 4;
    } else if (MATCH("ATTACKER")) {
        return 8;
    }
    char msg[1024] = {};
    snprintf(msg, sizeof(msg),
        "Errorous value detected in %s.\n"
        "Cannot parse Morale Filter from %s\n",
        current_file, name);
    MessageBoxA(0, msg, MOD_VERSION, MB_OK | MB_ICONWARNING);
    return 100;
}

uint8_t parse_nme(const char* name) {
    // Bits 1, 2, 3 and 4 indicate requiring native, non-native, defender, and attacker specifically when set, and always valid when not.
    if (MATCH("NUTRIENT")) {
        return 0;
    } else if (MATCH("MINERAL")) {
        return 1;
    } else if (MATCH("ENERGY")) {
        return 2;
    }
    char msg[1024] = {};
    snprintf(msg, sizeof(msg),
        "Errorous value detected in %s.\n"
        "Cannot parse NUTRIENT/MINERAL/ENERGY from %s\n",
        current_file, name);
    MessageBoxA(0, msg, MOD_VERSION, MB_OK | MB_ICONWARNING);
    return 100;
}

uint8_t parse_clp(const char* name) {
    // Bits 1, 2, 3 and 4 indicate requiring native, non-native, defender, and attacker specifically when set, and always valid when not.
    if (MATCH("CREDITS")) {
        return 0;
    } else if (MATCH("LABS")) {
        return 1;
    } else if (MATCH("PSYCH")) {
        return 2;
    }
    char msg[1024] = {};
    snprintf(msg, sizeof(msg),
        "Errorous value detected in %s.\n"
        "Cannot parse CREDITS/LABS/PSYCH from %s\n",
        current_file, name);
    MessageBoxA(0, msg, MOD_VERSION, MB_OK | MB_ICONWARNING);
    return 100;
}

uint8_t find_facility_id(const char* name) {
    // 0 is the "not found" value.
    for (int i = 1; i <= MaxFacilityNum; i++) {
        if (MATCH(Facility[i].name)) {
            return i;
        }
    }
    char msg[1024] = {};
    snprintf(msg, sizeof(msg),
        "Errorous value detected in %s.\n"
        "Cannot parse Facility from %s\n",
        current_file, name);
    MessageBoxA(0, msg, MOD_VERSION, MB_OK | MB_ICONWARNING);
    return 0;
}

uint32_t parse_terraform(const char* name) {
    // Returns bitflag for bits 1-18 (inclusive).
    if (MATCH("FARM")) {
        return 1 << FORMER_FARM;
    } else if (MATCH("SOIL_ENR")) {
        return 1 << FORMER_SOIL_ENR;
    } else if (MATCH("MINE")) {
        return 1 << FORMER_MINE;
    } else if (MATCH("SOLAR")) {
        return 1 << FORMER_SOLAR;
    } else if (MATCH("FOREST")) {
        return 1 << FORMER_FOREST;
    } else if (MATCH("ROAD")) {
        return 1 << FORMER_ROAD;
    } else if (MATCH("MAGTUBE")) {
        return 1 << FORMER_MAGTUBE;
    } else if (MATCH("BUNKER")) {
        return 1 << FORMER_BUNKER;
    } else if (MATCH("AIRBASE")) {
        return 1 << FORMER_AIRBASE;
    } else if (MATCH("SENSOR")) {
        return 1 << FORMER_SENSOR;
    } else if (MATCH("REMOVE_FUNGUS")) {
        return 1 << FORMER_REMOVE_FUNGUS;
    } else if (MATCH("PLANT_FUNGUS")) {
        return 1 << FORMER_PLANT_FUNGUS;
    } else if (MATCH("CONDENSER")) {
        return 1 << FORMER_CONDENSER;
    } else if (MATCH("ECH_MIRROR")) {
        return 1 << FORMER_ECH_MIRROR;
    } else if (MATCH("THERMAL_BORE")) {
        return 1 << FORMER_THERMAL_BORE;
    } else if (MATCH("AQUIFER")) {
        return 1 << FORMER_AQUIFER;
    } else if (MATCH("RAISE_LAND")) {
        return 1 << FORMER_RAISE_LAND;
    } else if (MATCH("LOWER_LAND")) {
        return 1 << FORMER_LOWER_LAND;
    } else if (MATCH("LEVEL_TERRAIN")) {
        return 1 << FORMER_LEVEL_TERRAIN;
    }
    char msg[1024] = {};
    snprintf(msg, sizeof(msg),
        "Errorous value detected in %s.\n"
        "Cannot parse Former Action from %s\n"
        "Please note that the FORMER_ prefix must NOT be present.\n",
        current_file, name);
    MessageBoxA(0, msg, MOD_VERSION, MB_OK | MB_ICONWARNING);
    return 0;
}

uint32_t parse_social_model(const char* name) {
    // Returns bitflag for bits 1-18 (inclusive).
    if (MATCH("FRONTIER")) {
        return (1 << (SOCIAL_C_POLITICS * 4 + SOCIAL_M_FRONTIER);
    } else if (MATCH("POLICE STATE")) {
        return (1 << (SOCIAL_C_POLITICS * 4 + SOCIAL_M_POLICE_STATE);
    } else if (MATCH("DEMOCRATIC")) {
        return (1 << (SOCIAL_C_POLITICS * 4 + SOCIAL_M_DEMOCRATIC);
    } else if (MATCH("FUNDAMENTALIST")) {
        return (1 << (SOCIAL_C_POLITICS * 4 + SOCIAL_M_FUNDAMENTALIST);
    } else if (MATCH("SIMPLE")) {
        return (1 << (SOCIAL_C_ECONOMICS * 4 + SOCIAL_M_SIMPLE);
    } else if (MATCH("FREE MARKET")) {
        return(1 << (SOCIAL_C_ECONOMICS * 4 + SOCIAL_M_FREE_MARKET);
    } else if (MATCH("PLANNED")) {
        return (1 << (SOCIAL_C_ECONOMICS * 4 + SOCIAL_M_PLANNED);
    } else if (MATCH("GREEN")) {
        return (1 << (SOCIAL_C_ECONOMICS * 4 + SOCIAL_M_GREEN);
    } else if (MATCH("SURVIVAL")) {
        return (1 << (SOCIAL_C_VALUES * 4 + SOCIAL_M_SURVIVAL);
    } else if (MATCH("POWER")) {
        return (1 << (SOCIAL_C_VALUES * 4 + SOCIAL_M_POWER);
    } else if (MATCH("KNOWLEDGE")) {
        return (1 << (SOCIAL_C_VALUES * 4 + SOCIAL_M_KNOWLEDGE);
    } else if (MATCH("WEALTH")) {
        return (1 << (SOCIAL_C_VALUES * 4 + SOCIAL_M_WEALTH);
    } else if (MATCH("NONE")) {
        return (1 << (SOCIAL_C_FUTURE * 4 + SOCIAL_M_NONE);
    } else if (MATCH("CYBERNETIC")) {
        return (1 << (SOCIAL_C_FUTURE * 4 + SOCIAL_M_CYBERNETIC);
    } else if (MATCH("EUDAIMONIC")) {
        return (1 << (SOCIAL_C_FUTURE * 4 + SOCIAL_M_EUDAIMONIC);
    } else if (MATCH("THOUGHT CONTROL")) {
        return (1 << (SOCIAL_C_FUTURE * 4 + SOCIAL_M_THOUGHT_CONTROL);
    }
    char msg[1024] = {};
    snprintf(msg, sizeof(msg),
        "Errorous value detected in %s.\n"
        "Cannot parse Social Model from %s\n",
        current_file, name);
    MessageBoxA(0, msg, MOD_VERSION, MB_OK | MB_ICONWARNING);
    return 0;
}

