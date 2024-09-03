#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stack>
#include <queue>
#include <vector>
#include <cstring>
#include <algorithm>
#include <string>

constexpr int32_t TABLE_SIZE = 43;
constexpr uint32_t MAX_SOLDIER_NAME = 64;
constexpr uint32_t MAX_LINE = 512;
constexpr uint32_t MAX_TOKEN_SIZE = 128;
constexpr uint32_t MAX_HASH_SIZE = 16;

struct HashNode {
    std::string key;
    uint64_t  value;
    HashNode* next;

    HashNode(std::string key, uint64_t(value)) : key(key), value(value), next(nullptr) {}
};

class HashTable {
public:
    HashNode** table;

    std::string toLowerCase(std::string str) {
        std::string result = "";
        for (char c : str) {
            result += tolower(c);
        }
        return result;
    }

    int hashFunction(std::string key, int m) {
        uint64_t ans = 0;
        for (auto c : key) {
            ans = (31 * ans + c) % TABLE_SIZE;
        }
        return ans;
    }

    HashTable() {
        table = new HashNode * [TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }

    void insert(std::string key, uint64_t value) {
        key = toLowerCase(key);
        uint64_t hash = hashFunction(key, TABLE_SIZE);

        HashNode* newNode = new HashNode(key, value);
        if (table[hash] == nullptr) {
            table[hash] = newNode;
        }
        else {
            HashNode* entry = table[hash];
            while (entry->next != nullptr) {
                entry = entry->next;
            }
            entry->next = newNode;
        }
    }


    int search(std::string key) {
        key = toLowerCase(key);
        int hash = hashFunction(key, TABLE_SIZE);
        HashNode* entry = table[hash];
        while (entry != nullptr) {
            if (entry->key == key) {
                return entry->value;
            }
            entry = entry->next;
        }
        return -1; // Indicador de que a chave nao foi encontrada
    }

    uint64_t sumValuesByKey(std::string key) {
        key = toLowerCase(key);
        uint64_t sum = 0;
        uint64_t hash = hashFunction(key, TABLE_SIZE);
        HashNode* entry = table[hash];
        while (entry != nullptr) {
            sum += entry->value;
            entry = entry->next;
        }
        return sum;
    }

    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode* entry = table[i];
            while (entry != nullptr) {
                HashNode* prev = entry;
                entry = entry->next;
                delete prev;
            }
        }
        delete[] table;
    }


    void printHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            std::cout << "Index " << i << ": ";
            HashNode* entry = table[i];
            while (entry != nullptr) {
                std::cout << "[" << entry->key << ": " << entry->value << "] -> ";
                entry = entry->next;
            }
            std::cout << "nullptr\n";
        }
    }

};

enum class Breed {
    Umashi,
    Azuri,
    Ianteco,
};

const char* breeds[] = {
    "UMASHI",
    "AZURI",
    "IANTECO",
};

const char* input =
R"(LEVI AZURI 5 5 20 XABLAU UMASHI 10 LUIZ AZURI 5
UTA
WILLIAM IANTECO 2 5 15 PIRES AZURI 10
UTA
YAGOV UMASHI 8 5 10
UTA
RODRIGO AZURI 5 8 34
UTA
FELIPE IANTECO 4 3 3 OLGA AZURI 4 JULIANY AZURI 1 DOM UMASHI 2
STO
FRANCISCO UMASHI 30
MARCELA AZURI 40
SERGIO IANTECO 90
CMD
FORCA 5 5
TERR 5 5
MAX_ATAQUE 5 5
FORCA 5 5
MAX_ATAQUE 4 3
FIM)";

const char* convert_breed_to_string(Breed breed) {
    const char* output;

    switch (breed) {
    case Breed::Umashi:
        output = "UMASHI";
        break;
    case Breed::Azuri:
        output = "AZURI";
        break;
    case Breed::Ianteco:
        output = "IANTECO";
        break;
    default:
        output = "UNKNOWN";
        break;
    }

    return output;
}

struct Substr {
    char data[MAX_TOKEN_SIZE];
};

struct CommonSoldier {
    char name[MAX_SOLDIER_NAME];
    Breed breed;
    uint64_t power;
};

