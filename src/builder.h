
#pragma once
#pragma pack(push, 1)

#include "main.h"
#include "engine.h"
#include "engine_enums.h"
#include "engine_veh.h"

// The properties of the facility. Each will have a 32-bit integer value to encode it's effects.
enum BuilderPropertyType : uint8_t {
    // Simple Properties can be cached into BuilderBase attributes.
    NULL_PROPERTY = 0,  // No effect
    PREVENT_POP_LOSS = 1, // Prevents population loss from being attacked, reduces pop loss from being attacked by nerve gas.
    DEFENSE = 2, // Increase defense by a int16_t data[0] + data[1] << 8 amount. Parse triad filter from data[2].
    MORALE_FLOOR = 3,  // Set morale floor of stationed units to data[0], using data[1] as filter.
    MORALE = 4, // On unit production increase morale by data[0]. Parse triad filter from data[2], with 3 = Natives and 4 = Probes as additional meaning. Triad gets halved effect for low morale, others don't. Probe stacks with triad.
    PROBE_PLAGUE_RESISTANCE = 5,  // Multiplicative between facilities, data[0] / data[1] is a multiplier for population loss and veh_damage suffered from Genetic Plague Probe Actions.
    HOMED_UNIT_MIND_CONTROL_RESISTANCE = 6, // Multiplicative between facilities. data[0] / data[1] is a multiplier for distance-from-home-base when a unit gets mind controlled.
    REPAIR = 7, // Base has accelerated repair for triad stored in data[2].
    PROTECT_RANGE_DROP_PODS = 8,  // Base prevents drop pods in this range. max() between facilities, clamp(0, 8) for engine limitations.
    TELEPORT = 9,  // Teleporter is present.
    GARRISON_IGNORE_NEGATIVE_SE_MORALE = 10, // data[0] bit 1 signals requirement to be native, bit 2 requirement to not be native.
    NUKE_DEFENSE = 11,  // data[0] has % intercept chance, while data[2] has range. Ranges are additive, while intercept chance stacks multiplicative. (50+50 = 75). Range is hardcapped at 8.
    MISSILE_DEFENSE = 12, // data[0] has defensive bonus, while data[2] has range. Ranges and defensive bonuses are both additive. Range is hardcapped at 8.
    SENSOR = 13, // Base counts as the Sensor Terraform.
    COUNTS_AS = 14, // Facility counts as the facility whose id is stored in data[0]. data[1] optionally contains a facility to require before COUNTS_AS functions.
    SUBMERSION = 15, // Allows base to be submerged.
    POP_LIMIT = 16, // Sets/increases population limit by data[0]. Capped by main.h->MaxBasePopSize.
    DRONES = 17, // Increase drones by int8_t data[0]
    TALENTS = 18, // Increase talents by int8_t data[0]
    SUPPRESS_PSYCH = 19, // Suppress psych. if data[0] & 1, suppresses drones. If data[0] & 2, suppresses talents.
    ECO_DAMAGE_REDUCTION_TERRAFORM = 20, // data[0] is a % reduction to eco damage from terraforming. Stacks additive.
    ECO_DAMAGE_REDUCTION_MINERAL = 21, // data[0] is added to the divisor.
    BASE_SQ_INCOME = 22, // data[0, 1, 2] has additional nutrient, mineral, energy to be added to the base square yield.
    RESOURCE_PERCENT = 23, // data[0, 1, 2] %-increases nutrients, minerals, and energy respectively. Interpreted as int8_t (so they can also decrease income). Stacks additively.
    FOREST_INCOME = 24, // data[0, 1, 2] has additional nutrient, mineral, energy to be added to all forest squares.
    IMPROVED_OCEAN_INCOME = 25, // data[0, 1, 2] has additional nutrient, mineral, energy to be added to improved ocean squares.
    FUNGUS_INCOME = 26, // data[0, 1, 2] has additional nutrient, mineral, energy to be added to fungus squares.
    SQUARE_INCOME = 27, // data[0, 1, 2] has additional nutrient, mineral, energy to be added to non-fungus squares.
    CLP_INCOME = 28, // data[0, 1, 2] gives flat credits/labs/psych income for the base.
    CLP_COEFF = 29,  // data[0, 1, 2] gives % increase to credits/labs/psych.
    BASE_FINAL_LABS_MULT = 30, // data[0] is a % modifier to labs. Multiplicative with anything, used by punishment sphere to nerf science.
    FULL_SATELLITE_EFFECT = 31, // Enables getting full benefits instead of half.
    CAN_BUILD_SATELLITE = 32,  // Allows building satellites.
    NME_CAP = 33,  // Added to the yield cap for Nutrient/Mineral/Energy income.
    IMPROVED_LAND_INCOME = 34, // data[0, 1, 2] has additional nutrient, mineral, energy to be added to improved land squares.
    BASE_SIZE_INDICATOR_BORDER_COLOR = 35,  // data[0] contains color code as defined in gui.h. data[1] has the border width (thickness?)
    MIND_CONTROL_RESISTANCE = 36, // data[0]/data[1] is a multiplier for distance from HQ when calculating base price for probe mind control.
    LINK_ALIEN_ARTIFACT = 37,  // Base can link alien artifacts.
    RETOOL_MAX_PENALTY_CLASS = 38, // data[0] = RETOOL_ALWAYS_FREE = 0, RETOOL_FREE_CATEGORY = 1, RETOOL_FREE_PROJECT = 2, RETOOL_NEVER_FREE = 3,
    PROTOTYPE_PENALTY_MULT = 39, // data[0] has % mult for the penalty = so 100 for full, 0 for no penalty.
    MULTIPLICATIVE_CREDITS = 40, // data[0] / data[1] is a multiplicative increase in credits.
    MULTIPLICATIVE_LABS = 41, // data[0] / data[1] is a multiplicative increase in labs.
    MULTIPLICATIVE_PSYCH = 42, // data[0] / data[1] is a multiplicative increase in psych.
    PLANET_FUNG_BONUS = 43, // data[2] has required PLANET score, data[1] the yield to increase (0/1/2 = Nutrient/Mineral/Energy) and data[0] the amount to increase it with.
    LINK_INFINITE_ARTIFACTS = 44,
    COMMERCE_TO_INCOME = 45, // Add Commerce to Credits (0), Labs (1), or Psych (2)
    POP_BOOM = 46, // Base is in permanent pop boom, if sufficient nutrients.
    HALF_MAINTENANCE = 47, // Reduce all facility maintenance cost by half.
    VIRTUAL_POLICE = 48, // Base psych acts as if data[0] garrison members are additionally present for policing.
    SATELLITE_PROD_MULT = 49, // Multiply minerals put towards satellite production by data[0] / data[1].
    PREVENT_DRONE_RIOT = 50, // Drones may still exist, but not riot.
    STOCKPILE_ENERGY_BONUS = 51, // data[0] / data[1] multiplier to stockpile energy.
    EFFICIENCY_FLOOR_FLAT = 52, // The first data[0] energy cannot be lost to inefficiency. (if >= total energy income, no inefficiency occurs)
    EFFICIENCY_FLOOR_PERCENT = 53, // This % of Energy cannot be lost to inefficiency. (if >= 100, no inefficiency occurs)
    MORALE_MOD = 54,  // Increases morale of units stationed in this base by data[0], using data[1] as filter.


