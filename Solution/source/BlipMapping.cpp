#include "BlipMapping.h"
#include "../../Scripting/GTAentity.h"
#include "../../Natives/natives.h"
#include "../../Scripting/GTAblip.h"
#include "Natives/natives2.h"
#include "Scripting/Model.h"

namespace sub::Spooner
{
    BlipMapping GetBlipMappingForEntity(GTAentity& ent)
    {
        if ((EntityType)ent.Type() == EntityType::VEHICLE)
        {
            int hash = ent.Model().hash;

            // Exact model matches — Face-up
            if (hash == GET_HASH_KEY("stockade"))       return { BlipIcon::ArmoredTruck,            false };
            if (hash == GET_HASH_KEY("stockade3"))       return { BlipIcon::ArmoredTruck,            false };
            if (hash == GET_HASH_KEY("riot"))       return { BlipIcon::ArmoredTruck,            false };
            if (hash == GET_HASH_KEY("towtruck"))       return { BlipIcon::TowTruck,                false };
            if (hash == GET_HASH_KEY("towtruck2"))      return { BlipIcon::TowTruck,                false };
            if (hash == GET_HASH_KEY("towtruck3"))      return { BlipIcon::TowTruck,                false };
            if (hash == GET_HASH_KEY("towtruck4"))      return { BlipIcon::TowTruck,                false };
            if (hash == GET_HASH_KEY("tourbus"))        return { BlipIcon::VinewoodTours,           false };
            if (hash == GET_HASH_KEY("dune"))           return { BlipIcon::ArmsTraffickingGround,   false };
            if (hash == GET_HASH_KEY("taxi"))           return { BlipIcon::Cab,                     false };
            if (hash == GET_HASH_KEY("garbage"))        return { BlipIcon::GarbageTruck,            false };
            if (hash == GET_HASH_KEY("garbage2"))       return { BlipIcon::GarbageTruck,            false };
            if (hash == GET_HASH_KEY("dinghy"))         return { BlipIcon::Dinghy,                  false };
            if (hash == GET_HASH_KEY("dinghy3"))        return { BlipIcon::Dinghy,                  false };
            if (hash == GET_HASH_KEY("dinghy4"))        return { BlipIcon::Dinghy,                  false };
            if (hash == GET_HASH_KEY("dinghy5"))        return { BlipIcon::Dinghy2,                 false };
            if (hash == GET_HASH_KEY("marquis"))        return { BlipIcon::Boat,                    false };
            if (hash == GET_HASH_KEY("seashark"))       return { BlipIcon::Seashark,                false };
            if (hash == GET_HASH_KEY("seashark2"))      return { BlipIcon::Seashark,                false };
            if (hash == GET_HASH_KEY("seashark3"))      return { BlipIcon::Seashark,                false };
            if (hash == GET_HASH_KEY("phantom2"))       return { BlipIcon::PhantomWedge,            false };
            if (hash == GET_HASH_KEY("boxville4"))      return { BlipIcon::ArmoredBoxville,         false };
            if (hash == GET_HASH_KEY("ruiner2"))        return { BlipIcon::Ruiner2000,              false };
            if (hash == GET_HASH_KEY("wastelander"))    return { BlipIcon::Wastelander,             false };
            if (hash == GET_HASH_KEY("voltic2"))        return { BlipIcon::RocketVoltic,            false };
            if (hash == GET_HASH_KEY("technical3"))     return { BlipIcon::TechnicalAqua,           false };
            if (hash == GET_HASH_KEY("oppressor"))      return { BlipIcon::Oppressor,               false };
            if (hash == GET_HASH_KEY("halftrack"))      return { BlipIcon::HalfTrack,               false };
            if (hash == GET_HASH_KEY("dune3"))          return { BlipIcon::DuneFAV,                 false };
            if (hash == GET_HASH_KEY("trailersmall2"))  return { BlipIcon::WeaponizedTrailer,       false };
            if (hash == GET_HASH_KEY("trailerlarge"))        return { BlipIcon::MobileOperationsCenter,  false };
            if (hash == GET_HASH_KEY("havok"))          return { BlipIcon::Havok,                   false };
            if (hash == GET_HASH_KEY("hunter"))         return { BlipIcon::Hunter,                  false };
            if (hash == GET_HASH_KEY("avenger"))        return { BlipIcon::Avenger,                 false };
            if (hash == GET_HASH_KEY("avenger2"))       return { BlipIcon::Avenger,                 false };
            if (hash == GET_HASH_KEY("stromberg"))      return { BlipIcon::Stromberg,               false };
            if (hash == GET_HASH_KEY("deluxo"))         return { BlipIcon::Deluxo,                  false };
            if (hash == GET_HASH_KEY("thruster"))       return { BlipIcon::Thruster,                false };
            if (hash == GET_HASH_KEY("riot2"))          return { BlipIcon::RCV,                     false };
            if (hash == GET_HASH_KEY("akula"))          return { BlipIcon::Akula,                   false };
            if (hash == GET_HASH_KEY("chernobog"))      return { BlipIcon::Chernobog,               false };
            if (hash == GET_HASH_KEY("seasparrow"))     return { BlipIcon::SeaSparrow,              false };
            if (hash == GET_HASH_KEY("seasparrow2"))    return { BlipIcon::SeaSparrow2,             false };
            if (hash == GET_HASH_KEY("seasparrow3"))    return { BlipIcon::SeaSparrow2,             false };
            if (hash == GET_HASH_KEY("pbus2"))          return { BlipIcon::FestivalBus,             false };
            if (hash == GET_HASH_KEY("pbus"))           return { BlipIcon::Bus,                     false };
            if (hash == GET_HASH_KEY("terbyte"))     return { BlipIcon::Terrorbyte,              false };
            if (hash == GET_HASH_KEY("pounder2"))       return { BlipIcon::PounderCustom,           false };
            if (hash == GET_HASH_KEY("mule4"))          return { BlipIcon::MuleCustom,              false };
            if (hash == GET_HASH_KEY("speedo4"))        return { BlipIcon::SpeedoCustom,            false };
            if (hash == GET_HASH_KEY("blimp"))          return { BlipIcon::Blimp2,                  false };
            if (hash == GET_HASH_KEY("blimp2"))         return { BlipIcon::Blimp2,                  false };
            if (hash == GET_HASH_KEY("blimp3"))         return { BlipIcon::Blimp2,                  false };
            if (hash == GET_HASH_KEY("oppressor2"))     return { BlipIcon::OppressorMkII,           false };
            if (hash == GET_HASH_KEY("bruiser"))        return { BlipIcon::Bruiser,                 false };
            if (hash == GET_HASH_KEY("bruiser2"))       return { BlipIcon::Bruiser,                 false };
            if (hash == GET_HASH_KEY("bruiser3"))       return { BlipIcon::Bruiser,                 false };
            if (hash == GET_HASH_KEY("brutus"))         return { BlipIcon::Brutus,                  false };
            if (hash == GET_HASH_KEY("brutus2"))        return { BlipIcon::Brutus,                  false };
            if (hash == GET_HASH_KEY("brutus3"))        return { BlipIcon::Brutus,                  false };
            if (hash == GET_HASH_KEY("cerberus"))       return { BlipIcon::Cerberus,                false };
            if (hash == GET_HASH_KEY("cerberus2"))      return { BlipIcon::Cerberus,                false };
            if (hash == GET_HASH_KEY("cerberus3"))      return { BlipIcon::Cerberus,                false };
            if (hash == GET_HASH_KEY("deathbike"))      return { BlipIcon::Deathbike,               false };
            if (hash == GET_HASH_KEY("deathbike2"))     return { BlipIcon::Deathbike,               false };
            if (hash == GET_HASH_KEY("deathbike3"))     return { BlipIcon::Deathbike,               false };
            if (hash == GET_HASH_KEY("dominator4"))     return { BlipIcon::Dominator,               false };
            if (hash == GET_HASH_KEY("dominator5"))     return { BlipIcon::Dominator,               false };
            if (hash == GET_HASH_KEY("dominator6"))     return { BlipIcon::Dominator,               false };
            if (hash == GET_HASH_KEY("impaler2"))       return { BlipIcon::Impaler,                 false };
            if (hash == GET_HASH_KEY("impaler3"))       return { BlipIcon::Impaler,                 false };
            if (hash == GET_HASH_KEY("impaler4"))       return { BlipIcon::Impaler,                 false };
            if (hash == GET_HASH_KEY("imperator"))      return { BlipIcon::Imperator,               false };
            if (hash == GET_HASH_KEY("imperator2"))     return { BlipIcon::Imperator,               false };
            if (hash == GET_HASH_KEY("imperator3"))     return { BlipIcon::Imperator,               false };
            if (hash == GET_HASH_KEY("issi4"))          return { BlipIcon::Issi,                    false };
            if (hash == GET_HASH_KEY("issi5"))          return { BlipIcon::Issi,                    false };
            if (hash == GET_HASH_KEY("issi6"))          return { BlipIcon::Issi,                    false };
            if (hash == GET_HASH_KEY("sasquatch"))      return { BlipIcon::Sasquatch,               false };
            if (hash == GET_HASH_KEY("sasquatch2"))     return { BlipIcon::Sasquatch,               false };
            if (hash == GET_HASH_KEY("sasquatch3"))     return { BlipIcon::Sasquatch,               false };
            if (hash == GET_HASH_KEY("sasquatch4"))     return { BlipIcon::Sasquatch,               false };
            if (hash == GET_HASH_KEY("slamvan4"))       return { BlipIcon::Slamvam,                 false };
            if (hash == GET_HASH_KEY("slamvan5"))       return { BlipIcon::Slamvam,                 false };
            if (hash == GET_HASH_KEY("slamvan6"))       return { BlipIcon::Slamvam,                 false };
            if (hash == GET_HASH_KEY("zr380"))          return { BlipIcon::ZR380,                   false };
            if (hash == GET_HASH_KEY("zr3802"))         return { BlipIcon::ZR380,                   false };
            if (hash == GET_HASH_KEY("zr3803"))         return { BlipIcon::ZR380,                   false };
            if (hash == GET_HASH_KEY("formula"))        return { BlipIcon::OpenWheelCar,            false };
            if (hash == GET_HASH_KEY("formula2"))       return { BlipIcon::OpenWheelCar,            false };
            if (hash == GET_HASH_KEY("everon"))         return { BlipIcon::SnowTruck,               false };
            if (hash == GET_HASH_KEY("outlaw"))         return { BlipIcon::Buggy1,                  false };
            if (hash == GET_HASH_KEY("vagrant"))        return { BlipIcon::Buggy2,                  false };
            if (hash == GET_HASH_KEY("zhaba"))          return { BlipIcon::Zhaba,                   false };
            if (hash == GET_HASH_KEY("winky"))          return { BlipIcon::Winky,                   false };
            if (hash == GET_HASH_KEY("avisa"))          return { BlipIcon::MiniSub,                 false };
            if (hash == GET_HASH_KEY("verus"))          return { BlipIcon::MilitaryQuad,            false };
            if (hash == GET_HASH_KEY("vetir"))          return { BlipIcon::MilitaryTruck,           false };
            if (hash == GET_HASH_KEY("patrolboat"))     return { BlipIcon::PatrolBoat,              false };
            if (hash == GET_HASH_KEY("toreador"))       return { BlipIcon::Toreador,                false };
            if (hash == GET_HASH_KEY("squadee"))        return { BlipIcon::Squadee,                 false };
            if (hash == GET_HASH_KEY("annihilator2"))   return { BlipIcon::AnnihilatorStealth,      false };
            if (hash == GET_HASH_KEY("slamvan"))        return { BlipIcon::Slamvan2,                false };
            if (hash == GET_HASH_KEY("slamvan2"))       return { BlipIcon::Slamvan2,                false };
            if (hash == GET_HASH_KEY("slamtruck"))      return { BlipIcon::Slamvan2,                false };
            if (hash == GET_HASH_KEY("crusader"))       return { BlipIcon::Crusader,                false };
            if (hash == GET_HASH_KEY("patriot3"))       return { BlipIcon::Patriot3,                false };
            if (hash == GET_HASH_KEY("jubilee"))        return { BlipIcon::Jubilee,                 false };
            if (hash == GET_HASH_KEY("granger2"))       return { BlipIcon::Granger2,                false };
            if (hash == GET_HASH_KEY("deity"))          return { BlipIcon::Deity,                   false };
            if (hash == GET_HASH_KEY("champion"))       return { BlipIcon::Champion,                false };
            if (hash == GET_HASH_KEY("buffalo4"))       return { BlipIcon::Buffalo4,                false };
            if (hash == GET_HASH_KEY("brickade2"))      return { BlipIcon::AcidLab,                 false };
            if (hash == GET_HASH_KEY("tractor"))        return { BlipIcon::Tractor,                 false };
            if (hash == GET_HASH_KEY("tractor2"))       return { BlipIcon::Tractor,                 false };
            if (hash == GET_HASH_KEY("tractor3"))       return { BlipIcon::Tractor,                 false };
            if (hash == GET_HASH_KEY("oiltanker"))      return { BlipIcon::OilTanker,               false };
            if (hash == GET_HASH_KEY("burrito"))       return { BlipIcon::Burrito,                 false };
            if (hash == GET_HASH_KEY("burrito2"))       return { BlipIcon::Burrito,                 false };
            if (hash == GET_HASH_KEY("burrito3"))       return { BlipIcon::Burrito,                 false };
            if (hash == GET_HASH_KEY("burrito4"))       return { BlipIcon::Burrito,                 false };
            if (hash == GET_HASH_KEY("burrito5"))       return { BlipIcon::Burrito,                 false };
            if (hash == GET_HASH_KEY("gburrito"))       return { BlipIcon::Burrito,                 false };
            if (hash == GET_HASH_KEY("gburrito2"))       return { BlipIcon::Burrito,                 false };
            if (hash == GET_HASH_KEY("policet"))       return { BlipIcon::Burrito,                 false };
            if (hash == GET_HASH_KEY("policet3"))       return { BlipIcon::Burrito,                 false };
            if (hash == GET_HASH_KEY("conada2"))        return { BlipIcon::WeaponizedConada,        false };
            if (hash == GET_HASH_KEY("cargobob5"))      return { BlipIcon::DH7IronMule,             false };
            if (hash == GET_HASH_KEY("firetruk"))      return { BlipIcon::FireTruck,               false };
            if (hash == GET_HASH_KEY("forklift"))       return { BlipIcon::Forklift,                false };
            if (hash == GET_HASH_KEY("vivianite2"))     return { BlipIcon::TaxiSelfDestruct,        false };
            if (hash == GET_HASH_KEY("cargobob"))       return { BlipIcon::Cargobob,                false };
            if (hash == GET_HASH_KEY("cargobob2"))      return { BlipIcon::Cargobob,                false };
            if (hash == GET_HASH_KEY("cargobob3"))      return { BlipIcon::Cargobob,                false };
            if (hash == GET_HASH_KEY("cargobob4"))      return { BlipIcon::Cargobob,                false };
            if (hash == GET_HASH_KEY("blazer"))         return { BlipIcon::QuadBike,                    false };
            if (hash == GET_HASH_KEY("blazer2"))        return { BlipIcon::QuadBike,                    false };
            if (hash == GET_HASH_KEY("blazer3"))        return { BlipIcon::QuadBike,                    false };
            if (hash == GET_HASH_KEY("blazer4"))        return { BlipIcon::QuadBike,                    false };
            if (hash == GET_HASH_KEY("blazer5"))        return { BlipIcon::QuadBike,                    false };
            if (hash == GET_HASH_KEY("stryder"))        return { BlipIcon::QuadBike,                    false };
            if (hash == GET_HASH_KEY("stunt"))    return { BlipIcon::Stunt,             false };
            if (hash == GET_HASH_KEY("metrotrain"))    return { BlipIcon::SubwayTrain,             false };
            if (hash == GET_HASH_KEY("cablecar"))    return { BlipIcon::Lift,             false };
            if (hash == GET_HASH_KEY("armytanker"))      return { BlipIcon::OilTanker,               false };
            if (hash == GET_HASH_KEY("armytrailer2"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("baletrailer"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("boattrailer"))      return { BlipIcon::Speedboat,               false };
            if (hash == GET_HASH_KEY("boattrailer2"))      return { BlipIcon::Speedboat,               false };
            if (hash == GET_HASH_KEY("boattrailer3"))      return { BlipIcon::Seashark,               false };
            if (hash == GET_HASH_KEY("docktrailer"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("freighttrailer"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("tanker2"))      return { BlipIcon::OilTanker,               false };
            if (hash == GET_HASH_KEY("trailers"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("trailers5"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("trflat2"))      return { BlipIcon::AnnihilatorStealth,               false };
            if (hash == GET_HASH_KEY("trailerlogs"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("tvtrailer"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("trailers2"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("trailers3"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("tr4"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("tanker"))      return { BlipIcon::OilTanker,               false };
            if (hash == GET_HASH_KEY("trflat"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("trailers4"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("tvtrailer2"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("tr2"))      return { BlipIcon::Trailer,               false };
            if (hash == GET_HASH_KEY("polcaracara"))      return { BlipIcon::PoliceCar,               false };



            // Exact model matches — Rotating
            if (hash == GET_HASH_KEY("cuban800"))       return { BlipIcon::ArmsTraffickingAir,      true };
            if (hash == GET_HASH_KEY("insurgent"))      return { BlipIcon::GunCar,                  true };
            if (hash == GET_HASH_KEY("insurgent2"))     return { BlipIcon::GunCar,                  true };
            if (hash == GET_HASH_KEY("insurgent3"))     return { BlipIcon::GunCar,                  true };
            if (hash == GET_HASH_KEY("rhino"))          return { BlipIcon::Tank,                    true };
            if (hash == GET_HASH_KEY("limo2"))          return { BlipIcon::TurretedLimo,            true };
            if (hash == GET_HASH_KEY("apc"))            return { BlipIcon::APC,                     true };
            if (hash == GET_HASH_KEY("tampa3"))         return { BlipIcon::WeaponizedTampa,         true };
            if (hash == GET_HASH_KEY("alphaz1"))        return { BlipIcon::AlphaZ1,                 true };
            if (hash == GET_HASH_KEY("bombushka"))      return { BlipIcon::Bombushka,               true };
            if (hash == GET_HASH_KEY("howard"))         return { BlipIcon::HowardNX25,              true };
            if (hash == GET_HASH_KEY("ultralight"))     return { BlipIcon::Ultralight,              true };
            if (hash == GET_HASH_KEY("mogul"))          return { BlipIcon::Mogul,                   true };
            if (hash == GET_HASH_KEY("molotok"))        return { BlipIcon::V65Molotok,              true };
            if (hash == GET_HASH_KEY("nokota"))         return { BlipIcon::P45Nokota,               true };
            if (hash == GET_HASH_KEY("pyro"))           return { BlipIcon::Pyro,                    true };
            if (hash == GET_HASH_KEY("rogue"))          return { BlipIcon::Rogue,                   true };
            if (hash == GET_HASH_KEY("starling"))       return { BlipIcon::Starling,                true };
            if (hash == GET_HASH_KEY("seabreeze"))      return { BlipIcon::Seabreeze,               true };
            if (hash == GET_HASH_KEY("tula"))           return { BlipIcon::Tula,                    true };
            if (hash == GET_HASH_KEY("khanjali"))       return { BlipIcon::Khanjali,                true };
            if (hash == GET_HASH_KEY("volatol"))        return { BlipIcon::Volatol,                 true };
            if (hash == GET_HASH_KEY("barrage"))        return { BlipIcon::Barrage,                 true };
            if (hash == GET_HASH_KEY("caracara"))       return { BlipIcon::Caracara,                true };
            if (hash == GET_HASH_KEY("caracara3"))      return { BlipIcon::Caracara,                true };
            if (hash == GET_HASH_KEY("menacer"))        return { BlipIcon::Menacer,                 true };
            if (hash == GET_HASH_KEY("scramjet"))       return { BlipIcon::Scramjet,                true };
            if (hash == GET_HASH_KEY("strikeforce"))    return { BlipIcon::B11StrikeForce,          true };
            if (hash == GET_HASH_KEY("rcbandito"))      return { BlipIcon::RCVehicle,               true };
            if (hash == GET_HASH_KEY("scarab"))         return { BlipIcon::Scarab,                  true };
            if (hash == GET_HASH_KEY("scarab2"))        return { BlipIcon::Scarab,                  true };
            if (hash == GET_HASH_KEY("scarab3"))        return { BlipIcon::Scarab,                  true };
            if (hash == GET_HASH_KEY("stretch"))        return { BlipIcon::Limo,                    true };
            if (hash == GET_HASH_KEY("minitank"))       return { BlipIcon::RCTank,                  true };
            if (hash == GET_HASH_KEY("veto"))           return { BlipIcon::KartRetro,               true };
            if (hash == GET_HASH_KEY("veto2"))          return { BlipIcon::KartModern,              true };
            if (hash == GET_HASH_KEY("alkonost"))       return { BlipIcon::Alkonost,                true };
            if (hash == GET_HASH_KEY("kosatka"))        return { BlipIcon::Kostatka,                true };
            if (hash == GET_HASH_KEY("raiju"))          return { BlipIcon::Raiju,                   true };
            if (hash == GET_HASH_KEY("duster2"))        return { BlipIcon::Duster300H,              true };
            if (hash == GET_HASH_KEY("titan2"))         return { BlipIcon::Titan250D,               true };
            if (hash == GET_HASH_KEY("lazer"))         return { BlipIcon::Jet,               true };
            if (hash == GET_HASH_KEY("hydra"))         return { BlipIcon::Jet,               true };
            if (hash == GET_HASH_KEY("besra"))         return { BlipIcon::Jet,               true };
            if (hash == GET_HASH_KEY("streamer216"))    return { BlipIcon::Streamer216,             true };
            if (hash == GET_HASH_KEY("polgauntlet"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("polgreenwood"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("polignus"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("polimpaler5"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("polimpaler6"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("polfaction2"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("police"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("police2"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("police3"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("police4"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("police5"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("policeold1"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("policeold2"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("sheriff"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("sheriff2"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("polterminus"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("polbuffalo"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("polbuffalo6"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("polcoquette4"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("poldominator10"))      return { BlipIcon::CopCar,               true };
            if (hash == GET_HASH_KEY("poldorado"))      return { BlipIcon::CopCar,               true };

            // Vehicle class fallback
            int vehClass = VEHICLE::GET_VEHICLE_CLASS(ent.Handle());
            switch (vehClass)
            {
            case 5:  return { BlipIcon::SportsCar,           false }; // Sports Classics
            case 6:  return { BlipIcon::SportsCar,           false }; // Sports
            case 7:  return { BlipIcon::SportsCar,           false }; // Super
            case 8:  return { BlipIcon::PersonalVehicleBike, false }; // Motorcycles
            case 13: return { BlipIcon::Bicycle,             false }; // Cycles
            case 14: return { BlipIcon::Speedboat,           true }; // Boats
            case 15: return { BlipIcon::HelicopterAnimated,  false }; // Helicopters
            case 16: return { BlipIcon::Plane,                 true }; // Planes
            case 20: return { BlipIcon::Truck,               false }; // Commercial
            case 21: return { BlipIcon::Train,               false }; // Trains
            case 22: return { BlipIcon::OpenWheelCar,        false }; // Open Wheel
            default: return { BlipIcon::PersonalVehicleCar,  false }; // Generic car
            }
        }
        else if ((EntityType)ent.Type() == EntityType::PED)
        {
            int hash = ent.Model().hash;

            if (hash == GET_HASH_KEY("a_c_shepherd"))   return { BlipIcon::Dog,        false };
            if (hash == GET_HASH_KEY("a_c_husky"))      return { BlipIcon::Dog,        false };
            if (hash == GET_HASH_KEY("a_c_retriever"))  return { BlipIcon::Dog,        false };
            if (hash == GET_HASH_KEY("a_c_rottweiler")) return { BlipIcon::Dog,        false };
            if (hash == GET_HASH_KEY("a_c_pug"))        return { BlipIcon::Dog,        false };
            if (hash == GET_HASH_KEY("a_c_poodle"))     return { BlipIcon::Dog,        false };
            if (hash == GET_HASH_KEY("a_c_labrador"))   return { BlipIcon::Dog,        false };
            if (hash == GET_HASH_KEY("a_c_westy"))      return { BlipIcon::Dog,        false };
            if (hash == GET_HASH_KEY("a_c_cat_01"))     return { BlipIcon::Cat,        false };
            if (hash == GET_HASH_KEY("a_c_cat_02"))     return { BlipIcon::Cat,        false };
            if (hash == GET_HASH_KEY("a_c_sharktiger")) return { BlipIcon::SharkTiger, false };

            return { BlipIcon::Enemy, false };
        }

        return { BlipIcon::Standard, false };
    }
}