struct ActiveSoldier {
    CommonSoldier common_data;
    int32_t pos_x, pos_y;
    std::queue<CommonSoldier> captured;
};

struct SanctuarySoldier {
    char hash[MAX_HASH_SIZE];
    Breed breed;
    uint64_t power;
};

struct Team {
    uint64_t total_power;
    std::stack<ActiveSoldier> soldiers;

    Breed get_breed() {
        return soldiers.top().common_data.breed;
    }

    void sort() {
        uint32_t size = static_cast<uint32_t>(soldiers.size());

        std::vector<ActiveSoldier> input;
        for (uint32_t index = 0; index < size; ++index) {
            input.push_back(soldiers.top());
            soldiers.pop();
        }

        std::sort(input.begin(), input.end(), [](ActiveSoldier& s0, ActiveSoldier& s1) {
            return s0.common_data.power < s1.common_data.power;
            });

        for (auto& soldier : input) {
            soldiers.push(soldier);
        }
    }
};

struct Battlefield {
    std::vector<ActiveSoldier> umashi_soldiers;
    std::vector<ActiveSoldier> azuri_soldiers;
    std::vector<ActiveSoldier> ianteco_soldiers;
    std::vector<SanctuarySoldier> sanctuary;
    Team matrix[10][10];

    void sort_matrix() {
        for (uint32_t i = 0; i < 10; ++i) {
            for (uint32_t j = 0; j < 10; ++j) {
                Team* at = &matrix[i][j];
                if (!at->soldiers.empty()) {
                    at->sort();
                }
            }
        }
    }

    uint64_t breed_total_power(Breed target) {
        std::vector<ActiveSoldier>* list = nullptr;

        switch (target) {
        case Breed::Umashi:
            list = &umashi_soldiers;
            break;

        case Breed::Azuri:
            list = &azuri_soldiers;
            break;

        case Breed::Ianteco:
            list = &ianteco_soldiers;
            break;
        }

        uint64_t total_power = 0;
        for (int index = 0; index < list->size(); ++index) {
            total_power += list->at(index).common_data.power;
        }

        return total_power;
    }

    void breed_remove_list(Breed target, ActiveSoldier soldier) {
        std::vector<ActiveSoldier>* list = nullptr;

        switch (target) {
        case Breed::Umashi:
            list = &umashi_soldiers;
            break;

        case Breed::Azuri:
            list = &azuri_soldiers;
            break;

        case Breed::Ianteco:
            list = &ianteco_soldiers;
            break;
        }

        Breed breed = list->front().common_data.breed;
        for (int index = 0; index < list->size(); ++index) {
            if (strcmp(list->at(index).common_data.name, soldier.common_data.name) == 0) {
                list->erase(list->begin() + index);
            }
        }
    }
    void breed_eliminated(Breed target) {
        std::vector<ActiveSoldier>* list = nullptr;

        switch (target) {
        case Breed::Umashi:
            list = &umashi_soldiers;
            break;

        case Breed::Azuri:
            list = &azuri_soldiers;
            break;

        case Breed::Ianteco:
            list = &ianteco_soldiers;
            break;
        }


        if (list->size() == 0) {
            const char* breed = convert_breed_to_string(target);
            std::cout << breed << " foi eliminada\n";
        }
    }
};

void tokenize(const char* line, std::vector<Substr>& tokens) {
    uint32_t offset = 0;
    uint32_t substr_start = 0;
    uint32_t substr_end = 0;

    const char* cpy = line;
    while (true) {
        const uint8_t at = *cpy;

        if (at == ' ' || at == '\n' || at == '\0') {
            // Mark the substring end, copy to vector
            substr_end = offset;

            // Fetch size bytes and clamp it to not overflow the allowed substring size
            const uint32_t size = substr_end - substr_start;
            const uint32_t max = MAX_TOKEN_SIZE;
            const uint32_t clamp = size >= max ? max : size;

            // Prevent inserting whitespaces
            if ((offset - substr_start) > 0) {
                Substr str = {};
                strncpy(str.data, line + substr_start, clamp);
                tokens.push_back(str);
            }

            // Mark the new substring start
            substr_start = substr_end + 1;

            if (at == '\n' || at == '\0') { // Break condition
                break;
            }
        }

        ++cpy;
        ++offset;
    }
}