    // Complex properties are saved to the BuilderBase as properties, and calculated on demand.
    COST_MULT = 62,  // Grant int8_t data[0] discount to the unit filtered by triad, probe, and native in data[2].
    PREQ_COUNT_AS = 63, // Is considered to also be facility_id=data[0] when checking prerequisites.
    FACILITY_COUNT = 64, // data[0] is the facility_id to count, data[1] is where to add it to (0 = LABS, 1 = CREDITS, 2 = PSYCH)

    // Non-globalizable properties that are still saved on the Base. Most of these are flags that are checked for.
    SE = 65,  // Social Engieering increases. data[0] is effect size, data[2] has the SE effect index from engine_enums.h->SocialEffect.
    IS_HQ = 66, // Fac is HQ.
    ALIEN_VICTORY = 67, // Counts towards Alien Victory.
    HANDLE_EVENT_GROWTH = 68, // id=19. Vanilla, Children's Creche, base grows or drones #CRECHE
    HANDLE_EVENT_ENERGY = 69, // id=17, Energy Bank, gain or lose credits #SURGE
    HANDLE_EVENT_NETBONUS = 70, // id=18, Network Node, lose all accumulated labs or get free tech #NETBONUS
    HANDLE_EVENT_BLIGHT = 71, // id=16 Biology Lab, gain food income +1 or lose all forests/farms #BLIGHT
    HANDLE_EVENT_PROMETHEUS = 72, // id=6, hospitals and projects. Resist or lose half pop in multiple bases. #PROMETHEUS1
    NUKE_TARGET = 73,

