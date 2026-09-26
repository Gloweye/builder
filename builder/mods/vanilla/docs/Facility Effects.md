# Facility Effects
This document will explain all facility effects that the Builder mod offers users.

Values in parentheses are optional. 

Facilities are currently hard limited to 8 effects. 

The explanations often talk about "multiple facilities stack additively", or similar wording. This also means that 
having the same effect multiple times on the same facility stacks in the same way.

Note that the moddable facilities only applies to those constructed in a city - this excludes Stockpile Energy and the
three satellites. These are special-cased everywhere, and `alphax.txt` allows for all possible customization.

## Globalizable Effects
Most effects fall into this category. These effects make sense on a per-base level, but also on all-bases. 

The effect name may be assigned to a Secret Project with the `global_` prefix in front, and it will affect all bases.

For instance, the Hab Complex facility uses the `population_limit` property; but The Ascetic Virtues uses
`global_population_limit` to apply a similar effect to all bases. 

### Garrison Morale
Increases Morale (veterancy) for units garrisoned in the base. Final calculation clamps between 0 and 6.
Multiple facilities are additive.
 - Key: `garrison_morale_mod`
 - Value: integer, (string), (string)
   - integer 0-6: Added to unit morale.
   - string: may be NATIVE, NON_NATIVE, ATTACKER, or DEFENDER. Effect will only apply to units that meet all 
conditions. (Attackers cannot benefit if "DEFENDER" has been configured.) Leave empty to affect all units in base 
square.
 - Vanilla usage: Children's Creche, Brood Pit

### Garrison Morale Floor
Enforces a minimum Morale (veterancy) level for units in the base. Applies after other morale modifiers. Values go from
0 (Very Green/Hatchlings) to 6 (Elite/Demon Boil) 
With multiple facilities, the best (for the base owner) applies.
 - Key: `base_morale_floor`
 - Value: integer, (string), (string)
   - integer 0-6: New morale floor (0 = Very Green, 2 = 0% combat bonus, 6 = Elite).
   - string: may be NATIVE, NON_NATIVE, ATTACKER, or DEFENDER. Effect will only apply to units that meet all 
conditions. (Attackers cannot benefit if "DEFENDER" has been configured.) Leave empty to affect all units in base 
square.
 - Vanilla usage: None

### Garrison Ignore Negative SE Morale
Units stations in the base ignore negative Morale from SE. This means that even if they have 0 (Very Green), they fight
as if they have 2 (Disciplined).
Multiple facilities are redundant.
 - Key: `garrison_ignore_negative_se_morale`
 - Value: string
   - string: NATIVE, NON_NATIVE, or ALL. The configured group does not suffer detrimental effects from morale.
 - Vanilla usage: Children's Creche, Brood Pit

### Prevent Population Loss
When a base is attacked, normally it loses 1 population. This property makes the base not lose anything.
Multiple facilities are redundant.
 - Key: `base_prevent_pop_loss`
 - Value: None
 - Vanilla usage: Perimeter Defense

### Base Defense
Increases the defense value of the base against the specified Triad.
Multiple facilities stack additively up to 65535%.
 - Key: `base_defense`
 - Value: integer, string
   - integer 0-255: Base defense increase in % (100 = double defense)
   - string: Triad - must be LAND, SEA or AIR.
 - Vanilla usage: Perimeter Defense, Naval Yard, Aerospace Complex, Tachyon Field

### Morale
Increases the Morale or Lifecycle level of units produced in this base.
For Native units, this also counts as "repair" facilities. 
For Triad units, this bonus is halved by having a Morale SE value of -2 or less.
Multiple facilities are additive.
 - Key: `morale`
 - Value: string, integer
   - string: NATIVE, LAND, SEA, AIR or PROBE. Probe and Natives don't benefit from triad morale.
   - integer 0-6: Modifier. Vanilla uses 2 for Triad, 1 for Native.
 - Vanilla usage: Command Center, Naval Yard, Biology Lab, Xenoempathy Dome, and more.

### Resist Genetic Plague Probe Action
Reduces suffered impact from hostile probes spreading plagues.
Value is interpreted as a fraction of numerator/demoninator.
Multiple facilities are multiplicative.
 - Key: `probe_plague_resistance`
 - Value: integer, integer
   - integer 0-255: First integer multiplies impact of the plague.
   - integer 1-255: Second integer divides impact of the plague.
 - Vanilla usage: Research Hospital, Nanohospital (both halve impact, reducing pop loss and unit damage)

### Mind Control Resistance (base)
Multiplies the "distance from HQ" factor in the calculation for mind control price for this base. Higher values
mean that the base is more expensive to subvert. (but not harder to do so)
Multiple facilities are multiplicative.
 - Key: `mind_control_resistance`
 - Value: integer, integer
   - integer 0-255: Multiplier
   - integer 1-255: Divisor
 - Vanilla usage: Children's Creche, Punishment Sphere, Genejack Factory (inverted, so cheaper instead of expensive)

### Mind Control Resistance (remote units)
Increases the price of the mind control action for hostile probes, when used against units supported by this base.
This affects the distance factor, which is one small part of a large, complicated equation.
Multiple facilities are multiplicative.
 - Key: `remote_unit_mind_control_resistance`
 - Value: integer, integer
   - integer 0-255: Multiplier
   - integer 1-255: Divisor
 - Vanilla usage: Punishment Sphere

