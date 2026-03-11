#include <bits/stdc++.h>

using namespace std;

template <typename K, typename V>
class MapStore {
    unordered_map<K, V> data;
public:
    void insert(const K& key, const V& val) {
        data[key] = val;
    }

    V& fetch(const K& key) {
        auto it = data.find(key);
        if (it == data.end())
            throw runtime_error("Key not found in MapStore.");
        return it->second;
    }

    bool exists(const K& key) {
        return data.count(key) > 0;
    }
};

// ABSTRACT BASE CLASS FOR FILE READING
class Base_Reader {
public:
    virtual bool open(const string& path) = 0;
    virtual int  readChunk(char* buf, int size) = 0;
    virtual void close() = 0;
    virtual ~Base_Reader() {}
};

// DERIVED CLASS 1: A CLASS FOR BUFFERED FILE READING
class Buffered_Reader : public Base_Reader {
    ifstream file;
public:
    bool open(const string& path) override {
        file.open(path, ios::binary);
        return file.is_open();
    }

    int readChunk(char* buf, int size) override {
        file.read(buf, size);
        return (int)file.gcount();
    }

    void close() override {
        if (file.is_open())
            file.close();
    }
};

// DERIVED CLASS 2: A CLASS FOR STRING DATA READING
class String_Reader : public Base_Reader {
    string dummy_Data;
    int pos = 0;
public:
    bool open(const string& path) override { 
        dummy_Data = "sample data for testing polymorphism";
        return true; 
    }
    int readChunk(char* buf, int size) override {
        int bytes = min(size, (int)dummy_Data.size() - pos);
        if (bytes <= 0) return 0;
        copy(dummy_Data.begin() + pos, dummy_Data.begin() + pos + bytes, buf);
        pos += bytes;
       return bytes;
    }
    void close() override { pos = 0; }
};

// AN ABSTRACT BASE CLASS FOR TOKENIZATION
class Base_Tokenizer {
public:
    virtual void feed(const char* data, int len,const string& leftover, string& newLeftover,unordered_map<string, long long>& index) = 0;
    virtual ~Base_Tokenizer() {}
};

// A DERIVED CLASS FOR TOKENIZATION
class Word_Tokenizer : public Base_Tokenizer {

    // ALPHANUMERIC
    bool isAlNum(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
    }

    // CASE INSENSITIVE
    char to_Low(char c) {
        if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
        return c;
    }

public:
    void feed(const char* data, int len, const string& leftover, string& newLeftover, unordered_map<string, long long>& index) override {
        string cur = leftover;
        for (int i = 0; i < len; i++) {
            char c = data[i];
            if (isAlNum(c)) {
                cur += to_Low(c);
            } else {
                if (!cur.empty()) {
                    index[cur]++;
                    cur.clear();
                }
            }
        }
        newLeftover = cur;
    }

    // FUNCTION OVERLOADING
    void feed(const string& leftover, unordered_map<string, long long>& index) {
        if (!leftover.empty())
            index[leftover]++;
    }
};

// A CLASS FOR VERSIONED INDEXING
class Versioned_Index {
    MapStore<string, unordered_map<string, long long>> store;

public:
    void build_Index(const string& filepath, const string& version, int buff) {
        int buff_Size = buff * 1024;
        vector<char> buf(buff_Size);

        // RUNTIME POLYMORPHISM: Using base pointer for dynamic dispatch
        Base_Reader* reader = new Buffered_Reader(); 
        
        if (!reader->open(filepath)) {
            delete reader;
            throw runtime_error("Cannot open file: " + filepath);
        }

        unordered_map<string, long long> index;
        index.reserve(1 << 20);

        Word_Tokenizer tokenizer;
        string leftover = "";
        string newLeftover;

        int bytes;
        while ((bytes = reader->readChunk(buf.data(), buff_Size)) > 0) {
            tokenizer.feed(buf.data(), bytes, leftover, newLeftover, index);
            leftover = newLeftover;
        }
        tokenizer.feed(leftover, index);

        reader->close();
        delete reader; 

        store.insert(version, index);
    }

    long long query_Word_Count(const string& version, const string& word) {
        auto& idx = store.fetch(version);
        auto it = idx.find(word);
        if (it == idx.end()) return 0;
        return it->second;
    }

    vector<pair<string, long long>> query_topK(const string& version, int k) {
        auto& idx = store.fetch(version);
        vector<pair<string, long long>> vec(idx.begin(), idx.end());
        int limit = min(k, (int)vec.size());
        partial_sort(vec.begin(), vec.begin() + limit, vec.end(),
                     [](const pair<string,long long>& a, const pair<string,long long>& b) {
                         return a.second > b.second;
                     });
        vec.resize(limit);
        return vec;
    }