    // Properties that make no sense on the base, but are sensible on the facility itself.
    // Build Requirement related.
    IS_SATELLITE = 80, // Facility is a satellite - it requires CAN_BUILD_SATELLITE, and it can be attacked by ODP's.
    REQUIRE_FACILITY = 81, // data[0] has id of required facility
    INCOMPATIBLE_FACILITY = 82, // data[0] has id of facility that may not be built.
    REQUIRE_COAST = 83, // Facility requires coastal city.
    IS_SMAX = 84, // Facility not available in smac_only mode.
    ALIEN_EXCLUSIVE = 85, // Facility is alien exclusive.
    REQUIRE_POP_SIZE = 86, // Facility requires this much pop size.
    REQUIRE_TRANSCEND_VICTORY_POSSIBLE = 87,
    REQUIRE_PROJECT_COMPLETED = 88,
    // Free gifts
    FIRST_BASE_FREE_MP = 89, // In Multiplayer, first base receives this facility for free.
    FIRST_BASE_FREE_MP_DIFFICULTY = 90, // In Multiplayer, if the difficulty is data[0] or higher, give this facility for free.
    FIRST_BASE_FREE_MP_SE_POSITIVE = 91, // In Multiplayer, if your starting SE rating of data[1] exceeds data[0], this facility is given for free.
    FIRST_BASE_FREE_TURN = 92, // If you build your first base while *CurrentTurn >= (int8_t)data[0], this facility is given for free.
    SEA_BASE_FREE = 93, // This facility is given for free to all Sea bases (at least 1 of those should probably give Submersion...)
    FIRST_BASE_FREE_TIME_WARP = 94, // In time warp starts, HQ starts with this facility.
    // AI Specific
    AI_HURRY_ECON_EAGER = 95, // AI likes to hurry this facility, for economic reasons.
    AI_TECH_VALUE_EARLY_ECO = 96, // AI has higher tech valuation for the technology that unlocks this facility.
    AI_RIOT_PREVENTION = 97, // AI prefers this facility to stop riots.
    // Misc
    DEFENDS_BASE = 98, // Probes favor targetting these facilities for destruction.
    PREQ_TECH_IS_CLIMACTIC = 99,
    CLEAN_MINERALS = 100, // data[0] added to the player's global Clean Mineral cap. Don't use on re-parse.
    NO_CAPTURE_DESTRUCTION = 101, // Facility may not be destroyed when base is captured.
    FREE_TECH = 102, // gain data[0] free techs on completion.
    SCORCHED_EARTH = 103, // Always destroyed on base capture, unless thinker.ini/capture_fix or recapture.