### Repair
Sets this base to heal units of this Triad faster, as specified in `thinker.ini` by the `repair_base_facility` field.
Native units do not use this; instead any facility increasing their Lifecycle (See [Lifecycle](#Morale)) will
trigger the enhanced bonus repair rate. (`repair_base_native` from `thinker.ini`)
Multiple facilities are redundant.
 - Key: `repair`
 - Value: string
   - string: LAND, SEA or AIR.
 - Vanilla usage: Command Center, Naval Yard, Aerospace Complex

### Drop Pod Protection
Protects the base by disabling Drop Pod usage in a radius around the base. 
With multiple facilities, the best (for the base owner) applies.
 - Key: `prevent_drop_pod_range`
 - Value: integer
   - integer 1-8: Range to protect (vanilla uses 2).
 - Vanilla usage: Aerospace Complex

### Teleporter
Marks this base as having a Teleporter. Doesn't allow much customization, behavior is very hard-coded in the base game.
Multiple facilities are redundant.
 - Key: `is_teleporter`
 - Value: None
 - Vanilla usage: PSI Gate

### Cost Multiplier
Modify the cost (in minerals) of a Triad of units. Value is a % multiplier of the unit cost. Note that values 49 and 
lower will result in a net mineral gain when building+disbanding units. For supply crawlers, this is 99 and lower. 
Use with caution.
Multiple facilities are multiplicative.
 - Key: `cost_mult`
 - Value: string, integer
   - string: LAND, SEA, AIR, NATIVE or PROBE.
   - integer 0-255:
 - Vanilla usage: Brood Pit

### Planet Buster Defense
Defends the base against Planet Buster attacks. Base defenses are processed after Orbital Defense Pods, but before they
are sacrificed. Since the base facility is not an Orbital Defense Pod and has no "deployed" state, this is reusable in
the same turn multiple times.
This protects not just the base, but also the surrounding area.
With multiple facilities, the best range (for the base owner) applies.
With multiple facilities, the chances are calculated independently.
With multiple bases in range of the attacked tile, each has their defense chance calculated separately.
 - Key: `nuke_defense`
 - Value: integer, integer
   - integer 0-8: Range from the base where the protection applies. (vanilla: 2)
   - integer 0-100: % chance of a Planet Buster being neutralized. (vanilla: 50)
 - Vanilla usage: Flechette Defense System

### Missile Defense
Provides a defensive combat boost against conventional (non-nuke) missile attacks. Protects not just the base, but also
the surrounding area.
With multiple facilities, the best range (for the base owner) applies.
With multiple facilities, the boosts apply independently, up to 250% per base.
With multiple bases in range of the attacked tile, each has their defense boost applied separately.
 - Key: `missile_defense`
 - Value: integer, integer
   - integer 0-8: Range from the base where the combat bonus applies (vanilla: 2)
   - integer 0-250: % Defense boost magnitude (vanilla: 50)
 - Vanilla usage: Flechette Defense System

### Sensor
The base will count as a Sensor for both line of sight, and defensive combat bonus in the same area as the terrain
improvement as constructed by formers (range=2, defense_boost=25%)
Multiple bases/facilities are redundant.
 - Key: `is_sensor`
 - Value: None
 - Vanilla usage: Geosynchronous Survey Pod

### Counts As 
Facilities may "count as" another facility. This makes the game as as if the other facility is also constructed.
An optional second parameter acts as a filter (The Virtual World counts as a Hologram Theater in bases that already 
have a Network Node)
Multiple facilities are redundant.
 - Key: `counts_as`
 - Value: string(, string)
   - string: Facility that should be considered constructed, by name.
   - (string): Only apply if this Facility (by name) is present.
 - Vanilla usage: Pressure Dome, The Virtual World, The Command Nexus, Many Secret Projects

### Submersion
Allows this base to exist on a sea tile. 
Multiple facilities are redundant.
 - Key: `submersion`
 - Value: None
 - Vanilla usage: Pressure Dome

### Population Limit
Base Population may not grow beyond this. 
Multiple Facilities are additive up to 127 (hardcoded engine cap).
 - Key: `population_limit`
 - Value: integer
   - integer 1-127: Increase limit by this much.
 - Vanilla usage: Hab Complex, Habitation Dome, The Ascetic Virtues

### Drones
Creates this many extra drones in the base.
May be negative, in which case it makes citizens content instead.
Multiple facilities are additive.
 - Key: `drones`
 - Value: integer
   - integer -128 to 127: Add this many drones to the base.
 - Vanilla usage: Genejack Factory (adds drones), Recreation Commons (reduces drones)

### Talents
Creates this many extra talents in the base.
May be negative, in which case it turns talents content instead.
Multiple facilities are additive.
 - Key: `talents`
 - Value: integer
   - integer -128 to 127: Add this many talents to the base.
 - Vanilla usage: Paradise Garden, The Human Genome Project

### Suppress Psych
Disallow certain categories of citizens in a base.
Multiple facilities combine their effects.
 - Key: `suppress_psych`
 - Value string(, string): Valid are DRONES and TALENTS, seperated by a comma if both. 
 - Vanilla usage: Punishment Sphere

### Terraform Eco Damage Reduction
Reduce eco damage from working terraformed tiles by this %. In the base game, building a Tree Farm and Hybrid Forest
adds up to 100, which in turn reduces the eco damage from this source to 0.
Multiple facilities are additive.
 - Key: `terraform_eco_damage_reduction`
 - Value: integer
   - integer 1-100: Eco Damage reduction.
 - Vanilla usage: Tree Farm, Hybrid Forest

### Mineral Eco Damage Reduction
Adds to the divisor of the big equation in 
[Ecology (Revised)](https://alphacentauri.miraheze.org/wiki/Ecology_(Revised)) (external link). Progressively reduces
ecological damage as a consequence of production, but cannot eliminate it. Nevertheless, high values may trivialize it
anyway.
Multiple facilities are additive.
 - Key: `mineral_eco_damage_reduction`
 - Value: integer
   - integer 1-255: DamageReductionInBase field of the above link. (vanilla: 1)
 - Vanilla usage: Centauri Preserve, Temple of Planet, Quantum Converter, The Pholus Mutagen

### Social Engineering (base)
Adds to the value of the noted category, when it concerns base operation.
Morale and Research are not available because they are not referred to in the context of an individual base.
Multiple facilities are additive, to a per-base total of -10 or 10 (beyond which it has no effect for most categories).
 - Keys: `base_se_efficiency`, `base_se_growth`, `base_se_probe`, `base_se_police`, `base_se_economy`,
`base_se_industry`, `base_se_support`, `base_se_planet`
 - Value: integer
   - integer -10 to 10: Modifier to the SE category
 - Vanilla usage: Children's Creche (Efficiency, Growth), Covert Ops Center (probe), Brood Pit (police), 
Genejack Factory (negative probe)

### Population Boom
Puts the base into a permanent state of Population Boom, just like having +6 Growth.
Multiple facilities are redundant.
 - Key: `pop_boom`
 - Value: None
 - Vanilla usage: The Cloning Vats


### Halve Maintenance
Reduces the maintenance of the base' facilities by half, exactly. This is hardcoded to handle rounding errors, and 
therefore leaves little room for customization.
Multiple facilities are redundant.
 - Key: `half_maintenance`
 - Value: None
 - Vanilla usage: The Self-Aware Colony

### Virtual Police
Adds a number of virtual police units when calculating base psych. These are assumed to NOT have the non-lethal methods
special ability, and are subject to the effects of the faction's POLICE social engineering rating and the maximum of
police units present. 
Multiple facilities are additive up to 3.
 - Key: `virtual_police`
 - Value: integer
   - integer 1-3: Number of virtual police present.
 - Vanilla usage: The Self-Aware Colony

### Prevent Drone Riots
The base may still have drones, but they cannot riot.
Multiple facilities are redundant.
 - Key: `prevent_drone_riot`
 - Value: None
 - Vanilla usage: The Telepathic Matrix

### Base Nutrients/Minerals/Energy
Increases Nutrient/Mineral/Energy yield of the base square itself. This square always ignores the income caps, and
is a reliable way to give a bonus to a base exactly once.
Multiple facilities are additive up to 255.
 - Key: `base_nme`
 - Value: integer, integer, integer
   - integer 0-250: Nutrient income
   - integer 0-250: Mineral income
   - integer 0-250: Energy income
 - Vanilla usage: Recycling Tanks

### Nutrients/Minerals/Energy Percent Bonus
Increases Nutrient/Mineral/Energy yield of the base as a whole by a %. In vanilla, this mechanic is only used for 
minerals, by the Robotic Assembly Plant and similar facilities.
Multiple facilities are additive up to 65000% per base.
 - Key: `resource_percent`
 - Value: integer, integer, integer
   - integer 0-250: Nutrient % income bonus
   - integer 0-250: Mineral % income bonus
   - integer 0-250: Energy % income bonus
 - Vanilla usage: Robotic Assembly Plant, Nanoreplicator, Genejack Factory, Quantum Converter, The Bulk Matter 
Transmitter

### Forest Nutrients/Minerals/Energy
Increases the Nutrient/Mineral/Energy yield of forest squares. 
Multiple facilities are additive up to 255.
 - Key: `forest_nme`
 - Value: integer, integer, integer
   - integer 0-250: Nutrient income
   - integer 0-250: Mineral income
   - integer 0-250: Energy income
 - Vanilla usage: Tree Farm, Hybrid Forest

### Improved Ocean Nutrients/Minerals/Energy
Increases the Nutrient/Mineral/Energy yield of ocean tiles with a Kelp Farm/Mining Platform/Tidal Harness present 
respectively. 
Multiple facilities are additive up to 255.
 - Key: `improved_ocean_nme`
 - Value: integer, integer, integer
   - integer 0-250: Nutrient income for Kelp Farms
   - integer 0-250: Mineral income for Mining Platforms
   - integer 0-250: Energy income for Tidal Harnesses
 - Vanilla usage: Aquafarm, Subsea Trunkline, Thermocline Transducer

### Improved Land Nutrients/Minerals/Energy
Increases the Nutrient/Mineral/Energy yield of ocean tiles with a Farm/Mine/Solar Collector present 
respectively. 
Multiple facilities are additive up to 255.
 - Key: `improved_land_nme`
 - Value: integer, integer, integer
   - integer 0-250: Nutrient income for Farms
   - integer 0-250: Mineral income for Mines
   - integer 0-250: Energy income for Solar Collectors
 - Vanilla usage: None

### Fungus Nutrients/Minerals/Energy
Increases the Nutrient/Mineral/Energy yield of (sea) fungus squares. 
Multiple facilities are additive up to 255.
 - Key: `fungus_nme`
 - Value: integer, integer, integer
   - integer 0-250: Nutrient income
   - integer 0-250: Mineral income
   - integer 0-250: Energy income
 - Vanilla usage: None

### Non-Fungus Nutrients/Minerals/Energy
Increases the Nutrient/Mineral/Energy yield of non-fungus squares. 
Multiple facilities are additive up to 255.
 - Key: `square_nme`
 - Value: integer, integer, integer
   - integer 0-250: Nutrient income
   - integer 0-250: Mineral income
   - integer 0-250: Energy income
 - Vanilla usage: The Merchant Exchange

### Nutrients/Minerals/Energy Cap
Increases the per-tile yield cap of Nutrient/Mineral/Energy.
Multiple facilities are additive up to 255.
 - Key: `nme_cap`
 - Value: integer, integer, integer
   - integer 0-250: Max Nutrient/square
   - integer 0-250: Max Mineral/square
   - integer 0-250: Max Energy/square
 - Vanilla usage: None

### Base Credits/Labs/Psych
Increases per-base income of Credits, Labs, and Psych.
Multiple facilities are additive up to 255.
 - Key: `base_clp`
 - Value: integer, integer, integer
   - integer 0-255: Flat Credits income
   - integer 0-255: Flat Labs income
   - integer 0-255: Flat Psych income
 - Vanilla usage: Biology Lab

### Credits/Labs/Psych Percent Bonus
Increases % of Credits, Labs, and Psych. Strict bonus, cannot be used to reduce income.
Multiple facilities are additive up to 65000%.
 - Key: `coeff_clp`
 - Value: integer, integer, integer
   - integer 0-255: Percent Credits bonus
   - integer 0-255: Percent Labs bonus
   - integer 0-255: Percent Psych bonus
 - Vanilla usage: Network Node, Energy Bank, Tree Farm

### Base Final Labs Mult
A % multiplier to labs. In vanilla, this is used so a Punishment Sphere cannot be trivially offset with a Network Node.
Multiple facilities are multiplicative between 0 and 250%.
 - Key: `base_final_labs_mult`
 - Value: integer
   - integer 0-255: Percent Labs modifier.
 - Vanilla usage: Punishment Sphere

### Full Satellite Value
Satellite bonuses are halved for bases without a facility that gives this. But because this cap is applied before the 
population cap, this is irrelevant for bases with a population up to half the number of satellites for that income type.
Multiple facilities are redundant.
 - Key: `full_satellite_value`
 - Value: None
 - Vanilla usage: Aerospace Complex, The Space Elevator

### Multiplicative Credits
Gives a fraction multiplier to credits income. Stacks multiplicatively with more instances and with Credits/Labs/Psych 
Percent Bonus. Due to its heavy impact, vanilla limits the use of this to secret projects.
Multiple facilities stack multiplicative.
 - Key: `mult_credits`
 - Value: integer, integer
   - integer 0-255: Multiplier
   - integer 1-255: Divisor
 - Vanilla usage: The Space Elevator (base-local)

### Multiplicative Labs
Gives a fraction multiplier to labs income. Stacks multiplicatively with more instances and with Credits/Labs/Psych 
Percent Bonus. Due to its heavy impact, vanilla limits the use of this to secret projects.
Multiple facilities stack multiplicative.
 - Key: `mult_labs`
 - Value: integer, integer
   - integer 0-255: Multiplier
   - integer 1-255: Divisor
 - Vanilla usage: The Supercollider, The Universal Translator (both base-local)

### Multiplicative Psych
Gives a fraction multiplier to psych income. Stacks multiplicatively with more instances and with Credits/Labs/Psych 
Percent Bonus. Due to its heavy impact, vanilla limits the use of this to secret projects.
Multiple facilities stack multiplicative.
 - Key: `mult_psych`
 - Value: integer, integer
   - integer 0-255: Multiplier
   - integer 1-255: Divisor
 - Vanilla usage: None

### Facility Count Income
Gives a certain income based on how many of a certain facility are built across planet, regardless of owner. The result
is unaffected by other facilities.
Multiple facilities stack additively.
 - Key: `fac_count`
 - Value: string, string
   - string: Facility to count, by name.
   - string: CREDITS, LABS or PSYCH.
 - Vanilla usage: The Network Backbone

### Planet Fungus Bonus
Increases fungus yield when Planet Social Engineering Rating exceeds a certain number.
Multiple facilities stack additively up to 250 per Planet level.
 - Key: `planet_fung_bonus`
 - Value: integer, string, integer
   - integer 0-3: Minimum Planet Value to trigger bonus (Setting this to 1 will benefit factions with 2 planet)
   - string: Bonus type - NUTRIENT, MINERAL or ENERGY
   - integer 0-250: Effect Magnitude.
 - Vanilla usage: The Manifold Harmonics

### Commerce to Income
Adds Commerce to a designated clp income.
Multiple facilities stack additively.
 - Key: `commerce_as_income`
 - Value: string
   - string: Income type - CREDITS, LABS or PSYCH
 - Vanilla usage: The Network Backbone

### Satellite Production Bonus
This increases the mineral production of the base while actively constructing Satellite improvements. 
Multiple facilities stack multiplicatively.
 - Key: `satellite_production_mult`
 - Value: integer, integer
   - integer 0-255: Multiplier to production.
   - integer 1-255: Divisor of production.
 - Vanilla usage: The Space Elevator

### Stockpile Energy Bonus
This modifies the efficiency of Stockpile Energy. Unmodified value is 50% (2 minerals -> 1 energy credit.)
Multiple facilities stack multiplicatively.
 - Key: `stockpile_energy_bonus`
 - Value: integer, integer
   - integer 0-255: Multiplier of energy credits.
   - integer 1-255: Divisor of energy credits.
 - Vanilla usage: The Planetary Energy Grid (using 5/4 for 20% bonus)

### Efficiency Floor (flat)
Enforces a minimum amount of energy that cannot be lost to inefficiency. You may still earn less energy if it never
gets produced in the first place. 
Multiple facilities are additive.
 - Key: `efficiency_floor_flat`
 - Value: integer
   - integer 1-250: Increase to the flat efficiency floor.
 - Vanilla usage: None

### Efficiency Floor (percent)
Enforces a minimum percentage of energy that cannot be lost to inefficiency. This is calculated as a % of the total
energy income of a base - so for a base generating 40, a floor of 10 safeguards 4 energy, and a base generating 151 has
at least 16 energy escape inefficiency.
Multiple facilities are additive.
 - Key: `efficiency_floor_percent`
 - Value: integer
   - integer 1-100: % of Energy safeguarded.
 - Vanilla usage: None

### Base Population Indicator Border
Sets the width and color of the border of the population size indicator.
Multiple facilities interact unpredictably.
 - Key: `base_size_indicator_border_color`
 - Value: integer, integer
   - integer: Border color. Must be a color definition in `gui.h`, otherwise may God be with you (I won't be).
   - integer 1-3: Border width.
 - Vanilla usage: Headquarters

### Link Alien Artifact
Allows 1 alien artifact to be linked to this base for a free tech.
Much of this is hardcoded, so this is just a yes/no option.
Multiple facilities are redundant.
 - Key: `link_alien_artifact`
 - Value: None
 - Vanilla usage: Network Node

### Link Infinite Alien Artifacts
Allows any number of alien artifacts to be linked to this base for a free tech each.
 - Key: `link_infinite_artifacts`
 - Value: None
 - Vanilla usage: The Universal Translator

### Alleviate Retool Penalty
When above a certain number of minerals (10 by default), changing production may lose you half of the minerals above 
that threshold. This effect lets you reduce the cases where this penalty applies.
With multiple facilities, the best applies.
 - Key: `retool_max_penalty`
 - Value: integer
   - 0: Always Free
   - 1: Free in category (Categories are Units/Facilities/Secret Projects)
   - 2: Free for switching between Secret Projects only
   - 3: Never Free
 - Vanilla usage: Skunkworks(free in category)

### Prototype Penalty Multiplier
Modifies the mineral penalty suffered when creating prototypes. By default, this is an additional 50% cost for the unit.
This modifier is a % modifier to the 50% - so if this is set to "50%", the net penalty is 25%, and if set to 200, the
net penalty is 100% of unit cost.
Multiple facilities are multiplicative.
 - Key: `prototype_penalty_mult`
 - Value: integer
   - integer 0-255: % modifier to the 50% cost penalty.
 - Vanilla usage: Skunkworks (sets to 0)

### Can Build Satellite
Marks this base as capable of building satellites.
Multiple facilities are redundant.
 - Key: `can_build_satellite`
 - Value: None
 - Vanilla usage: Aerospace Complex, The Space Elevator (global)

### Special Event Handling
There are 5 events that play out differently based on facilities present in randomly selected bases. Generally, having
the facility turns it from a negative into a positive one.
None of them have values, so this will list keys and vanilla usage.
 - `handle_growth_event` - Children's Crèche
 - `handle_energy_event` - Energy Bank
 - `handle_netbonus_event` - Network Node
 - `handle_blight_event` - Biology Lab
 - `handle_prometheus_event` - Research Hospital/Nanohospital/The Human Genome Project/Clinical Immortality





## Facility Only Effects
These properties do not make sense with a `global_` prefix, for a variety of reasons. They still only affect the 
facility or the base, and not the entire faction.

Most of these properties do not count towards the 8-property limit for facilities, but exceptions will say they do.

### Scorched Earth
When this base is captured by an enemy faction, facilities with this property are destroyed.
 - Key: `scorched_earth`
 - Value: None
 - Vanilla usage: Recycling Tanks, Recreation Commons

### No Capture Destruction
On base capture, normally a random selection of facilities is destroyed. Facilities marked with this cannot be 
selected.
 - Key: `no_capture_destruction`
 - Value: None
 - Vanilla usage: Pressure Dome

### Clean Minerals
On construction, add this number to the player's global Clean Minerals counter, as per 
[Ecology (Revised)](https://alphacentauri.miraheze.org/wiki/Ecology_(Revised)) (external link).
Higher values will quickly trivialize eco-damage from minerals as a concept.
Unlike [Mineral Eco Damage](#Mineral-Eco-Damage-Reduction), this may eliminate mineral eco damage entirely.
Constructing this facility more often stacks infinitely up to 32-bit integer limit.
 - Key: `clean_minerals`
 - Value: integer
   - integer 1-255: Added to clean minerals faction value. (vanilla: 1)
 - Vanilla usage: Tree Farm, Hybrid Forest, Centauri Preserve, Temple of Planet

### Incompatibilities
Specify that a facility may NOT be built when a named other facility is present. This is a hard requirement that also
applies to human players.
 - Key: `incompatible_fac`
 - Value: string
   - string: Facility, by name, that prevents the construction of the other.
 - Vanilla usage: Recycling Tanks(Pressure Dome, 1-way, because redundant), Paradise Garden<->Punishment Sphere

### Requirements
Specify that another facility MUST be present before the current facility may be built. This is a hard requirement that
also applies to human players.
In the base game, this is often used to require basic/cheaper versions before their upgraded counterparts, like Tree
Farm and Hybrid Forest, or Centauri Preserve and Temple of Planet.
 - Key: `require_fac`
 - Value: string
   - string: Facility, by name, which is the requirement for the current facility.
 - Vanilla usage: Tree Farm->Hybrid Forest, Research Hospital->Nanohospital, Robotic Assembly Plant->Quantum Converter

### Count as Requirement
For the purposes of [Requirements](#Requirements), this facility counts as another facility. 
This allows to use `require_fac` to depend on facility A OR facility B. (for an AND situation, just have two 
`require_fac` fields).
 - Key: `preq_count_as`
 - Value: string
   - string: Facility, by name, that doesn't have to be present as long as this one is.
 - Vanilla usage: Robotic Assembly Plant(Genejack Factory)->Nanoreplicator

### Require Coast
Only allow this facility to be built in Coastal cities (an adjacent sea tile, so ships can move in or out).
 - Key: `require_coast`
 - Value: None
 - Vanilla usage: Naval Yard, Aquafarm

### Is Alien Crossfire
Marks this facility as being Alien Crossfire exclusive. If the Thinker option `smac-in-smax` is used, these facilities
are disabled.
 - Key: `is_smax`
 - Value: None
 - Vanilla usage: All SMAX facilities and secret projects.

### Alien Exclusive
Marks this facility as requiring being one of the Alien factions to build. 
 - Key: `alien_exclusive`
 - Value: None
 - Vanilla usage: Subspace Generator(alien-only victory condition)

### Require Pop Size
Marks this facility as requiring a certain population size to build.
 - Key: `require_pop_size`
 - Value: integer
   - integer 0-127: Required population size
 - Vanilla usage: Subspace Generator

### Alien Victory
Fulfils the alien victory condition when a specified amount of this facility are constructed.
 - Key: `victory_when_built`
 - Value: integer
   - integer 0-255: How many must be constructed to trigger victory
 - Vanilla usage: Subspace Generator

### Is Satellite
Marks this facility as a satellite, benefitting from [`satellite_production_mult`](#satellite-production-bonus), but 
requiring [`can_build_satellite`](#can-build-satellite) to construct.
Also manages some other aspects, like being constructed repeatedly. 
 - Key: `is_satellite`
 - Value: None
 - Vanilla usage: Geosynchronous Survey Pod, Nutrient/Mineral/Energy satellites

### Require Transcendence Victory Possible
These facilities are disabled when Transcendence Victory is.
 - Key: `require_transcend_victory_possible`
 - Value: None
 - Vanilla usage: Voice of Planet, Ascent to Transcendence

### Require Project Complete
Require a named project to be completed. Doesn't have to be the same player.
 - Key: `require_project_complete`
 - Value: string
   - string: Project to already be complete
 - Vanilla usage: Ascent to Transcendence

### Is Headquarters
Marks this facility as the Headquarters facility. This has many effects:
 - Limited to 1 per player
 - Immune to inefficiency, increases other bases' inefficiency based on distance
 - Immune to Probe Mind Control, increases other bases' mind control price based on distance
 - Is granted for free if you build a base while having no bases.

And more. 
This property DOES count towards the property limit. You may not put this property on multiple facilities. 
 - Key: `is_hq`
 - Value: None
 - Vanilla usage: Headquarters

### Free Tech
Gives a number of free technologies on construction.
Multiple facilities work independently.
 - Key: `free_tech`
 - Value: integer
   - integer 0-255: How many free techs
 - Vanilla usage: The Universal Translator

### Multiplayer Free Facilities
Free facilities for the first base when playing multiplayer.
 - Key: `first_base_free_mp`
 - Value: None
 - Vanilla usage: Recycling Tanks
 - Key: `first_base_free_mp_difficulty`
 - Value: integer
   - integer 0-6: Free when difficulty is this integer or higher (prevent drone riots in starting base)
 - Vanilla usage: Recreation Commons
 - Key: `first_base_free_mp_se_positive`
 - Value: string
   - string: SE category, by name, that if positive at the start, gives this facility for free
 - Vanilla usage: Energy Bank (Economy category)

### Free for first base after turn count
Used to help get restarted factions going again.
 - Key: `first_base_free_turn`
 - Value: integer
   - integer: Turn number after which this facility is free.
 - Vanilla usage: Recycling Tanks, Recreation Commons, Energy Bank

### Free for Sea Bases
Since a sea base can't exist without the [`submersion`](#submersion) property, it needs to be given for free on 
construction.
 - Key: `sea_base_free`
 - Value: None
 - Vanilla usage: Pressure Dome

### Free when playing with Time Warp
Time Warp is a feature to help people skip the boring early turns. It gives facilities with this property for free,
but only in their HQ.
 - Key: `time_warp_free_hq`
 - Value: None
 - Vanilla usage: Recycling Tanks, Perimeter Defense, Recreation Commons, Network Node

### AI Treats this as early economic facilities
AI is more likely to hurry this facility when it wants to progress economically.
Only works if the AI already selected the facility, it doesn't encourage selection.
 - Key: `ai_hurry_econ_eager`
 - Value: None
 - Vanilla usage: HQ, Recycling Tanks, Tree Farm, Pressure Dome

### AI Tech Value When Trading
The prerequisite tech of this facility is higher valued by the AI when trading technology in the early game.
 - Key: `ai_tech_value_early_eco`
 - Value: None
 - Vanilla usage: Recycling Tanks, Recreation Commons, Children's Crèche

### AI Riot Prevention
The purpose of these facilities is to prevent drone riots - therefore, the AI will be forbidden from building them in
bases that cannot riot (Nerve Stapling, Punishment Spheres or Telepathic Matrix, for example)
 - Key: `ai_riot_prevention`
 - Value: None
 - Vanilla usage: Recreation Commons, Hologram Theatre, Paradise Garden, Punishment Sphere

### Defends Base
The purpose of these facilities is base defense - therefore, probe team facility sabotage should prioritize these
facilities for sabotage.
 - Key: `defends_base`
 - Value: None
 - Vanilla usage: Perimeter Defense, Children's Creche, Tachyon Field, Command Center

### Nuke Target
This facility/secret project represents a major threat. When at war, the AI should prioritize it for Planet Buster
strikes.
 - Key: `nuke_target`
 - Value: None
 - Vanilla usage: The Hunter-Seeker Algorithm, The Cloning Vats, The Cloudbase Academy

### Interlude (Biology Lab)
An interlude is played if the prerequisite tech of this facility is known at turn 75. 
It's about the introduction to native life breeding.
 - Key: `preq_tech_starts_native_life_interlude`
 - Value: None
 - Vanilla usage: Biology Lab

### Interlude (Centauri Preserve)
An interlude is played on facility construction. It's about setting aside fungus for Planet.
 - Key: `centauri_preserve_interlude`
 - Value: None
 - Vanilla usage: Centauri Preserve


## Faction Level Effects
These effects only work on Secret Projects, and affect the entire faction. 

### Project Time Warp
When using the Time Warp feature, projects with this property are randomized among the factions.
 - Key: `project_time_warp_enabled`
 - Value: None
 - Vanilla usage: The 7 early game projects + The Planetary Transit System - The Empath Guild

### Terraform Rate
Makes Formers work faster for the designated actions. 100% == Double work speed.
Multiple facilities stack multiplicatively. 
 - Key: `terraform_rate`
 - Value: integer, string, (string, ...)
   - integer 1-31: Terraform rate increase, in units of 10%. (valid effect range 10% - 310%)
   - string: Comma seperated list with possible values: FARM, SOIL_ENR, MINE, SOLAR, FOREST, ROAD, MAGTUBE, BUNKER, 
AIRBASE, SENSOR, CONDENSER, ECH_MIRROR, THERMAL_BORE, AQUIFER, REMOVE_FUNGUS, PLANT_FUNGUS, RAISE_LAND, LOWER_LAND and 
LEVEL_TERRAIN
 - Vanilla usage: The Weather Paradigm, The Xenoempathy Dome

### Terraform Unlock
Unlocks Former actions without the normally required technologies.
Some of these terraforms are unlocked by default - should those be modified in `alphax.txt` to have prerequisite 
technologies, this property will work for them.
Multiple facilities combine their unlocks.
 - Key: `terraform_unlock`
 - Value: string, ...
   - string: Comma seperated list with possible values: FARM, SOIL_ENR, MINE, SOLAR, FOREST, ROAD, MAGTUBE, BUNKER, 
AIRBASE, SENSOR, CONDENSER, ECH_MIRROR, THERMAL_BORE, AQUIFER, REMOVE_FUNGUS, PLANT_FUNGUS, RAISE_LAND, LOWER_LAND and 
LEVEL_TERRAIN
 - Vanilla usage: The Weather Paradigm

### Council Votes Increase
Makes the faction's council vote increase in weight. Only applies to votes based on population, those being Planetary
Govenor and Supreme Leader. 
Multiple facilities stack multiplicatively.
 - Key: `council_votes_increase`
 - Value: integer, integer
   - integer 0-255: Multiplier to council vote increase.
   - integer 1-255: Divisor to council vote increase.
 - Vanilla usage: The Empath Guild, Clinical Immortality

### Count as Infiltration
This project counts as an active infiltration on all other factions.
Multiple facilities are redundant.
 - Key: `counts_as_infiltration`
 - Value: None
 - Vanilla usage: The Empath Guild

### Repair as Native while in Fungus
Units owned by this faction repair faster and uncapped while in fungus, just like native units.
Multiple facilities are redundant.
 - Key: `fungus_repair_as_native`
 - Value: None
 - Vanilla usage: The Xenoempathy Dome

### Move as Native while in Fungus
Units owned by this faction move through fungus like native units (1/3 movement, "as road", except when moving between
non-road fungus and non-fungus roads)
Multiple facilities are redundant.
 - Key: `fungus_move_as_native`
 - Value: None
 - Vanilla usage: The Xenoempathy Dome

### Gain Native combat bonus while in Fungus
Units owned by this faction fight in fungus like native units (get offensive boost against fungus-located defenders,
instead of them getting a defensive boost like normal faction vs faction combat)
Multiple facilities are redundant.
 - Key: `fungus_combat_as_native`
 - Value: None
 - Vanilla usage: The Pholus Mutagen

### PSI Defense boost
Units owned by this faction gain a combat bonus to defense in PSI combat, similar to and stacking with the Trance unit
ability.
Multiple facilities are additive.
 - Key: `psi_defense`
 - Value: integer
   - integer 0-255: % bonus (vanilla: 50)
 - Vanilla usage: The Neural Amplifier

### PSI Offense boost
Units owned by this faction gain a combat bonus to offense in PSI combat, similar to and stacking with the Empath unit
ability. This does NOT switch combat type from regular to PSI.
Multiple facilities are additive.
 - Key: `psi_attack`
 - Value: integer
   - integer 0-255: % bonus (vanilla: 50)
 - Vanilla usage: The Dream Twister

### Land Speed
Land units owned by this faction gain a bonus to movement points. Due to the naturally low move speed of land units, 
this should be considered a massive power boost, which is probably why no vanilla Project gives this.
Multiple facilities are additive.
 - Key: `land_speed`
 - Value: integer
   - integer 0-255: Flat bonus. Higher values will probably cause issues.
 - Vanilla usage: None

### Sea Speed
Sea units owned by this faction gain a bonus to movement points.
Multiple facilities are additive.
 - Key: `sea_speed`
 - Value: integer
   - integer 0-255: Flat bonus. 
 - Vanilla usage: The Maritime Control Center(+2)

### Air Speed
Air units owned by this faction gain a bonus to movement points. As if having an Aerospace Complex everywhere wasn't
powerful enough.
Multiple facilities are additive.
 - Key: `air_speed`
 - Value: integer
   - integer 0-255: Flat bonus
 - Vanilla usage: The Cloudbase Academy(+2)

### Tech Share
Gain any tech for free when it is owned by enough other factions.
With multiple facilities, the best is used.
 - Key: `tech_share`
 - Value: integer
   - integer 1-6: Number of other factions that need the technology for it to be granted.
 - Vanilla usage: The Planetary Datalinks

### Social Engineering (faction)
Gives a faction-level bonus to a Social Engineering rating. Will be reflected in the social engineering panel and affect
everything.
Multiple facilities are additive to extremes of -10 and 10.
 - Key: `se_economy`, `se_efficiency`, `se_support`, `se_morale`, `se_police`, `se_growth`, `se_planet`, `se_probe`, 
`se_industry`, `se_research`
 - Value: integer
   - integer: Increase to the rating.
 - Vanilla usage: The Ascetic Virtues, The Living Refinery

### Probe Immunity
Faction-level probe immunity. Set to 1 for the SMAX version, which can be bypassed with Algorithmically Enhanced probes.
(though it still increases mind control price and reduces success rates.). Set to 2 for SMAC version, which renders
everything completely probe-immune.
With multiple facilities, the best is used.
 - Key: `probe_immunity`
 - Value: integer
   - integer 1-2: 1 for SMAX, 2 for SMAC (=absolute)
 - Vanilla usage: The Hunter-Seeker Algorithm

### Repair Speed
Increases the repair-speed of all faction-owned units, regardless of location.
Does not bypass Battle Ogre repair limits.
Multiple facilities are additive.
 - Key: `repair_speed`
 - Value: integer
   - integer 0-10: HP repair per turn. Units have 10 HP with fission reactors.
 - Vanilla usage: The Nano Factory

### Lift Field Repair Cap
Normally, field repairs (healing outside of bases) is capped at healing to 80%, (just barely green, 70% puts a unit
in the yellow). This property removes that cap.
Multiple facilities are redundant.
 - Key: `lift_repair_limit`
 - Value: None
 - Vanilla usage: The Nano Factory

### Unit Upgrade Cost
Modifies the cost to upgrade units from one prototype to another (compatible) one.
Multiple facilities are multiplicative.
 - Key: `unit_upgrade_cost_mult`
 - Value: integer, integer
   - integer 0-255: Multiplier
   - integer 1-255: Divisor
 - Vanilla usage: The Nano Factory

### Impunity
Impunity removes the downsides of a specified Social Engineering model. Multiple may be specified in a comma-seperated
list.
This property ignores any renaming of the models in `alphax.txt`. Use the original names instead.
Multiple facilities combine their Impunity lists. Multiple facilities naming the same model are redundant.
 - Key: `impunity`
 - Value: string(, ...)
   - string: All-caps model name (examples: THOUGHT CONTROL, POWER, CYBERNETIC)
 - Vanilla usage: The Network Backbone (Cybernetic), The Cloning Vats (Power, Thought Control)

### Population Floor
When building new bases, population will be increased to the provided value. On project completion, all existing bases
will have their population increased to the provided value.
With multiple facilities, the highest-population one takes effect.
 - Key: `pop_floor`
 - Value: integer
   - integer 1-127: Population floor to enforce.
 - Vanilla usage: The Planetary Transit System(3)

### Drones At Population Floor
Like [drones](#drones), but only affects bases at or below the Population Floor. Without this effect, on higher 
difficulties, new bases would start in a state of Drone Riot. 
Multiple facilities stack additively.
 - Key: `drones_under_pop_floor`
 - Value: integer
   - integer -127 to 127: Amount of drones
 - Vanilla usage: The Planetary Transit System(-1)

### Orbital Insertions
Unlocks Orbital Insertions, removing the range limit for the Drop Pods unit ability. This effect is redundant with the
Graviton Theory technology.
Multiple facilities are redundant.
 - Key: `orbital_insertion`
 - Value: None
 - Vanilla usage: The Space Elevator

### Transcendence Victory
Completion of this project grants the completing player the Transcendence Victory, if enabled.
 - Key: `transcendence_victory`
 - Value: None
 - Vanilla usage: The Ascent to Transcendence

### Mind Control Cost Reduction (probe user)
Reduces the mind control cost when attempting to mind control things from a different faction.
Multiple facilities stack multiplicatively.
 - Key: `mind_control_mult`
 - Value: integer, integer
   - integer 0-255: Multiplier
   - integer 1-255: Divisor
 - Vanilla usage: The Nethack Terminus(3/4)

### Probes have Algorithmic Enhancement
All probes owned by this faction are considered to have Algorithmic Enhancement.
Multiple facilities are redundant.
 - Key: `all_probes_have_algo_enhance`
 - Value: None
 - Vanilla usage: The Nethack Terminus

### Active Probe Morale Bonus
All probes owned by this faction have increased morale.
Multiple facilities are additive.
 - Key: `active_probes_morale`
 - Value: integer
   - integer 1-6: Morale steps
 - Vanilla usage: The Telepathic Matrix

### Longevity Vaccine Effects
When using the following Economic Social Engineering Models, gives the following benefits:
 - Free Market: 50% credits income for only this base, as per [Multiplicative Credits](#multiplicative-credits).
 - Green or Simple: -1 Drone in all bases, as per [`global_drones`](#drones).
 - Planned: -2 Drones in all bases, as per [`global_drones`](#drones).
Multiple facilities are redundant, but the effects stack with similar effects of other sources.
 - Key: `longevity_vaccine_effects`
 - Value: None
 - Vanilla usage: The Longevity Vaccine

### Voice of Planet
Does... things. Rather highly specific and irregular things that are hardcoded and not interesting.
Only 1 secret project may have this property. (just like only 1 facility may have the `is_hq` property)
 - Key: `voice_of_planet_effects`
 - Value: None
 - Vanilla usage: The Voice of Planet




