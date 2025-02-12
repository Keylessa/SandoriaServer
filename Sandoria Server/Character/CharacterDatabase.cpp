#include "CharacterDatabase.h"
#include <iostream>

CharacterDatabase::CharacterDatabase() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        conn = std::unique_ptr<sql::Connection>(driver->connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS));
        conn->setSchema(MYSQL_DB_CHARACTER);
    }
    catch (sql::SQLException& e) {
        std::cerr << "[MySQL Error] " << e.what() << std::endl;
    }
}

CharacterDatabase::~CharacterDatabase() {}

bool CharacterDatabase::createCharacter(int accountID, const std::string& name, int faction, int race, int charClass, int gender){
    try {
        // Definirea variabilelor pentru statistici
        int strength = 0, agility = 0, stamina = 0, intellect = 0, spirit = 0;
        int armor = 0, resHoly = 0, resFire = 0, resNature = 0, resFrost = 0, resShadow = 0, resArcane = 0;
        float blockPct = 0, dodgePct = 0, parryPct = 0, critPct = 0, rangedCritPct = 0, spellCritPct = 0;
        int attackPower = 0, rangedAttackPower = 0, spellPower = 0, resilience = 0;

        // Obținem statisticile clasei
        getClassStats(charClass, strength, agility, stamina, intellect, spirit, armor, resHoly, resFire, resNature, resFrost, resShadow, resArcane, blockPct, dodgePct, parryPct, critPct, rangedCritPct, spellCritPct, attackPower, rangedAttackPower, spellPower, resilience);

        // Inserăm în tabelul `characters`
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO characters (account, name, faction, race, class, gender, level, xp, money, playerBytes, playerBytes2, playerFlags, position_x, position_y, position_z, map, is_online) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));

        // Convertim accountName direct în int
        pstmt->setInt(1, (accountID));  // accountID
        pstmt->setString(2, name); // name
        pstmt->setInt(3, faction); // faction
        pstmt->setInt(4, race);    // race
        pstmt->setInt(5, charClass); // class
        pstmt->setInt(6, gender);  // gender
        pstmt->setInt(7, 1);   // level default 1
        pstmt->setInt(8, 0);   // xp default 0
        pstmt->setInt(9, 0);   // money default 0
        pstmt->setInt(10, 0);  // playerBytes
        pstmt->setInt(11, 0);  // playerBytes2
        pstmt->setInt(12, 0);  // playerFlags
        pstmt->setDouble(13, 0.0); // position_x
        pstmt->setDouble(14, 0.0); // position_y
        pstmt->setDouble(15, 0.0); // position_z
        pstmt->setInt(16, 0);   // map
        pstmt->setInt(17, 0);   // is_online (default 0)

        std::cout << "Date inserate în baza de date cu succes!" << std::endl;  // Log pentru succes
        pstmt->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> guidStmt(conn->prepareStatement("SELECT LAST_INSERT_ID()"));
        std::unique_ptr<sql::ResultSet> guidRes(guidStmt->executeQuery());
        int guid = 0;
        if (guidRes->next()) {
            guid = guidRes->getInt(1);
        }
        
        // Inserăm acum în tabelul `Character_Stats` cu valorile corecte
        std::unique_ptr<sql::PreparedStatement> pstmtStats(conn->prepareStatement(
            "INSERT INTO character_stats (guid, maxhealth, maxpower1, maxpower2, maxpower3, maxpower4, maxpower5, maxpower6, maxpower7, strength, agility, stamina, intellect, spirit, armor, resHoly, resFire, resNature, resFrost, resShadow, resArcane, blockPct, dodgePct, parryPct, critPct, rangedCritPct, spellCritPct, attackPower, rangedAttackPower, spellPower, resilience) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));

        // Folosim valorile pentru statistici care depind de clasa
        pstmtStats->setInt(1, guid);  // Guid-ul va trebui să fie obținut (poate după inserare, sau după un query separat)
        pstmtStats->setInt(2, 100);  // maxhealth default 100
        pstmtStats->setInt(3, 50);   // maxpower1 (Mana)
        pstmtStats->setInt(4, 50);   // maxpower2 (Rage)
        pstmtStats->setInt(5, 50);   // maxpower3 (Focus)
        pstmtStats->setInt(6, 50);   // maxpower4 (Energy)
        pstmtStats->setInt(7, 50);   // maxpower5 (Happiness)
        pstmtStats->setInt(8, 50);   // maxpower6 (Rune)
        pstmtStats->setInt(9, 50);   // maxpower7 (Runic Power)
        pstmtStats->setInt(10, strength);
        pstmtStats->setInt(11, agility);
        pstmtStats->setInt(12, stamina);
        pstmtStats->setInt(13, intellect);
        pstmtStats->setInt(14, spirit);
        pstmtStats->setInt(15, armor);
        pstmtStats->setInt(16, resHoly);
        pstmtStats->setInt(17, resFire);
        pstmtStats->setInt(18, resNature);
        pstmtStats->setInt(19, resFrost);
        pstmtStats->setInt(20, resShadow);
        pstmtStats->setInt(21, resArcane);
        pstmtStats->setDouble(22, blockPct);
        pstmtStats->setDouble(23, dodgePct);
        pstmtStats->setDouble(24, parryPct);
        pstmtStats->setDouble(25, critPct);
        pstmtStats->setDouble(26, rangedCritPct);
        pstmtStats->setDouble(27, spellCritPct);
        pstmtStats->setInt(28, attackPower);
        pstmtStats->setInt(29, rangedAttackPower);
        pstmtStats->setInt(30, spellPower);
        pstmtStats->setInt(31, resilience);

        pstmtStats->executeUpdate();

        std::cout << "Data inserată în tabelul Character_Stats cu succes!" << std::endl;

        return true;

    }
    catch (sql::SQLException& e) {
        std::cerr << "[MySQL Error] " << e.what() << std::endl;
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////////
// Funcție pentru a obține statistici în funcție de clasa caracterului
///////////////////////////////////////////////////////////////////////////////////

void CharacterDatabase::getClassStats(int& charClass, int& strength, int& agility, int& stamina, int& intellect, int& spirit,
    int& armor, int& resHoly, int& resFire, int& resNature, int& resFrost, int& resShadow, int& resArcane,
    float& blockPct, float& dodgePct, float& parryPct, float& critPct, float& rangedCritPct, float& spellCritPct,
    int& attackPower, int& rangedAttackPower, int& spellPower, int& resilience) {
    if (charClass == 1) {
        strength = 20;
        agility = 10;
        stamina = 15;
        intellect = 5;
        spirit = 5;
        armor = 30;
        resHoly = 5;
        resFire = 5;
        resNature = 5;
        resFrost = 5;
        resShadow = 5;
        resArcane = 5;
        blockPct = 0.05f;
        dodgePct = 0.02f;
        parryPct = 0.03f;
        critPct = 0.1f;
        rangedCritPct = 0.05f;
        spellCritPct = 0.02f;
        attackPower = 50;
        rangedAttackPower = 30;
        spellPower = 10;
        resilience = 10;
    }
    else if (charClass == 2) {
        strength = 5;
        agility = 5;
        stamina = 5;
        intellect = 20;
        spirit = 15;
        armor = 10;
        resHoly = 5;
        resFire = 10;
        resNature = 10;
        resFrost = 10;
        resShadow = 10;
        resArcane = 10;
        blockPct = 0.0f;
        dodgePct = 0.0f;
        parryPct = 0.0f;
        critPct = 0.05f;
        rangedCritPct = 0.02f;
        spellCritPct = 0.2f;
        attackPower = 10;
        rangedAttackPower = 10;
        spellPower = 60;
        resilience = 5;
    }
    // Poți adăuga alte clase și statistici corespunzătoare
}

///////////////////////////////////////////////////////////////////////////////////
//Preluam anumite date din baza de date pentru a le introduce in Selectia Characterului
///////////////////////////////////////////////////////////////////////////////////

std::vector<CharacterDataList> CharacterDatabase::getCharacters(const std::string& accountName) {
    std::vector<CharacterDataList> characters;
    try {
        // Adăugăm și coloana "guid" și "account" pentru a avea mai multe informații
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT guid, account, name, level "
            "FROM characters WHERE account = ?"
        ));

        pstmt->setString(1, accountName);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            CharacterDataList cd;
            // Atribuim valorile primite din interogare
            cd.Name = res->getString("name");
            cd.Level = res->getString("level");
            //cd.Level = static_cast<uint8_t>(res->getUInt("level"));
            
            characters.push_back(cd);
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "[MySQL Error] " << e.what() << std::endl;
    }
    return characters;
}