    // Global effects from projects - these are equal to their non-global version plus 128.
    // These are never parsed directly - instead, their value-128 is parsed into the faction's template base.
    GLOBAL_PREVENT_POP_LOSS = 129,
    GLOBAL_DEFENSE = 130,
    GLOBAL_MORALE_FLOOR = 131,
    GLOBAL_MORALE = 132,
    GLOBAL_PROBE_PLAGUE_RESISTANCE = 133,
    GLOBAL_HOMED_UNIT_MIND_CONTROL_RESISTANCE = 134,
    GLOBAL_REPAIR = 135,
    GLOBAL_PROTECT_RANGE_DROP_PODS = 136,
    GLOBAL_TELEPORT = 137,
    GLOBAL_GARRISON_IGNORE_NEGATIVE_SE_MORALE = 138,
    GLOBAL_NUKE_DEFENSE = 139,
    GLOBAL_MISSILE_DEFENSE = 140,
    GLOBAL_SENSOR = 141,
    GLOBAL_COUNTS_AS = 142,
    GLOBAL_SUBMERSION = 143,
    GLOBAL_POP_LIMIT = 144,
    GLOBAL_DRONES = 145,
    GLOBAL_TALENTS = 146,
    GLOBAL_SUPRPESS_PSYCH = 147,
    GLOBAL_ECO_DAMAGE_REDUCTION_TERRARFORM = 148,
    GLOBAL_ECO_DAMAGE_REDUCTION_MINERAL = 149,
    GLOBAL_BASE_SQ_INCOME = 150,
    GLOBAL_RESOURCE_PERCENT = 151,
    GLOBAL_FOREST_INCOME = 152,
    GLOBAL_IMPROVED_OCEAN_INCOME = 153,
    GLOBAL_FUNGUS_INCOME = 154,
    GLOBAL_SQUARE_INCOME = 155,
    GLOBAL_CLP_INCOME = 156,
    GLOBAL_CLP_COEFF = 157,
    GLOBAL_BASE_FINAL_LABS_MULT = 158,
    GLOBAL_FULL_SATELLITE_BONUS = 159,
    GLOBAL_CAN_BUILD_SATELLITE = 160,
    GLOBAL_NME_CAP = 161,
    GLOBAL_IMPROVED_LAND_INCOME = 162,
    GLBOAL_BASE_SIZE_INDICATOR_BORDER_COLOR = 163,
    GLOBAL_MIND_CONTROL_RESISTANCE = 164,
    GLOBAL_LINK_ALIEN_ARTIFACT = 165,
    GLOBAL_RETOOL_MAX_PENALTY_CLASS = 166,
    GLOBAL_PROTOTYPE_PENALTY_MULT = 167,
    GLOBAL_MULTIPLICATIVE_CREDITS = 168,
    GLOBAL_MULTIPLICATIVE_LABS = 169,
    GLOBAL_MULTIPLICATIVE_PSYCH = 170,
    GLOBAL_PLANET_FUNG_BONUS = 171,
    GLOBAL_LINK_INFINITE_ARTIFACTS = 172,
    GLOBAL_COMMERCE_TO_INCOME = 173,
    GLOBAL_POP_BOOM = 174,
    GLOBAL_HALF_MAINTENANCE = 175,
    GLOBAL_VIRTUAL_POLICE = 176,
    GLOBAL_SATELLITE_PROD_MULT = 177,
    GLOBAL_PREVENT_DRONE_RIOT = 178,
    GLOBAL_STOCKPILE_ENERGY_BONUS = 179,
    GLOBAL_EFFICIENCY_FLOOR_FLAT = 180,
    GLOBAL_EFFICIENCY_FLOOR_PERCENT = 181,
    GLOBAL_MORALE_MOD = 182,

    GLOBAL_COST_MULT = 190,
    GLOBAL_PREQ_COUNT_AS = 191,
    GLOBAL_FACILITY_COUNT = 192,

    // Global Effects that aren't a generalization of non-global ones. TODO: Implement everything below here.
    ACTIVE_PROBE_MORALE = 193, // Different from morale_probe on that this operates on active probes, instead of probes being built.
    TERRAFORM_UNLOCK = 194, // data is 24 bits, the 19 least significant of which are Former actions, from farm (0) to level terrain (18), as defined in engine_veh.h.
    COUNCIL_VOTES_MODIFIER = 196, // data[0]/data[1] is the multiplier for council votes for purpose of Govenor and Supreme Leader.
    COUNTS_AS_INFILTRATION = 197, // Project counts as permanent infiltration for all other factions.
    FUNGUS_REPAIR_AS_NATIVE = 198, // Repair faster on fungus, and up to full health.
    FUNGUS_MOVE_AS_NATIVE = 199, // Fungus squares act as road squares regarding movement.
    FUNGUS_COMBAT_AS_NATIVE = 200, // Gain Fungus combat advantages like natives.
    PSI_DEFENSE = 201, // data[0] to PSI defense.
    PSI_ATTACK = 202, // Data[0] to PSI attack.
    UNIT_SPEED = 203, // data[0] for magnitude, data[1] for Triad filter (Land/Sea/Air = 0/1/2)
    TECH_SHARE = 204, // data[0] notes how many other factions need to have the tech for it to be shared. NOT DECOMPILED YET.
    SE_FACTION = 205, // Increase faction-wide SE ratings. data[0] is effect size, data[2] has the SE effect index from engine_enums.h->SocialEffect.
    PROBE_IMMUNITY = 206, // Immune to probes. If data[0] = 2, this is total, if data[0]=1, then Algorithmic Enhancement probes can breach through. (though at lower success chances)
    REPAIR_SPEED = 207, // data[0] is repair speed modifier applied to all units.
    LIFT_REPAIR_LIMIT = 208, // Normally, field repairs cap at 80% of total HP. This sets that to 100.
    UNIT_UPGRADE_COST_MULT = 209, // data[0]/data[1] multiplier to upgrade costs.
    IMPUNITY = 210, // data[1] and data[0] contain 16 bit flags to signal impunities.
    POP_FLOOR = 211, // All bases start with this population. On construction, all bases are increased to this population, if smaller.
    DRONES_UNDER_POP_FLOOR = 212, // As (GLOBAL_)DRONES, but only works when population does not exceed POP_FLOOR (defaults to 1)
    ORBITAL_INSERTION = 213, // Unlock Orbital Insertion.
    MIND_CONTROL_MULTIPLIER = 214, // data[0]/data[1] mult to all mind control done by probes of the owning faction.
    ALL_PROBES_ALGORITHMIC_ENHANCEMENT = 215,
    LONGEVITY_VACCINE_EFFECTS = 216, // Way too specific for me to generalize now. This one is special cased to also be parsed on individual bases, but only where built.
    // Complex properties
    TERRAFORM_RATE = 220, // Same as Terraform Unlock, but the rate is 10*the 5 least significant bits for effect magnitude (10-310)