    long long query_Diff(const string& v1, const string& v2, const string& word) {
        long long c1 = query_Word_Count(v1, word);
        long long c2 = query_Word_Count(v2, word);
        return c2 - c1;
    }
};

// A CLASS FOR QUERY PROCESSING
class Query_Processor {
    Versioned_Index& vi;
    int buff;

public:
    Query_Processor(Versioned_Index& v, int b) : vi(v), buff(b) {}

    void Word_Count(const string& file, const string& version, const string& word) {
        auto t1 = chrono::high_resolution_clock::now();
        vi.build_Index(file, version, buff);
        long long count = vi.query_Word_Count(version, word);
        auto t2 = chrono::high_resolution_clock::now();
        double elapsed = chrono::duration<double>(t2 - t1).count();

        cout << "Version: " << version << "\n";
        cout << "Count: " << count << "\n";
        cout << "Buffer Size (KB): " << buff << "\n";
        cout << "Execution Time (s): " << elapsed << "\n";
    }

    void topK(const string& file, const string& version, int k) {
        auto t1 = chrono::high_resolution_clock::now();
        vi.build_Index(file, version, buff);
        auto results = vi.query_topK(version, k);
        auto t2 = chrono::high_resolution_clock::now();
        double elapsed = chrono::duration<double>(t2 - t1).count();

        cout << "Top-" << k << " words in version " << version << ":\n";
        for (auto& p : results) cout << p.first << " " << p.second << "\n";
        cout << "Buffer Size (KB): " << buff << "\n";
        cout << "Execution Time (s): " << elapsed << "\n";
    }

    void Difference(const string& f1, const string& v1, const string& f2, const string& v2, const string& word) {
        auto t1 = chrono::high_resolution_clock::now();
        vi.build_Index(f1, v1, buff);
        vi.build_Index(f2, v2, buff);
        long long diff = vi.query_Diff(v1, v2, word);
        auto t2 = chrono::high_resolution_clock::now();
        double elapsed = chrono::duration<double>(t2 - t1).count();

        cout << "Difference (v2 - v1): " << diff << "\n";
        cout << "Buffer Size (KB): " << buff << "\n";
        cout << "Execution Time (s): " << elapsed << "\n";
    }
};

string getArg(int argc, char* argv[], const string& flag) {
    for (int i = 1; i < argc - 1; i++) {
        if (string(argv[i]) == flag) return string(argv[i + 1]);
    }
    return "";
}

int main(int argc, char* argv[]) {
    try {
        string query = getArg(argc, argv, "--query");
        string bufStr = getArg(argc, argv, "--buffer");

        if (query.empty() || bufStr.empty())
            throw runtime_error("Missing required arguments --query or --buffer.");

        int buff = stoi(bufStr);
        if (buff < 256 || buff > 1024)
            throw runtime_error("Buffer size must be between 256 and 1024 KB.");

        Versioned_Index vi;
        Query_Processor qp(vi, buff);

        if (query == "word") {
            string file = getArg(argc, argv, "--file");
            string version = getArg(argc, argv, "--version");
            string word = getArg(argc, argv, "--word");
            if (file.empty() || version.empty() || word.empty())
                throw runtime_error("Missing arguments for word query.");
            for (char& c : word) c = tolower(c);
            qp.Word_Count(file, version, word);
        }
        else if (query == "top") {
            string file = getArg(argc, argv, "--file");
            string version = getArg(argc, argv, "--version");
            string topStr = getArg(argc, argv, "--top");
            if (file.empty() || version.empty() || topStr.empty())
                throw runtime_error("Missing arguments for top query.");
            qp.topK(file, version, stoi(topStr));
        }
        else if (query == "diff") {
            string f1 = getArg(argc, argv, "--file1");
            string v1 = getArg(argc, argv, "--version1");
            string f2 = getArg(argc, argv, "--file2");
            string v2 = getArg(argc, argv, "--version2");
            string word = getArg(argc, argv, "--word");
            if (f1.empty() || v1.empty() || f2.empty() || v2.empty() || word.empty())
                throw runtime_error("Missing arguments for diff query.");
            for (char& c : word) c = tolower(c);
            qp.Difference(f1, v1, f2, v2, word);
        }
        else {
            throw runtime_error("Unknown query type: " + query);
        }

    }
    catch (exception& e) {
        cout << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}