//
// Created by neotron on 2018-01-07.
//

#pragma once
#define MAKEID(NAME) (QString(NAME).toLower().replace(" ", ""))
#define Encoded(NAME, RARITY, ABR, ID) { ID, Material(ID, NAME, ABR, Material::RARITY, Material::Encoded) }
#define Manufactured(NAME, RARITY, ABR) { MAKEID(NAME), Material(MAKEID(NAME), NAME, ABR, Material::RARITY, Material::Manufactured)}
#define ManufacturedA(NAME, RARITY, ABR, ID) { ID, Material(ID, NAME, ABR, Material::RARITY, Material::Manufactured)}
#define Raw(NAME, RARITY, ABR) { MAKEID(NAME), Material(MAKEID(NAME), NAME, ABR, Material::RARITY, Material::Raw)}

QMap<QString, Material> Materials::_materialTable = QMap<QString, Material>(
        {
                // very common data
                Encoded("Anomalous Bulk Scan Data", VeryCommon, "ABSD", "bulkscandata"),
                Encoded("Atypical Disrupted Wake Echoes", VeryCommon, "ADWE", "disruptedwakeechoes"),
                Encoded("Distorted Shield Cycle Recordings", VeryCommon, "DSCR", "shieldcyclerecordings"),
                Encoded("Exceptional Scrambled Emission Data", VeryCommon, "ESED", "scrambledemissiondata"),
                Encoded("Specialised Legacy Firmware", VeryCommon, "SLF", "legacyfirmware"),
                Encoded("Unusual Encrypted Files", VeryCommon, "UEF", "encryptedfiles"),
                // common data
                Encoded("Anomalous FSD Telemetry", Common, "AFT", "fsdtelemetry"),
                Encoded("Inconsistent Shield Soak Analysis", Common, "ISSA", "shieldsoakanalysis"),
                Encoded("Irregular Emission Data", Common, "IED", "archivedemissiondata"),
                Encoded("Modified Consumer Firmware", Common, "MCF", "consumerfirmware"),
                Encoded("Tagged Encryption Codes", Common, "TEC", "encryptioncodes"),
                Encoded("Unidentified Scan Archives", Common, "USA", "scanarchives"),
                Encoded("Pattern Beta Obelisk Data", Common, "PBOD", "ancientculturaldata"),
                Encoded("Pattern Gamma Obelisk Data", Common, "PGOD", "ancienthistoricaldata"),
                // standard data
                Encoded("Classified Scan Databanks", Standard, "CSD", "scandatabanks"),
                Encoded("Cracked Industrial Firmware", Standard, "CIF", "industrialfirmware"),
                Encoded("Open Symmetric Keys", Standard, "OSK", "symmetrickeys"),
                Encoded("Strange Wake Solutions", Standard, "SWS", "wakesolutions"),
                Encoded("Unexpected Emission Data", Standard, "UED", "emissiondata"),
                Encoded("Untypical Shield Scans", Standard, "USS", "shielddensityreports"),
                Encoded("Peculiar Shield Frequency Data", Standard, "SFD", "shieldfrequencydata"),
                Encoded("Classified Scan Fragment", Standard, "CFSD", "classifiedscandata"),
                Encoded("Abnormal Compact Emissions Data", Standard, "CED", "compactemissionsdata"),
                Encoded("Modified Embedded Firmware", Standard, "EFW", "embeddedfirmware"),
                Encoded("Pattern Alpha Obelisk Data", Standard, "PAOD", "ancientbiologicaldata"),
                // rare data
                Encoded("Aberrant Shield Pattern Analysis", Rare, "ASPA", "shieldpatternanalysis"),
                Encoded("Atypical Encryption Archives", Rare, "AEA", "encryptionarchives"),
                Encoded("Decoded Emission Data", Rare, "DED", "decodedemissiondata"),
                Encoded("Divergent Scan Data", Rare, "DSD", "encodedscandata"),
                Encoded("Eccentric Hyperspace Trajectories", Rare, "EHT", "hyperspacetrajectories"),
                Encoded("Security Firmware Patch", Rare, "SFP", "securityfirmware"),
                Encoded("Pattern Delta Obelisk Data", Rare, "PDOD", "ancientlanguagedata"),
                // very rare data
                Encoded("Adaptive Encryptors Capture", VeryRare, "AEC", "adaptiveencryptors"),
                Encoded("Datamined Wake Exceptions", VeryRare, "DWEx", "dataminedwake"),
                Encoded("Peculiar Shield Frequency Data", VeryRare, "PSFD", "shieldfrequencydata"),
                Encoded("Pattern Epsilon Obelisk Data", VeryRare, "PSFD", "ancienttechnologicaldata"),
                // very common manufactured
                Manufactured("Basic Conductors", VeryCommon, "BaC"),
                Manufactured("Chemical Storage Units", VeryCommon, "CSU"),
                Manufactured("Compact Composites", VeryCommon, "CC"),
                Manufactured("Crystal Shards", VeryCommon, "CS"),
                Manufactured("Grid Resistors", VeryCommon, "GR"),
                Manufactured("Heat Conduction Wiring", VeryCommon, "HCW"),
                Manufactured("Mechanical Scrap", VeryCommon, "MS"),
                Manufactured("Salvaged Alloys", VeryCommon, "SAll"),
                Manufactured("Worn Shield Emitters", VeryCommon, "WSE"),
                Manufactured("Thermic Alloys", VeryCommon, "ThA"),
                // common manufactured
                Manufactured("Chemical Processors", Common, "CP"),
                Manufactured("Conductive Components", Common, "CCo"),
                Manufactured("Filament Composites", Common, "FiC"),
                ManufacturedA("Flawed Focus Crystals", Common, "FFC", "uncutfocuscrystals"),
                Manufactured("Galvanising Alloys", Common, "GA"),
                Manufactured("Heat Dispersion Plate", Common, "HDP"),
                Manufactured("Heat Resistant Ceramics", Common, "HRC"),
                Manufactured("Hybrid Capacitors", Common, "HC"),
                Manufactured("Mechanical Equipment", Common, "ME"),
                Manufactured("Shield Emitters", Common, "SE"),
                // standard manufactured
                Manufactured("Chemical Distillery", Standard, "CD"),
                Manufactured("Conductive Ceramics", Standard, "CCe"),
                Manufactured("Electrochemical Arrays", Standard, "EA"),
                Manufactured("Focus Crystals", Standard, "FoC"),
                Manufactured("Heat Exchangers", Standard, "HE"),
                Manufactured("High Density Composites", Standard, "HDC"),
                Manufactured("Mechanical Components", Standard, "MC"),
                Manufactured("Phase Alloys", Standard, "PA"),
                Manufactured("Precipitated Alloys", Standard, "PAll"),
                Manufactured("Shielding Sensors", Standard, "SS"),
                // rare manufactured
                Manufactured("Chemical Manipulators", Rare, "CM"),
                Manufactured("Compound Shielding", Rare, "CoS"),
                Manufactured("Conductive Polymers", Rare, "CPo"),
                Manufactured("Configurable Components", Rare, "CCom"),
                Manufactured("Heat Vanes", Rare, "HV"),
                Manufactured("Polymer Capacitors", Rare, "PCa"),
                Manufactured("Proto Light Alloys", Rare, "PLA"),
                Manufactured("Refined Focus Crystals", Rare, "RFC"),
                Manufactured("Tempered Alloys", Rare, "TeA"),
                ManufacturedA("Proprietary Composites", Rare, "FPC", "fedproprietarycomposites"),
                // very rare manufactured
                ManufacturedA("Core Dynamics Composites", VeryRare, "FCC", "fedcorecomposites"),
                Manufactured("Biotech Conductors", VeryRare, "BiC"),
                Manufactured("Exquisite Focus Crystals", VeryRare, "EFC"),
                Manufactured("Imperial Shielding", VeryRare, "IS"),
                Manufactured("Improvised Components", VeryRare, "IC"),
                Manufactured("Military Grade Alloys", VeryRare, "MGA"),
                Manufactured("Military Supercapacitors", VeryRare, "MSC"),
                Manufactured("Pharmaceutical Isolators", VeryRare, "PI"),
                Manufactured("Proto Heat Radiators", VeryRare, "PHR"),
                Manufactured("Proto Radiolic Alloys", VeryRare, "PRA"),

                // THARGOID MATERIALS
                ManufacturedA("Thargoid Carapace", Common, "UKCP", "unknowncarapace"),
                ManufacturedA("Thargoid Energy Cell", Standard, "UKEC", "unknownenergycell"),
                ManufacturedA("Thargoid Organic Circuitry", VeryRare, "UKOC", "unknownorganiccircuitry"),
                ManufacturedA("Thargoid Technology Components", Rare, "UKTC", "unknowntechnologycomponents"),
                ManufacturedA("Sensor Fragment", VeryRare, "UES", "unknownenergysource"),

                // THARGOID DATA
                Encoded("Thargoid Material Composition Data", Standard, "UMCD", "tg_compositiondata"),
                Encoded("Thargoid Structural Data", Common, "UKSD", "tg_structuraldata"),
                Encoded("Thargoid Residue Data", Rare, "URDA", "tg_residuedata"),
                Encoded("Thargoid Ship Signature", Standard, "USSig", "unknownshipsignature"),
                Encoded("Thargoid Wake Data", Rare, "UWD", "unknownwakedata"),

                // RAW VERY COMMON
                Raw("Carbon", VeryCommon, "C"),
                Raw("Iron", VeryCommon, "Fe"),
                Raw("Nickel", VeryCommon, "Ni"),
                Raw("Phosphorus", VeryCommon, "P"),
                Raw("Sulphur", VeryCommon, "S"),

                // RAW COMMON
                Raw("Chromium", Common, "Cr"),
                Raw("Germanium", Common, "Ge"),
                Raw("Manganese", Common, "Mn"),
                Raw("Vanadium", Common, "V"),
                Raw("Zinc", Common, "Zn"),

                // RAW STANDARD
                Raw("Niobium", Standard, "Nb"),
                Raw("Tungsten", Standard, "W"),
                Raw("Arsenic", Standard, "As"),
                Raw("Selenium", Standard, "Se"),
                Raw("Zirconium", Standard, "Zr"),

                // RAW RARE
                Raw("Yttrium", Rare, "Y"),
                Raw("Cadmium", Rare, "Cd"),
                Raw("Mercury", Rare, "Hg"),
                Raw("Molybdenum", Rare, "Mo"),
                Raw("Tin", Rare, "Sn"),

                // RAW VERY RARE
                Raw("Antimony", VeryRare, "Sb"),
                Raw("Polonium", VeryRare, "Po"),
                Raw("Ruthenium", VeryRare, "Ru"),
                Raw("Technetium", VeryRare, "Tc"),
                Raw("Tellurium", VeryRare, "Te")

        });