    // Truly Global Effects, doesn't matter which faction.
    VOICE_OF_PLANET_EFFECTS = 240, // Does several things, like interludes and stuff.
    ECO_DAMAGE_GLOBAL_MULT = 242, // Multiply all eco damage everywhere by data[0]/data[1].

    // No need to parse
    PROJECT_TIME_WARP_ENABLED = 251,
    TRANSCENDENCE_VICTORY = 252, // Win now.

};



struct BuilderProperty {
    // data[0-2] are type-dependent, data[3] is the type.
    uint8_t data[4] = {0, 0, 0, 0};

    BuilderPropertyType type() {return (BuilderPropertyType)data[3];}

};


struct BuilderFacility {
    uint64_t require_facilities = 0;
    uint64_t incompatible_facilities = 0;

    BuilderProperty properties[8];
    uint8_t prop_count = 0;
    int8_t facility_id = -1; // Default value indicates a "special" facility.


    bool is_satellite = 0;
    bool no_capture_destruction = 0;
    bool is_hq = 0;
    bool require_coast = 0;
    bool is_smax = 0;
    bool alien_exclusive = 0;
    bool require_transcend_victory_possible = 0;
    bool preq_tech_is_climactic = 0;
    bool scorched_earth = 0;
    // AI behavior props
    bool ai_hurry_econ_eager = 0;
    bool ai_tech_value_early_eco = 0;
    bool ai_riot_prevention = 0;
    bool defends_base = 0;
    // Free for...
    bool sea_base_free = 0;
    bool time_warp_free = 0;

    uint8_t require_pop_size = 0; // Required pop size to build.
    uint8_t require_project_completed = 0; // facility_id of the project
    uint8_t clean_minerals = 0;
    uint8_t free_techs = 0;

    int add_prop(BuilderProperty prop);
    bool is_free_mp(bool base_is_hq);
};

/*
BuilderBase is basically a cache object for every base_id that combines the data from all constructed facilities. It needs to be refreshed when a facility is removed or a base with a secret project is lost.
*/
struct BuilderBase {
    // Bookkeeping
    BuilderProperty properties[16];  // Stores complex properties that can't be loaded as struct members easily. (vanilla game maxes out at 6 of these)
    uint64_t facilities_parsed = 0;
    uint8_t property_count = 0;
    uint8_t faction_id = 0;
    // Gameplay relevant
    bool prevent_pop_loss = 0;
    bool repair_land = 0;
    bool repair_sea = 0;
    bool repair_air = 0;
    bool teleporter = 0;
    bool garrison_ignore_negative_se_morale_native = 0;
    bool garrison_ignore_negative_se_morale_non_native = 0;
    bool sensor = 0;
    bool submersion = 0;
    bool suppress_psych = 0;
    bool full_satellite_effect = 0;
    bool can_build_satellite = 0;
    bool is_hq = 0;
    bool link_alien_artifact = 0;
    bool alien_victory = 0;
    bool handle_event_growth = 0;
    bool handle_event_energy = 0;
    bool handle_event_netbonus = 0;
    bool handle_event_blight = 0;
    bool handle_event_prometheus = 0;
    bool link_infinite_artifacts = 0;
    bool pop_boom = 0;
    bool nuke_target = 0;
    bool half_maintenance = 0;
    bool prevent_drone_riot = 0;
    bool is_longevity_vaccine = 0;

