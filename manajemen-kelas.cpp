#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <sstream>

using namespace std;

// ==========================================================
// STRUKTUR DATA UTAMA
// ==========================================================

struct Jadwal
{
    int id;
    string matkul;
    string dosen;
    string ruangan;
    string hari;
    string jam;

    // Constructor
    Jadwal() : id(0) {}
    Jadwal(int _id, string _matkul, string _dosen, string _ruangan, string _hari, string _jam)
        : id(_id), matkul(_matkul), dosen(_dosen), ruangan(_ruangan), hari(_hari), jam(_jam) {}
};

// Node untuk Linked List (penyimpanan utama)
struct Node
{
    Jadwal data;
    Node *next;

    Node() : next(NULL) {}
    Node(Jadwal _data) : data(_data), next(NULL) {}
};

// Node untuk Graf (deteksi konflik)
struct GraphNode
{
    int jadwalId;
    string resourceKey;            // "dosen_hari_jam" atau "ruangan_hari_jam"
    GraphNode **conflicts;         // Array pointer ke jadwal yang konflik
    int conflictCount;             // Jumlah konflik
    int maxConflicts;              // Kapasitas array

    GraphNode(int id, string key) : jadwalId(id), resourceKey(key), conflictCount(0), maxConflicts(10)
    {
        conflicts = new GraphNode*[maxConflicts];
    }
    
    ~GraphNode()
    {
        delete[] conflicts;
    }
    
    void addConflict(GraphNode* node)
    {
        if (conflictCount < maxConflicts)
        {
            conflicts[conflictCount++] = node;
        }
    }
};

// ==========================================================
// KELAS UTAMA: SISTEM MANAJEMEN JADWAL
// ==========================================================

class SistemManajemenJadwal
{
private:
    Node *head;                             // Linked List untuk penyimpanan
    int idCounter;                          // Counter untuk ID unik
    map<string, GraphNode *> conflictGraph; // Graf untuk deteksi konflik
    map<int, Jadwal> jadwalMap;             // Map untuk akses cepat berdasarkan ID

public:
    SistemManajemenJadwal() : head(NULL), idCounter(1) {}

    ~SistemManajemenJadwal()
    {
        // Cleanup Linked List
        while (head != NULL)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
        }