Breed convert_breed(const char* str) {
    Breed output;

    if (strcmp("UMASHI", str) == 0) {
        output = Breed::Umashi;
    }
    else if (strcmp("AZURI", str) == 0) {
        output = Breed::Azuri;
    }
    else if (strcmp("IANTECO", str) == 0) {
        output = Breed::Ianteco;
    }

    return output;
}

void maxAtaque(int x, int y, Battlefield bf, HashTable& hashTable) {
    uint64_t test = 0;
    uint64_t power = 0;
    Team* start = &bf.matrix[x][y];

    const Breed in_breed = start->get_breed();

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            Team* end = &bf.matrix[i][j];

            if (end->soldiers.size() > 0) {
                const Breed out_breed = end->get_breed();
                std::string n = convert_breed_to_string(out_breed);
            }

            else if (end->soldiers.size() == 0) {
                continue;
            }

            if (abs(x - i) + abs(y - j) <= 3 && in_breed != end->soldiers.top().common_data.breed) {
                Team* end = &bf.matrix[i][j];
                if (start->total_power > end->total_power) {
                    if (end->soldiers.top().captured.size() == 0) {
                        test = end->soldiers.top().common_data.power;
                    }

                }
                else if (start->total_power < 2 * (end->total_power) / 3) {
                    Breed temp = start->get_breed();

                    const char* breed1 = convert_breed_to_string(temp);
                    std::string breed = std::string(breed1);
                    std::string name = start->soldiers.top().common_data.name;

                    uint64_t sanctuary = 0;

                    sanctuary = hashTable.sumValuesByKey(name + breed);

                    sanctuary = (sanctuary * 70) / 100;

                    if ((start->total_power + sanctuary) > end->total_power) {
                        if (end->soldiers.top().captured.size() == 0) {
                            test = end->soldiers.top().common_data.power;
                        }
                    }
                }
                if (test > power) {
                    power = test;
                }
            }
        }
    }
    std::cout << power + start->total_power << "\n";
}

void process_uta(Battlefield& bf, const std::vector<Substr>& tokens) {
    // Common Data
    const char* name = tokens.at(0).data;
    const char* breed = tokens.at(1).data;
    const uint64_t power = atoll(tokens.at(4).data);

    // Active Data
    const int32_t pos_x = atoi(tokens.at(2).data);
    const int32_t pos_y = atoi(tokens.at(3).data);

    const uint32_t captured_count = (static_cast<uint32_t>(tokens.size()) - 5) / 3;

    ActiveSoldier soldier;
    strcpy(soldier.common_data.name, name);
    soldier.common_data.power = power;
    soldier.common_data.breed = convert_breed(breed);
    soldier.pos_x = pos_x;
    soldier.pos_y = pos_y;

    uint32_t token_start = 5;
    for (uint32_t index = 0; index < captured_count; ++index) {
        CommonSoldier comm{};
        strcpy(comm.name, tokens.at(token_start).data);
        comm.breed = convert_breed(tokens.at(token_start + 1).data);
        comm.power = atoll(tokens.at(token_start + 2).data);

        soldier.common_data.power += comm.power;  // add a forca do soldado capturado na unidade tatica

        soldier.captured.push(comm);
        token_start += 3;
    }

    if (strcmp("UMASHI", breed) == 0) {
        bf.umashi_soldiers.push_back(soldier);
    }
    else if (strcmp("AZURI", breed) == 0) {
        bf.azuri_soldiers.push_back(soldier);
    }
    else if (strcmp("IANTECO", breed) == 0) {
        bf.ianteco_soldiers.push_back(soldier);
    }

    const int32_t x = soldier.pos_x;
    const int32_t y = soldier.pos_y;
    bf.matrix[x][y].total_power += soldier.common_data.power;
    bf.matrix[x][y].soldiers.push(soldier);
}

void process_sto(const std::vector<Substr>& tokens, HashTable& hashtable) {
    CommonSoldier comm{};
    std::string name1 = tokens.at(0).data;
    std::string breed1 = tokens.at(1).data;
    const uint64_t power = atoll(tokens.at(2).data);

    std::string name = name1;
    std::string breed = breed1;
    hashtable.insert(name + breed, power);
    //hashtable.printHashTable();
}