    uint16_t land_defense = 0;
    uint16_t sea_defense = 0;
    uint16_t air_defense = 0;
    int16_t nutrient_percent = 0;
    int16_t mineral_percent = 0;
    int16_t energy_percent = 0;
    int16_t credits_percent = 0;
    int16_t labs_percent = 0;
    int16_t psych_percent = 0;
    uint8_t protect_range_drop_pod = 0;
    uint8_t protect_range_missile = 0;
    uint8_t missile_defense = 0;
    uint8_t protect_range_nuke = 0;
    uint8_t protect_chance_nuke = 0;
    uint8_t pop_limit = 0;
    uint8_t eco_damage_reduction_terraform = 0;
    uint8_t eco_damage_reduction_mineral = 0;;
    uint8_t base_sq_nutrients = 0;
    uint8_t base_sq_minerals = 0;
    uint8_t base_sq_energy = 0;
    uint8_t forest_nutrients = 0;
    uint8_t forest_minerals = 0;
    uint8_t forest_energy = 0;
    uint8_t improved_ocean_nutrients = 0;
    uint8_t improved_ocean_minerals = 0;
    uint8_t improved_ocean_energy = 0;
    uint8_t improved_land_nutrients = 0;
    uint8_t improved_land_minerals = 0;
    uint8_t improved_land_energy = 0;
    uint8_t fungus_nutrients = 0;
    uint8_t fungus_minerals = 0;
    uint8_t fungus_energy = 0;
    uint8_t square_nutrients = 0;
    uint8_t square_minerals = 0;
    uint8_t square_energy = 0;
    uint8_t flat_credits = 0;
    uint8_t flat_labs = 0;
    uint8_t flat_psych = 0;
    uint8_t final_labs_mult = 100; // Used for Punishment Sphere - must stay multiplicative!
    uint8_t yield_cap_nutrient = 0;
    uint8_t yield_cap_mineral = 0;
    uint8_t yield_cap_energy = 0;
    uint8_t base_pop_indicator_border_color = 0;
    uint8_t base_pop_indicator_border_thickness = 0;
    uint8_t retool_penalty = 3; // Game settings may override this
    uint8_t prototype_penalty_mult = 100;
    uint8_t planet_fung_bonus[4][3]; // Indexed first by clamp(SE_PLANET, 0, 3), then by yield-type (nutrient, mineral, energy)
    uint8_t virtual_police = 0;
    uint8_t efficency_flat = 0;
    uint8_t efficency_percent = 0;
    uint8_t garrison_morale_floor_offensive_native = 0;
    uint8_t garrison_morale_floor_offensive_non_native = 0;
    uint8_t garrison_morale_floor_defensive_native = 0;
    uint8_t garrison_morale_floor_defensive_non_native = 0;
    uint8_t garrison_morale_boost_offensive_native = 0;
    uint8_t garrison_morale_boost_offensive_non_native = 0;
    uint8_t garrison_morale_boost_defensive_native = 0;
    uint8_t garrison_morale_boost_defensive_non_native = 0;
    uint8_t land_morale = 0;
    uint8_t sea_morale = 0;
    uint8_t air_morale = 0;
    uint8_t probe_morale = 0;
    uint8_t lifecycle = 0;

    float probe_plague_resistance = 1.0;
    float homed_unit_mc_resist = 1.0;
    float mind_control_resistance = 1.0;
    float credits_multiplicative = 1.0;
    float labs_multiplicative = 1.0;
    float psych_multiplicative = 1.0;
    float satellite_production_mult = 1.0;
    float stockpile_energy_bonus = 1.0;

    int8_t drones = 0;
    int8_t talents = 0;
    int8_t SE_economy = 0;
    int8_t SE_efficiency = 0;
    int8_t SE_growth = 0;
    int8_t SE_industry = 0;
    int8_t SE_planet = 0;
    int8_t SE_police = 0;
    int8_t SE_probe = 0;
    int8_t SE_support = 0

    void add_facility(BuilderFacility* facility);
    void add_prop(BuilderProperty prop);
    void income_from_facility_count(int* credits, int* labs, int* psych);

    bool has_fac_parsed(int item_id) {
        // We need to decrement the ID, because indexation is 1-64 instead of the 0-63 that actually fits in a 64-bit int.
        return (item_id >= 0 && item_id <= Fac_ID_Last
            && facilities_parsed & (1 << (item_id - 1) ));
    }