        // Cleanup Graph
        for (map<string, GraphNode *>::iterator it = conflictGraph.begin(); it != conflictGraph.end(); ++it)
        {
            delete it->second;
        }
    }

    // ==========================================================
    // HELPER FUNCTIONS
    // ==========================================================

    int waktuKeMenit(const string &waktuStr)
    {
        try
        {
            size_t pos = waktuStr.find('.');
            if (pos == string::npos)
                return -1;

            stringstream ss;
            int jam, menit;
            
            ss << waktuStr.substr(0, pos);
            ss >> jam;
            ss.clear();
            ss.str("");
            
            ss << waktuStr.substr(pos + 1);
            ss >> menit;
            
            return (jam * 60) + menit;
        }
        catch (...)
        {
            return -1;
        }
    }

    string generateResourceKey(const string &resource, const string &hari, const string &jam)
    {
        return resource + "_" + hari + "_" + jam;
    }

    bool cekTumpangTindihWaktu(const string &jam1, const string &jam2)
    {
        size_t pemisah1 = jam1.find('-');
        size_t pemisah2 = jam2.find('-');

        if (pemisah1 == string::npos || pemisah2 == string::npos)
            return false;

        int mulai1 = waktuKeMenit(jam1.substr(0, pemisah1));
        int selesai1 = waktuKeMenit(jam1.substr(pemisah1 + 1));
        int mulai2 = waktuKeMenit(jam2.substr(0, pemisah2));
        int selesai2 = waktuKeMenit(jam2.substr(pemisah2 + 1));

        if (mulai1 == -1 || selesai1 == -1 || mulai2 == -1 || selesai2 == -1)
            return false;

        return (mulai1 < selesai2) && (mulai2 < selesai1);
    }

    // ==========================================================
    // DETEKSI KONFLIK MENGGUNAKAN GRAF
    // ==========================================================

    string* deteksiKonflikGraf(const Jadwal &jadwalBaru, int &konflikCount)
    {
        konflikCount = 0;
        string* konflikDitemukan = new string[100]; // Array untuk menyimpan konflik

        Node *current = head;

        while (current != NULL)
        {
            if (current->data.hari == jadwalBaru.hari && current->data.dosen == jadwalBaru.dosen)
            {
                if (cekTumpangTindihWaktu(jadwalBaru.jam, current->data.jam))
                {
                    konflikDitemukan[konflikCount++] = "KONFLIK DOSEN: " + jadwalBaru.dosen +
                                               " sudah mengajar " + current->data.matkul +
                                               " di ruangan " + current->data.ruangan +
                                               " pada " + current->data.jam;
                }
            }

            // Cek konflik ruangan
            if (current->data.hari == jadwalBaru.hari && current->data.ruangan == jadwalBaru.ruangan)
            {
                if (cekTumpangTindihWaktu(jadwalBaru.jam, current->data.jam))
                {
                    konflikDitemukan[konflikCount++] = "KONFLIK RUANGAN: " + jadwalBaru.ruangan +
                                               " sudah digunakan untuk " + current->data.matkul +
                                               " oleh " + current->data.dosen +
                                               " pada " + current->data.jam;
                }
            }
            current = current->next;
        }

        return konflikDitemukan;
    }

    void tambahKeGraf(const Jadwal &jadwal)
    {
        // Tambah node untuk dosen
        string dosenKey = generateResourceKey(jadwal.dosen, jadwal.hari, jadwal.jam);
        if (conflictGraph.find(dosenKey) == conflictGraph.end())
        {
            conflictGraph[dosenKey] = new GraphNode(jadwal.id, dosenKey);
        }

        // Tambah node untuk ruangan
        string ruanganKey = generateResourceKey(jadwal.ruangan, jadwal.hari, jadwal.jam);
        if (conflictGraph.find(ruanganKey) == conflictGraph.end())
        {
            conflictGraph[ruanganKey] = new GraphNode(jadwal.id, ruanganKey);
        }

        // Tambah edges untuk konflik yang ada
        for (map<string, GraphNode *>::iterator it = conflictGraph.begin(); it != conflictGraph.end(); ++it)
        {
            GraphNode *node = it->second;
            if (node->jadwalId != jadwal.id)
            {
                map<int, Jadwal>::iterator jadwalIt = jadwalMap.find(node->jadwalId);
                if (jadwalIt != jadwalMap.end())
                {
                    Jadwal existingJadwal = jadwalIt->second;
                    if (existingJadwal.hari == jadwal.hari)
                    {
                        if ((existingJadwal.dosen == jadwal.dosen || existingJadwal.ruangan == jadwal.ruangan) &&
                            cekTumpangTindihWaktu(jadwal.jam, existingJadwal.jam))
                        {
                            // Tambah edge dua arah
                            conflictGraph[dosenKey]->addConflict(node);
                            node->addConflict(conflictGraph[dosenKey]);
                        }
                    }
                }
            }
        }
    }

    // ==========================================================
    // CRUD OPERATIONS (CREATE, READ, UPDATE, DELETE)
    // ==========================================================

    bool tambahJadwal(const string &matkul, const string &dosen, const string &ruangan,
                      const string &hari, const string &jam)
    {

        Jadwal jadwalBaru(idCounter, matkul, dosen, ruangan, hari, jam);

        // Deteksi konflik menggunakan graf
        int konflikCount = 0;
        string* konflik = deteksiKonflikGraf(jadwalBaru, konflikCount);

        if (konflikCount > 0)
        {
            cout << "\n[GAGAL] Tidak dapat menambahkan jadwal karena terjadi konflik:" << endl;
            for (int i = 0; i < konflikCount; i++)
            {
                cout << "  • " << konflik[i] << endl;
            }
            delete[] konflik;
            return false;
        }

        delete[] konflik;

        // Jika tidak ada konflik, tambahkan ke struktur data
        // 1. Tambah ke Linked List
        Node *newNode = new Node(jadwalBaru);
        if (head == NULL)
        {
            head = newNode;
        }
        else
        {
            Node *temp = head;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = newNode;
        }

        // 2. Tambah ke Map untuk akses cepat
        jadwalMap[jadwalBaru.id] = jadwalBaru;

        // 3. Tambah ke Graf untuk deteksi konflik
        tambahKeGraf(jadwalBaru);

        idCounter++;
        cout << "\n[SUKSES] Jadwal berhasil ditambahkan dengan ID: " << jadwalBaru.id << endl;
        return true;
    }

    void tampilkanSemuaJadwal()
    {
        cout << "\n"
             << string(60, '=') << endl;
        cout << "           DAFTAR SEMUA JADWAL KELAS" << endl;
        cout << string(60, '=') << endl;

        if (head == NULL)
        {
            cout << "Belum ada jadwal yang terdaftar." << endl;
            return;
        }

        Node *current = head;
        int nomor = 1;

        while (current != NULL)
        {
            cout << "\n[" << nomor << "] ID: " << current->data.id << endl;
            cout << "    Mata Kuliah : " << current->data.matkul << endl;
            cout << "    Dosen       : " << current->data.dosen << endl;
            cout << "    Ruangan     : " << current->data.ruangan << endl;
            cout << "    Waktu       : " << current->data.hari << ", " << current->data.jam << endl;
            cout << "    " << string(50, '-') << endl;

            current = current->next;
            nomor++;
        }
    }

    bool updateJadwal(int id, const string &matkul, const string &dosen, const string &ruangan,
                      const string &hari, const string &jam)
    {

        // Cari jadwal yang akan diupdate
        Node *current = head;
        Node *targetNode = NULL;

        while (current != NULL)
        {
            if (current->data.id == id)
            {
                targetNode = current;
                break;
            }
            current = current->next;
        }

        if (targetNode == NULL)
        {
            cout << "\n[GAGAL] Jadwal dengan ID " << id << " tidak ditemukan." << endl;
            return false;
        }

        // Simpan data lama
        Jadwal dataLama = targetNode->data;

        // Buat jadwal baru untuk validasi
        Jadwal jadwalBaru(id, matkul, dosen, ruangan, hari, jam);

        // Sementara hapus dari struktur untuk validasi
        jadwalMap.erase(id);

        // Cek konflik
        int konflikCount = 0;
        string* konflik = deteksiKonflikGraf(jadwalBaru, konflikCount);

        if (konflikCount > 0)
        {
            cout << "\n[GAGAL] Tidak dapat mengupdate jadwal karena terjadi konflik:" << endl;
            for (int i = 0; i < konflikCount; i++)
            {
                cout << "  • " << konflik[i] << endl;
            }
            // Kembalikan data lama
            jadwalMap[id] = dataLama;
            delete[] konflik;
            return false;
        }

        delete[] konflik;

        // Update data
        targetNode->data = jadwalBaru;
        jadwalMap[id] = jadwalBaru;

        cout << "\n[SUKSES] Jadwal berhasil diupdate." << endl;
        return true;
    }

    bool hapusJadwal(int id)
    {
        // Hapus dari Linked List
        if (head == NULL)
        {
            cout << "\n[GAGAL] Tidak ada jadwal untuk dihapus." << endl;
            return false;
        }

        // Jika yang dihapus adalah head
        if (head->data.id == id)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
            jadwalMap.erase(id);
            cout << "\n[SUKSES] Jadwal berhasil dihapus." << endl;
            return true;
        }

        // Cari dan hapus node
        Node *current = head;
        while (current->next != NULL && current->next->data.id != id)
        {
            current = current->next;
        }

        if (current->next == NULL)
        {
            cout << "\n[GAGAL] Jadwal dengan ID " << id << " tidak ditemukan." << endl;
            return false;
        }

        Node *nodeToDelete = current->next;
        current->next = nodeToDelete->next;
        delete nodeToDelete;
        jadwalMap.erase(id);

        cout << "\n[SUKSES] Jadwal berhasil dihapus." << endl;
        return true;
    }

    // ==========================================================
    // SEARCHING & SORTING MANUAL (TANPA VECTOR)
    // ==========================================================

    void cariJadwal(const string &keyword)
    {
        cout << "\n"
             << string(50, '=') << endl;
        cout << "  HASIL PENCARIAN: '" << keyword << "'" << endl;
        cout << string(50, '=') << endl;

        // Buat linked list sementara untuk hasil pencarian
        Node* hasilHead = NULL;
        Node* hasilTail = NULL;
        int jumlahHasil = 0;
        
        string keywordLower = keyword;
        transform(keywordLower.begin(), keywordLower.end(), keywordLower.begin(), ::tolower);

        Node *current = head;
        while (current != NULL)
        {
            string matkul = current->data.matkul;
            string dosen = current->data.dosen;
            string ruangan = current->data.ruangan;
            string hari = current->data.hari;

            transform(matkul.begin(), matkul.end(), matkul.begin(), ::tolower);
            transform(dosen.begin(), dosen.end(), dosen.begin(), ::tolower);
            transform(ruangan.begin(), ruangan.end(), ruangan.begin(), ::tolower);
            transform(hari.begin(), hari.end(), hari.begin(), ::tolower);

            if (matkul.find(keywordLower) != string::npos ||
                dosen.find(keywordLower) != string::npos ||
                ruangan.find(keywordLower) != string::npos ||
                hari.find(keywordLower) != string::npos)
            {
                // Tambah ke hasil pencarian
                Node* newNode = new Node(current->data);
                if (hasilHead == NULL)
                {
                    hasilHead = hasilTail = newNode;
                }
                else
                {
                    hasilTail->next = newNode;
                    hasilTail = newNode;
                }
                jumlahHasil++;
            }
            current = current->next;
        }

        if (jumlahHasil == 0)
        {
            cout << "Tidak ada jadwal yang cocok dengan keyword '" << keyword << "'." << endl;
        }
        else
        {
            Node* tempCurrent = hasilHead;
            int nomor = 1;
            while (tempCurrent != NULL)
            {
                cout << "\n[" << nomor << "] " << tempCurrent->data.matkul << endl;
                cout << "    Dosen   : " << tempCurrent->data.dosen << endl;
                cout << "    Ruangan : " << tempCurrent->data.ruangan << endl;
                cout << "    Waktu   : " << tempCurrent->data.hari << ", " << tempCurrent->data.jam << endl;
                tempCurrent = tempCurrent->next;
                nomor++;
            }
        }
        
        // Cleanup hasil pencarian
        while (hasilHead != NULL)
        {
            Node* temp = hasilHead;
            hasilHead = hasilHead->next;
            delete temp;
        }
    }

    void urutkanJadwal(int pilihan)
    {
        if (head == NULL)
        {
            cout << "\nTidak ada jadwal untuk diurutkan." << endl;
            return;
        }

        // Buat copy dari linked list untuk sorting
        Node* sortedHead = NULL;
        Node* current = head;
        
        // Copy semua data ke linked list sementara
        while (current != NULL)
        {
            Node* newNode = new Node(current->data);
            newNode->next = sortedHead;
            sortedHead = newNode;
            current = current->next;
        }

        // Bubble sort pada linked list
        bool swapped;
        do
        {
            swapped = false;
            Node* ptr1 = sortedHead;
            
            while (ptr1 != NULL && ptr1->next != NULL)
            {
                bool shouldSwap = false;
                
                switch (pilihan)
                {
                case 1: // Urutkan berdasarkan Mata Kuliah
                    shouldSwap = (ptr1->data.matkul > ptr1->next->data.matkul);
                    break;
                case 2: // Urutkan berdasarkan Dosen
                    shouldSwap = (ptr1->data.dosen > ptr1->next->data.dosen);
                    break;
                case 3: // Urutkan berdasarkan Hari
                    {
                        string urutanHari[7] = {"senin", "selasa", "rabu", "kamis", "jumat", "sabtu", "minggu"};
                        string hariA = ptr1->data.hari;
                        string hariB = ptr1->next->data.hari;
                        transform(hariA.begin(), hariA.end(), hariA.begin(), ::tolower);
                        transform(hariB.begin(), hariB.end(), hariB.begin(), ::tolower);

                        int indexA = -1, indexB = -1;
                        for (int k = 0; k < 7; k++)
                        {
                            if (urutanHari[k] == hariA)
                                indexA = k;
                            if (urutanHari[k] == hariB)
                                indexB = k;
                        }
                        shouldSwap = (indexA > indexB);
                    }
                    break;
                default:
                    cout << "Pilihan tidak valid." << endl;
                    // Cleanup
                    while (sortedHead != NULL)
                    {
                        Node* temp = sortedHead;
                        sortedHead = sortedHead->next;
                        delete temp;
                    }
                    return;
                }
                
                if (shouldSwap)
                {
                    // Swap data
                    Jadwal temp = ptr1->data;
                    ptr1->data = ptr1->next->data;
                    ptr1->next->data = temp;
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
        } while (swapped);

        // Tampilkan header sesuai pilihan
        switch (pilihan)
        {
        case 1:
            cout << "\n--- JADWAL DIURUTKAN BERDASARKAN MATA KULIAH ---" << endl;
            break;
        case 2:
            cout << "\n--- JADWAL DIURUTKAN BERDASARKAN DOSEN ---" << endl;
            break;
        case 3:
            cout << "\n--- JADWAL DIURUTKAN BERDASARKAN HARI ---" << endl;
            break;
        }

        // Tampilkan hasil pengurutan
        Node* displayCurrent = sortedHead;
        int nomor = 1;
        while (displayCurrent != NULL)
        {
            cout << "\n[" << nomor << "] " << displayCurrent->data.matkul << endl;
            cout << "    Dosen   : " << displayCurrent->data.dosen << endl;
            cout << "    Ruangan : " << displayCurrent->data.ruangan << endl;
            cout << "    Waktu   : " << displayCurrent->data.hari << ", " << displayCurrent->data.jam << endl;
            displayCurrent = displayCurrent->next;
            nomor++;
        }
        
        // Cleanup sorted list
        while (sortedHead != NULL)
        {
            Node* temp = sortedHead;
            sortedHead = sortedHead->next;
            delete temp;
        }
    }

    // ==========================================================
    // ANALISIS GRAF (FITUR TAMBAHAN)
    // ==========================================================

    void analisisKonflik()
    {
        cout << "\n"
             << string(60, '=') << endl;
        cout << "           ANALISIS KONFLIK MENGGUNAKAN GRAF" << endl;
        cout << string(60, '=') << endl;

        int totalKonflik = 0;
        set<pair<int, int> > konflikPairs;

        for (map<string, GraphNode *>::iterator it = conflictGraph.begin(); it != conflictGraph.end(); ++it)
        {
            GraphNode *node = it->second;
            for (int i = 0; i < node->conflictCount; i++)
            {
                GraphNode *conflictNode = node->conflicts[i];
                int id1 = min(node->jadwalId, conflictNode->jadwalId);
                int id2 = max(node->jadwalId, conflictNode->jadwalId);
                konflikPairs.insert(make_pair(id1, id2));
            }
        }

        if (konflikPairs.empty())
        {
            cout << "✅ Tidak ada konflik dalam jadwal saat ini." << endl;
        }
        else
        {
            cout << "⚠️  Ditemukan " << konflikPairs.size() << " konflik:" << endl;
            for (set<pair<int, int> >::iterator it = konflikPairs.begin(); it != konflikPairs.end(); ++it)
            {
                map<int, Jadwal>::iterator it1 = jadwalMap.find(it->first);
                map<int, Jadwal>::iterator it2 = jadwalMap.find(it->second);
                if (it1 != jadwalMap.end() && it2 != jadwalMap.end())
                {
                    cout << "  • Konflik antara:" << endl;
                    cout << "    - " << it1->second.matkul << " (" << it1->second.dosen << ")" << endl;
                    cout << "    - " << it2->second.matkul << " (" << it2->second.dosen << ")" << endl;
                }
            }
        }
    }
};

// ==========================================================
// FUNGSI MAIN DAN MENU
// ==========================================================

void tampilkanMenu()
{
    cout << "\n"
         << string(70, '=') << endl;
    cout << "                     SISTEM MANAJEMEN JADWAL KELAS" << endl;
    cout << "        Menggunakan Linked List dan Graf untuk Deteksi Konflik" << endl;
    cout << string(70, '=') << endl;
    cout << "1. 📅 Tambah Jadwal Baru" << endl;
    cout << "2. 📋 Tampilkan Semua Jadwal" << endl;
    cout << "3. ✏️  Update Jadwal" << endl;
    cout << "4. 🗑️  Hapus Jadwal" << endl;
    cout << "5. 🔍 Cari Jadwal" << endl;
    cout << "6. 📊 Urutkan Jadwal" << endl;
    cout << "7. 🔗 Analisis Konflik (Graf)" << endl;
    cout << "0. 🚪 Keluar" << endl;
    cout << string(70, '-') << endl;
    cout << "Pilihan Anda: ";
}

int main()
{
    SistemManajemenJadwal sistem;
    int pilihan;

    cout << "\n🎓 Selamat datang di Aplikasi Manajemen Jadwal Kelas!" << endl;
    cout << "   Sistem ini menggunakan Linked List untuk penyimpanan" << endl;
    cout << "   dan Graf untuk deteksi konflik secara otomatis." << endl;

    do
    {
        tampilkanMenu();
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
        {
            string matkul, dosen, ruangan, hari, jam;
            cout << "\n--- TAMBAH JADWAL BARU ---" << endl;
            cout << "Nama Mata Kuliah: ";
            cin.ignore();
            getline(cin, matkul);
            cout << "Nama Dosen: ";
            getline(cin, dosen);
            cout << "Kode Ruangan: ";
            getline(cin, ruangan);
            cout << "Hari (contoh: Senin): ";
            getline(cin, hari);
            cout << "Jam (Format: 08.00-09.40): ";
            getline(cin, jam);

            sistem.tambahJadwal(matkul, dosen, ruangan, hari, jam);
            break;
        }

        case 2:
            sistem.tampilkanSemuaJadwal();
            break;

        case 3:
        {
            int id;
            string matkul, dosen, ruangan, hari, jam;
            cout << "\n--- UPDATE JADWAL ---" << endl;
            cout << "Masukkan ID Jadwal yang akan diupdate: ";
            cin >> id;
            cout << "Data baru:" << endl;
            cout << "Nama Mata Kuliah: ";
            cin.ignore();
            getline(cin, matkul);
            cout << "Nama Dosen: ";
            getline(cin, dosen);
            cout << "Kode Ruangan: ";
            getline(cin, ruangan);
            cout << "Hari: ";
            getline(cin, hari);
            cout << "Jam: ";
            getline(cin, jam);

            sistem.updateJadwal(id, matkul, dosen, ruangan, hari, jam);
            break;
        }

        case 4:
        {
            int id;
            cout << "\n--- HAPUS JADWAL ---" << endl;
            cout << "Masukkan ID Jadwal yang akan dihapus: ";
            cin >> id;
            sistem.hapusJadwal(id);
            break;
        }

        case 5:
        {
            string keyword;
            cout << "\n--- CARI JADWAL ---" << endl;
            cout << "Masukkan kata kunci (mata kuliah/dosen/ruangan/hari): ";
            cin.ignore();
            getline(cin, keyword);
            sistem.cariJadwal(keyword);
            break;
        }

        case 6:
        {
            int pilihanSort;
            cout << "\n--- URUTKAN JADWAL ---" << endl;
            cout << "1. Berdasarkan Mata Kuliah" << endl;
            cout << "2. Berdasarkan Dosen" << endl;
            cout << "3. Berdasarkan Hari" << endl;
            cout << "Pilihan: ";
            cin >> pilihanSort;
            sistem.urutkanJadwal(pilihanSort);
            break;
        }

        case 7:
            sistem.analisisKonflik();
            break;

        case 0:
            cout << "\n🙏 Terima kasih telah menggunakan Sistem Manajemen Jadwal Kelas!" << endl;
            cout << "   Semoga membantu dalam mengelola jadwal dengan lebih efisien." << endl;
            break;

        default:
            cout << "\n❌ Pilihan tidak valid. Silakan coba lagi." << endl;
            break;
        }

        if (pilihan != 0)
        {
            cout << "\nTekan Enter untuk kembali ke menu...";
            cin.ignore();
            cin.get();
            system("clear"); // Untuk macOS/Linux. Ganti dengan "cls" untuk Windows
        }

    } while (pilihan != 0);

    return 0;
}