void process_cmd(Battlefield& bf, const std::vector<Substr>& tokens, HashTable& hashTable) {
    const char* command = tokens.at(0).data;

    if (strcmp(command, "FORCA") == 0) {
        const int32_t pos_x = atoi(tokens.at(1).data);
        const int32_t pos_y = atoi(tokens.at(2).data);

        if (bf.matrix[pos_x][pos_y].soldiers.empty()) {
            std::cout << -1 << std::endl;
        }
        else {
            uint64_t power = bf.matrix[pos_x][pos_y].total_power;
            std::cout << power << std::endl;
        }
    }
    else if (strcmp(command, "FORCA_RACA") == 0) {
        Breed breed = convert_breed(tokens.at(1).data);
        uint64_t max_power = bf.breed_total_power(breed);
        std::cout << max_power << std::endl;
    }
    else if (strcmp(command, "TERR") == 0) {
        const int32_t pos_x = atoi(tokens.at(1).data);
        const int32_t pos_y = atoi(tokens.at(2).data);

        Team* at = &bf.matrix[pos_x][pos_y];

        const uint64_t total_power = at->total_power;
        const uint32_t soldier_counter = static_cast<uint32_t>(at->soldiers.size());

        std::cout << '[';

        if (!at->soldiers.empty()) {
            ActiveSoldier strongest = at->soldiers.top();

            const char* breed_name = breeds[static_cast<uint32_t>(strongest.common_data.breed)];

            if (soldier_counter > 1) std::cout << '[';
            std::cout << strongest.common_data.name << ' ' << breed_name << ' ' << total_power;
            if (soldier_counter > 1) std::cout << ']';
        }
        else {
            std::cout << ' ';
        }

        std::cout << "]\n";
    }
    else if (strcmp(command, "MAX_ATAQUE") == 0) {
        const int32_t x = atoi(tokens.at(1).data);
        const int32_t y = atoi(tokens.at(2).data);
        maxAtaque(x, y, bf, hashTable);
    }
    else if (strcmp(command, "MOVA") == 0) {
        const int32_t x1 = atoi(tokens.at(1).data);
        const int32_t y1 = atoi(tokens.at(2).data);
        const int32_t x2 = atoi(tokens.at(3).data);
        const int32_t y2 = atoi(tokens.at(4).data);

        // Movimento valido
        if (abs(x1 - x2) + abs(y1 - y2) <= 3) {
            Team* start = &bf.matrix[x1][y1];
            Team* end = &bf.matrix[x2][y2];

            const bool has_soldier_start = !start->soldiers.empty();
            const bool has_soldier_end = !end->soldiers.empty();

            if (has_soldier_start) {
                if (has_soldier_end) {
                    const Breed in_breed = start->get_breed();
                    const Breed out_breed = end->get_breed();
                    Breed in_breed1 = start->get_breed();

                    if (in_breed == out_breed) { // Unir tropas
                        const uint32_t count = static_cast<uint32_t>(start->soldiers.size());
                        for (uint32_t index = 0; index < count; ++index) {
                            end->soldiers.push(start->soldiers.top());
                            start->soldiers.pop();
                        }

                        end->total_power += start->total_power;
                        start->total_power = 0;
                    }
                    else { // Ataque
                        if (start->total_power > end->total_power) { // start mais forte
                            if (end->soldiers.top().captured.size() > 0) { // se end tiver capurados
                                CommonSoldier captured; // cria soldado

                                //pega os dados 
                                std::string name1 = end->soldiers.top().captured.front().name;
                                char breed1 = static_cast<char>(end->soldiers.top().captured.front().breed);
                                const uint64_t power = end->soldiers.top().captured.front().power;

                                hashTable.insert(name1 + breed1, power);
                                std::cout << "Ataque bem sucedido\n";
                            }
                            if (end->soldiers.top().captured.size() == 0) { // caso nao tenha capturados

                                char* name1 = end->soldiers.top().common_data.name;
                                char breed1 = static_cast<char>(end->soldiers.top().common_data.breed);
                                const uint64_t power = end->soldiers.top().common_data.power;
                                char nome = *name1;
                                CommonSoldier captured = { nome, breed1, power };

                                start->soldiers.top().captured.push(captured); // topo da pilha pro final da fila Nao consegui
                                end->soldiers.pop();
                                std::cout << "Ataque bem sucedido\n";

                                if (end->soldiers.size() == 0) {  // se zerar a tropa inimiga
                                    std::string name1 = start->soldiers.top().common_data.name;
                                    char breed1 = static_cast<char>(start->soldiers.top().common_data.breed);

                                    std::cout << "unidade " << name1 + breed1 << "avanca para (" << x2 << ", " << y2 << ")\n";
                                }
                            }
                        }

                        else if (start->total_power <= 2 * (end->total_power) / 3) { // caso com santuario  
                            Breed breed1 = start->get_breed();
                            const char* breed = convert_breed_to_string(breed1);
                            std::string convert = std::string(breed);

                            uint64_t lend_strength = hashTable.sumValuesByKey(start->soldiers.top().common_data.name + convert); // consulta forca

                            start->total_power += (lend_strength * 70) / 100;


                            if (start->total_power >= end->total_power) { // se o poder mais 70 por cento do santuario for maior q o end
                                start->total_power -= (lend_strength * 70) / 100;

                                if (end->soldiers.top().captured.size() > 0) {

                                    std::string name1 = end->soldiers.top().captured.front().name;
                                    char breed1 = static_cast<char>(end->soldiers.top().captured.front().breed);
                                    const uint64_t power = end->soldiers.top().captured.front().power;

                                    hashTable.insert(name1 + breed1, power);
                                    std::cout << "Ataque bem sucedido\n";
                                }
                                if (end->soldiers.top().captured.size() == 0) { // caso nao tenha capturados
                                    CommonSoldier captured; //novo soldier

                                    char name1[MAX_SOLDIER_NAME];

                                    strcpy(captured.name, end->soldiers.top().common_data.name);
                                    Breed breed1 = end->soldiers.top().common_data.breed;
                                    const uint64_t power = end->soldiers.top().common_data.power;

                                    captured.breed = breed1;
                                    captured.power = power;


                                    start->soldiers.top().captured.push(captured); // topo da pilha pro final da fila
                                    start->soldiers.top().common_data.power += captured.power;

                                    end->total_power -= end->soldiers.top().common_data.power;

                                    end->soldiers.pop();      //desempilha final

                                    std::cout << "Ataque bem sucedido\n";

                                    if (end->soldiers.size() == 0) {  // se zerar a tropa inimiga

                                        std::string name1 = start->soldiers.top().common_data.name;
                                        char breed1 = static_cast<char>(start->soldiers.top().common_data.breed);

                                        std::cout << "unidade " << name1 + breed1 << "avanca para (" << x2 << "," << y2 << ")\n";

                                        end->total_power = start->total_power;
                                        start->total_power = 0; // Zera

                                        const uint32_t count = static_cast<uint32_t>(start->soldiers.size());
                                        for (uint32_t index = 0; index < count; ++index) {
                                            end->soldiers.push(start->soldiers.top());
                                            start->soldiers.pop();
                                        }
                                    }
                                }
                            }
                            else if (start->total_power < end->total_power) {
                                {
                                    start->total_power -= (lend_strength * 70) / 100;
                                    std::cout << "Contra-ataque!\n";

                                    if (start->soldiers.top().captured.size() > 0) {

                                        std::string name1 = start->soldiers.top().captured.front().name;
                                        char breed1 = static_cast<char>(start->soldiers.top().captured.front().breed);
                                        const uint64_t power = start->soldiers.top().captured.front().power;

                                        hashTable.insert(name1 + breed1, power);

                                    }
                                    if (start->soldiers.top().captured.size() == 0) { // caso nao tenha capturados
                                        CommonSoldier captured; //novo soldier

                                        char name1[MAX_SOLDIER_NAME];

                                        //pega nome, raca e poder pra fazer o cap e add na fila dps tirar da pilha

                                        strcpy(captured.name, start->soldiers.top().common_data.name);
                                        Breed breed1 = start->soldiers.top().common_data.breed;
                                        const uint64_t power = start->soldiers.top().common_data.power;

                                        captured.breed = breed1;
                                        captured.power = power;

                                        end->soldiers.top().captured.push(captured); // topo da pilha pro final da fila
                                        end->soldiers.top().common_data.power += captured.power; // inccrementa poder do soldado

                                        start->total_power -= start->soldiers.top().common_data.power; // decrementa poder total
                                        //teste
                                        end->total_power += start->soldiers.top().common_data.power; // incrementa poder total

                                        Breed elim = start->get_breed(); //pega raca 
                                        bf.breed_remove_list(start->get_breed(), start->soldiers.top()); // remove soldado da lista das racas



                                        start->soldiers.pop();      //desempilha final



                                        start->total_power -= (lend_strength * 70) / 100;

                                        //certo
                                        if (start->soldiers.size() == 0) { // se zerar a tropa inimiga (contra ataque)

                                            std::string name1 = end->soldiers.top().common_data.name;

                                            const char* breed1 = convert_breed_to_string(end->soldiers.top().common_data.breed);


                                            std::cout << "unidade " << name1 << "+" << breed1 << " avanca para (" << x1 << ", " << y1 << ")\n";

                                            bf.breed_eliminated(elim);

                                            start->total_power = end->total_power; // move
                                            end->total_power = 0; // Zera



                                            const uint32_t count = static_cast<uint32_t>(end->soldiers.size());
                                            for (uint32_t index = 0; index < count; ++index) {
                                                start->soldiers.push(end->soldiers.top());
                                                end->soldiers.pop();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else { // Move para posicao vazia
                    end->total_power = start->total_power;
                    start->total_power = 0; // Zera

                    const uint32_t count = static_cast<uint32_t>(start->soldiers.size());
                    for (uint32_t index = 0; index < count; ++index) {
                        end->soldiers.push(start->soldiers.top());
                        start->soldiers.pop();
                    }
                }
                bf.sort_matrix();
            }
        }
    }
}

void loop() {
    // Variaveis de verificacao de comandos reposicionadas
    bool uta = true;
    bool sto = false;
    bool cmd = false;

    uint32_t offset = 0;
    uint32_t line_start = 0;
    uint32_t line_end = 0;

    Battlefield bf{};
    HashTable hashTable;

    char* buffer = new char[4098];
    int32_t n_read = fread(buffer, sizeof(char), 4098, stdin);

    if (n_read < 4098) {
        buffer[n_read] = '\0';
    }

    while (true) {
        char line[MAX_LINE];
        memset(line, 0, MAX_LINE);

        char at = buffer[offset];

        if (at == '\n' || at == '\0') {
            line_end = offset;
            strncpy(line, input + line_start, line_end - line_start);

            line_start = line_end + 1;

            if (strcmp("FIM", line) == 0) {
                break;
            }
            else if (strcmp("UTA", line) == 0) { // Se for uta sa add o offset e continua
                uta = true;
                cmd = false;
                sto = false;
                ++offset;
                continue;
            }
            else if (strcmp("STO", line) == 0) { // se for sto a variavel uta fica falsa e a sto verdadeira e add 1 no offset
                bf.sort_matrix();

                uta = false;
                cmd = false;
                sto = true;
                ++offset;
                continue;
            }
            else if (strcmp("CMD", line) == 0) { // se for cmd ele troca as variaveis booleanas add offset e continua
                uta = false;
                sto = false;
                cmd = true;
                ++offset;
                continue;
            }

            std::vector<Substr> tokens;
            tokenize(line, tokens);

            if (uta) { // com uta true ele processa linhas
                process_uta(bf, tokens);
            }
            else if (sto) { // com sto true ele processa o santuario
                process_sto(tokens, hashTable);
            }
            else if (cmd) { // com cmd true ele processa o cmd
                process_cmd(bf, tokens, hashTable);
            }
        }

        ++offset;
    }

    delete[] buffer;

    std::cin.ignore();
    std::cin.get();
}

int main() {

    /*while (true) { // leitura de dados
        char linha[MAX_LINE];
        std::cin.getline(linha, MAX_LINE);

        // Aloca memoria para a nova linha
        char* linhaDinamica = new char[strlen(linha) + 1];
        strcpy(linhaDinamica, linha);

        list.push_back(linhaDinamica);

        if (strcmp(linha, "FIM") == 0) {
            break;
        }
    }*/
    loop();
    return 0;
}