    int cost_calc(int cost, int triad_or_native_or_probe);
    bool check_available(int facility_id);
};

/*
BuilderFaction contains the effects of all projects from that faction that have a global effect. However, this excludes those that can be parsed on individual bases - those with the GLOBAL_ prefix.
Otherwise, BuilderFaction is a cache object that combines the information of all projects constructed by a faction. It needs to be refreshed when a base containing a secret project is lost.
*/
struct BuilderFaction {
    uint64_t projects_parsed = 0; // Track to avoid duplicates
    BuilderProperty properties[8] = {};  // Stores complex properties that can't be loaded as struct members easily (just terraform rate).
    uint8_t property_count = 0;
    uint8_t faction_id = 0;

    bool infiltration = 0;
    bool fungus_move_as_native = 0;
    bool fungus_repair_as_native = 0;
    bool fungus_combat_as_native = 0;
    bool lift_repair_limit = 0;
    bool orbital_insertion = 0;
    bool algorithmic_enhancement = 0;
    bool longevity_vaccine = 0;
    bool voice_of_planet = 0;

    uint8_t active_probe_morale = 0;
    uint8_t land_speed = 0;
    uint8_t sea_speed = 0;
    uint8_t air_speed = 0;
    uint8_t tech_share = 10;
    uint8_t probe_immunity = 0; // 1=vanilla HSA, 2=SMAC HSA (no bypass)
    uint8_t repair_speed = 0;
    uint8_t pop_floor = 1;
    uint8_t psi_defense = 0;
    uint8_t psi_attack = 0;

    int8_t drones_while_pop_floor = 0;
    int8_t se_economy = 0;
    int8_t se_efficiency = 0;
    int8_t se_support = 0;
    int8_t se_talent = 0;
    int8_t se_morale = 0;
    int8_t se_police = 0;
    int8_t se_growth = 0;
    int8_t se_planet = 0;
    int8_t se_probe = 0;
    int8_t se_industry = 0;
    int8_t se_research = 0;

    uint32_t terraform_unlock = 0;
    uint16_t impunity = 0; // 16-bitarray read as (4xSocialCategory + SocialModel) (engine__enums.h)

    float council_votes_mod = 1.0;
    float unit_upgrade_cost_mult = 1.0;
    float mind_control_multiplier = 1.0;

    void add_project_property(BuilderProperty prop);
    int terraform_rate(int rate, FormerItem terraform); // Returns multiplier
};


void reset_builder_factions(); // Also resets all bases - call this on game load/init.
void reset_builder_faction(int faction_id); // Same as above, but only for 1 faction.

void rebuild_builder_base(int base_id);
void delete_builder_base(int base_id); // Called on mod_base_kill, removes base from the list, moves data of higher id bases. Checks for projects and initiates any recalculates.
BuilderBase* init_builder_base(int base_id); // On establishing new bases. Copies from template.
void construct_facility(int base_id, int facility_id); // On facility construction. Checks for projects, and sends out any neccesary updates for those.
void builder_init();


const int ComplexPropertyFirst = 60;
const int ComplexPropertyLast = 64;
const int BasePropertyLast = 73;
const int FacilityTypeGlobalFirst = 129;
const int FacilityTypeGlobalUniqueFirst = 193;
const int FacilityTypeGlobalLast = 239;
const int GlobalPropertyTypeOffset = 128;
BuilderFacility builder_facilities[MaxFacilityNum]; // Indexed by [engine_enums.h->FacilityId - 1]
BuilderFacility builder_projects[MaxSecretProjectNum]; // Indexed by facility_id - SP_ID_First.
BuilderFacility special_facilities[8]; // Special facilities. faction_id leads to their "every base has this" facility, while index 0 has global all. Maybe add Golden Age and Drone Riots later?
BuilderBase builder_base_templates[8];  // Frefab "empty" base for each faction, constructed from owned projects. Indexed by faction_id. 0 is the fully empty "prefab".
BuilderBase builder_bases[MaxBaseNum * MaxPlayerNum];  // Indexed by the same id as BASE array.
BuilderFaction builder_factions[8]; // 1 prefab "emtpy" in slot 0, the other 7 are indexed by faction_id.
BuilderFacility* HQ_FACILITY = nullptr; // Pointer should be set when parsing facilities.

// Ini parsing stuff
typedef int (*string_transformer)(const char* name);



#pragma pack(pop)