bool CharacterDatabase::doesCharacterExist(const std::string& charName)
{
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM characters WHERE name = ?"));
        pstmt->setString(1, charName);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        res->next();
        return res->getInt(1) > 0;  // Returnează true dacă există cel puțin un rezultat
    }
    catch (sql::SQLException& e) {
        std::cerr << "[MySQL Error] " << e.what() << std::endl;
        return true;  // Dacă apare o eroare, presupunem că numele nu e disponibil pentru siguranță
    }
}

///////////////////////////////////////////////////////////////////////////////////
//Preluam date din baza de date pentru a crea characteru si al introduce in World
///////////////////////////////////////////////////////////////////////////////////

CharacterData CharacterDatabase::LoadCharacter(uint32_t CharacterID) {
    CharacterData charData;

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT c.guid, c.account, c.name, c.faction, c.race, c.class, c.gender, "
            "c.level, c.xp, c.money, c.position_x, c.position_y, c.position_z, "
            "c.map, c.is_online, c.playerBytes, "
            "s.maxhealth, s.maxpower1, s.maxpower2, s.maxpower3, s.maxpower4, "
            "s.maxpower5, s.maxpower6, s.maxpower7, s.strength, s.agility, s.stamina, "
            "s.intellect, s.spirit, s.armor, s.resHoly, s.resFire, s.resNature, "
            "s.resFrost, s.resShadow, s.resArcane, s.blockPct, s.dodgePct, s.parryPct, "
            "s.critPct, s.rangedCritPct, s.spellCritPct, s.attackPower, s.rangedAttackPower, "
            "s.spellPower, s.resilience "
            "FROM characters c "
            "LEFT JOIN character_stats s ON c.guid = s.guid "
            "WHERE c.guid = ?"
        ));
        pstmt->setUInt(1, CharacterID);

        std::unique_ptr<sql::ResultSet> result(pstmt->executeQuery());

        if (result->next()) {
            charData.CharacterID = result->getUInt("guid");
            charData.OwnerID = result->getUInt("account");
            charData.Name = result->getString("name");
            charData.Faction = result->getUInt("faction");
            charData.Race = result->getUInt("race");
            charData.Class = result->getUInt("class");
            charData.Gender = result->getUInt("gender");
            charData.Level = result->getUInt("level");
            charData.XP = result->getUInt("xp");
            charData.Money = result->getUInt("money");
            charData.SpawnLocation = FVector(result->getDouble("position_x"), result->getDouble("position_y"), result->getDouble("position_z"));
            charData.MapID = result->getUInt("map");
            charData.IsOnline = result->getUInt("is_online") == 1;

            // Statistici
            charData.MaxHealth = result->getUInt("maxhealth");
            charData.MaxPower[0] = result->getUInt("maxpower1");
            charData.MaxPower[1] = result->getUInt("maxpower2");
            charData.MaxPower[2] = result->getUInt("maxpower3");
            charData.MaxPower[3] = result->getUInt("maxpower4");
            charData.MaxPower[4] = result->getUInt("maxpower5");
            charData.MaxPower[5] = result->getUInt("maxpower6");
            charData.MaxPower[6] = result->getUInt("maxpower7");
            charData.Strength = result->getUInt("strength");
            charData.Agility = result->getUInt("agility");
            charData.Stamina = result->getUInt("stamina");
            charData.Intellect = result->getUInt("intellect");
            charData.Spirit = result->getUInt("spirit");
            charData.Armor = result->getUInt("armor");
            charData.Resistances[0] = result->getUInt("resHoly");
            charData.Resistances[1] = result->getUInt("resFire");
            charData.Resistances[2] = result->getUInt("resNature");
            charData.Resistances[3] = result->getUInt("resFrost");
            charData.Resistances[4] = result->getUInt("resShadow");
            charData.Resistances[5] = result->getUInt("resArcane");
            charData.BlockChance = result->getDouble("blockPct");
            charData.DodgeChance = result->getDouble("dodgePct");
            charData.ParryChance = result->getDouble("parryPct");
            charData.CritChance = result->getDouble("critPct");
            charData.RangedCritChance = result->getDouble("rangedCritPct");
            charData.SpellCritChance = result->getDouble("spellCritPct");
            charData.AttackPower = result->getUInt("attackPower");
            charData.RangedAttackPower = result->getUInt("rangedAttackPower");
            charData.SpellPower = result->getUInt("spellPower");
            charData.Resilience = result->getUInt("resilience");
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "MySQL Query Error: " << e.what() << std::endl;
    }

    return charData;
}
