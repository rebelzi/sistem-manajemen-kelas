#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream> // Untuk stringstream

using namespace std;

// Struct dan Node tetap sama
struct Jadwal {
    int id;
    string matkul;
    string dosen;
    string ruangan;
    string hari;
    string jam;
};

struct Node {
    Jadwal data;
    Node* next;
};

Node* head = NULL;
int idCounter = 1;

// ==========================================================
// BAGIAN BARU: HELPER UNTUK KONVERSI WAKTU
// ==========================================================

// Fungsi untuk mengubah string "HH.MM" menjadi total menit
int waktuKeMenit(const string& waktuStr) {
    try {
        size_t pos = waktuStr.find('.');
        if (pos == string::npos) return -1; // Format salah

        int jam = stoi(waktuStr.substr(0, pos));
        int menit = stoi(waktuStr.substr(pos + 1));
        return (jam * 60) + menit;
    } catch (...) {
        return -1; // Error jika konversi gagal
    }
}


// ==========================================================
// BAGIAN YANG DIPERBAIKI: LOGIKA CEK KONFLIK
// ==========================================================

bool cekKonflik(Jadwal jadwalBaru) {
    // 1. Parse waktu mulai dan selesai untuk jadwal baru
    size_t pemisahBaru = jadwalBaru.jam.find('-');
    if (pemisahBaru == string::npos) {
        cout << "\n[ERROR] Format waktu baru salah: " << jadwalBaru.jam << endl;
        return true; // Anggap konflik jika format salah
    }
    int mulaiBaru = waktuKeMenit(jadwalBaru.jam.substr(0, pemisahBaru));
    int selesaiBaru = waktuKeMenit(jadwalBaru.jam.substr(pemisahBaru + 1));
    
    if (mulaiBaru == -1 || selesaiBaru == -1) {
        cout << "\n[ERROR] Format waktu baru tidak valid." << endl;
        return true;
    }

    Node* current = head;
    while (current != NULL) {
        // Hanya cek jika hari dan (dosen atau ruangan) sama
        if (current->data.hari == jadwalBaru.hari &&
            (current->data.dosen == jadwalBaru.dosen || current->data.ruangan == jadwalBaru.ruangan)) {
            
            // 2. Parse waktu mulai dan selesai untuk jadwal yang sudah ada di list
            size_t pemisahAda = current->data.jam.find('-');
            if (pemisahAda == string::npos) {
                current = current->next;
                continue; // Abaikan data yang formatnya salah
            }
            int mulaiAda = waktuKeMenit(current->data.jam.substr(0, pemisahAda));
            int selesaiAda = waktuKeMenit(current->data.jam.substr(pemisahAda + 1));

            if(mulaiAda == -1 || selesaiAda == -1) {
                current = current->next;
                continue;
            }

            // 3. Cek tumpang tindih menggunakan formula
            if (mulaiBaru < selesaiAda && mulaiAda < selesaiBaru) {
                string pemicu = (current->data.dosen == jadwalBaru.dosen) ? "Dosen '" + jadwalBaru.dosen + "'" : "Ruangan '" + jadwalBaru.ruangan + "'";
                cout << "\n[KONFLIK] " << pemicu << " sudah ada jadwal lain (" << current->data.jam << ") pada rentang waktu tersebut!" << endl;
                return true;
            }
        }
        current = current->next;
    }
    return false;
}

// ... (Fungsi lainnya seperti tambahJadwal, tampilkanJadwal, updateJadwal, dll. tetap sama)
// Cukup salin dan tempel sisa kode dari jawaban sebelumnya karena perubahannya hanya di cekKonflik.


// --- SALIN SISA KODE DARI JAWABAN SEBELUMNYA DI SINI ---
// Fungsi tambahJadwal, tampilkanJadwal, updateJadwal, hapusJadwal, 
// cariJadwal, urutkanJadwal, dan main() tidak perlu diubah.
// Mereka akan secara otomatis menggunakan `cekKonflik` yang baru.

// ==========================================================
// BAGIAN 3: FUNGSI CRUD (CREATE, READ, UPDATE, DELETE)
// ==========================================================

void tambahJadwal() {
    Jadwal jadwalBaru;
    cout << "\n--- Tambah Jadwal Baru ---" << endl;
    cout << "Nama Mata Kuliah: ";
    cin.ignore();
    getline(cin, jadwalBaru.matkul);
    cout << "Nama Dosen Pengajar: ";
    getline(cin, jadwalBaru.dosen);
    cout << "Kode Ruangan: ";
    getline(cin, jadwalBaru.ruangan);
    cout << "Hari (e.g., Senin): ";
    getline(cin, jadwalBaru.hari);
    cout << "Jam (Format 07.00-08.40): ";
    getline(cin, jadwalBaru.jam);

    // Cek konflik sebelum menambahkan
    if (cekKonflik(jadwalBaru)) {
        cout << "Gagal menambahkan jadwal karena terjadi konflik." << endl;
        return;
    }
    
    jadwalBaru.id = idCounter++;
    Node* newNode = new Node();
    newNode->data = jadwalBaru;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
    } else {
        Node* temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    cout << "\n[SUKSES] Jadwal berhasil ditambahkan dengan ID: " << jadwalBaru.id << endl;
}

void tampilkanJadwal(const vector<Jadwal>& listJadwal) {
    cout << "\n--- Daftar Semua Jadwal ---" << endl;
    if (listJadwal.empty()) {
        cout << "Belum ada data jadwal." << endl;
        return;
    }
    
    for (const auto& j : listJadwal) {
        cout << "ID      : " << j.id << endl;
        cout << "Matkul  : " << j.matkul << endl;
        cout << "Dosen   : " << j.dosen << endl;
        cout << "Ruangan : " << j.ruangan << endl;
        cout << "Waktu   : " << j.hari << ", " << j.jam << endl;
        cout << "---------------------------------" << endl;
    }
}

// Overload function untuk menampilkan dari Linked List langsung
void tampilkanJadwal() {
    vector<Jadwal> tempVector;
    Node* current = head;
    while(current != NULL){
        tempVector.push_back(current->data);
        current = current->next;
    }
    tampilkanJadwal(tempVector);
}

void updateJadwal() {
    int id;
    cout << "\nMasukkan ID Jadwal yang akan diupdate: ";
    cin >> id;

    Node* current = head;
    bool ditemukan = false;
    Node* targetNode = NULL;
    while (current != NULL) {
        if (current->data.id == id) {
            ditemukan = true;
            targetNode = current;
            break;
        }
        current = current->next;
    }
    
    if(!ditemukan) {
        cout << "Jadwal dengan ID " << id << " tidak ditemukan." << endl;
        return;
    }

    Jadwal jadwalUpdate;
    cout << "--- Update Jadwal ID " << id << " ---" << endl;
    cout << "Nama Mata Kuliah Baru: ";
    cin.ignore();
    getline(cin, jadwalUpdate.matkul);
    cout << "Nama Dosen Baru: ";
    getline(cin, jadwalUpdate.dosen);
    cout << "Kode Ruangan Baru: ";
    getline(cin, jadwalUpdate.ruangan);
    cout << "Hari Baru: ";
    getline(cin, jadwalUpdate.hari);
    cout << "Jam Baru (Format HH.MM-HH.MM): ";
    getline(cin, jadwalUpdate.jam);
    
    // Simpan data lama dan hapus sementara dari list untuk pengecekan
    Jadwal dataLama = targetNode->data;
    targetNode->data.id = -99; // Ubah ID agar tidak konflik dengan diri sendiri
    
    if(cekKonflik(jadwalUpdate)){
        cout << "Gagal mengupdate jadwal karena terjadi konflik." << endl;
        targetNode->data = dataLama; // Kembalikan data lama
        return;
    }

    // Jika tidak konflik, update permanen
    targetNode->data = jadwalUpdate;
    targetNode->data.id = id; // Kembalikan ID asli
    cout << "\n[SUKSES] Jadwal berhasil diupdate." << endl;
}

void hapusJadwal() {
    int id;
    cout << "\nMasukkan ID Jadwal yang akan dihapus: ";
    cin >> id;

    Node* current = head;
    Node* prev = NULL;
    bool ditemukan = false;

    // Kasus khusus: menghapus head
    if (current != NULL && current->data.id == id) {
        head = current->next;
        delete current;
        cout << "\n[SUKSES] Jadwal berhasil dihapus." << endl;
        return;
    }

    // Mencari node yang akan dihapus
    while (current != NULL && current->data.id != id) {
        prev = current;
        current = current->next;
    }

    // Jika ditemukan
    if (current != NULL) {
        prev->next = current->next;
        delete current;
        cout << "\n[SUKSES] Jadwal berhasil dihapus." << endl;
    } else {
        cout << "Jadwal dengan ID " << id << " tidak ditemukan." << endl;
    }
}


// ==========================================================
// BAGIAN 4: FUNGSI SEARCHING & SORTING
// ==========================================================

void cariJadwal() {
    string keyword;
    cout << "\nMasukkan kata kunci (Dosen/Matkul/Ruangan/Hari): ";
    cin.ignore();
    getline(cin, keyword);
    transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower); // Ubah keyword ke lowercase

    vector<Jadwal> hasilPencarian;
    Node* current = head;
    while (current != NULL) {
        string matkul = current->data.matkul;
        string dosen = current->data.dosen;
        string ruangan = current->data.ruangan;
        string hari = current->data.hari;
        
        transform(matkul.begin(), matkul.end(), matkul.begin(), ::tolower);
        transform(dosen.begin(), dosen.end(), dosen.begin(), ::tolower);
        transform(ruangan.begin(), ruangan.end(), ruangan.begin(), ::tolower);
        transform(hari.begin(), hari.end(), hari.begin(), ::tolower);

        if (matkul.find(keyword) != string::npos ||
            dosen.find(keyword) != string::npos ||
            ruangan.find(keyword) != string::npos ||
            hari.find(keyword) != string::npos) {
            hasilPencarian.push_back(current->data);
        }
        current = current->next;
    }
    
    if(hasilPencarian.empty()){
        cout << "\nTidak ada jadwal yang cocok dengan kata kunci '" << keyword << "'." << endl;
    } else {
        cout << "\n--- Hasil Pencarian ---" << endl;
        tampilkanJadwal(hasilPencarian);
    }
}

void urutkanJadwal() {
    if (head == NULL) {
        cout << "\nBelum ada data untuk diurutkan." << endl;
        return;
    }

    vector<Jadwal> tempVector;
    Node* current = head;
    while (current != NULL) {
        tempVector.push_back(current->data);
        current = current->next;
    }

    int pilihan;
    cout << "\nUrutkan berdasarkan:" << endl;
    cout << "1. Mata Kuliah (A-Z)" << endl;
    cout << "2. Dosen (A-Z)" << endl;
    cout << "3. Hari (Senin -> Jumat... dst)" << endl;
    cout << "Pilihan Anda: ";
    cin >> pilihan;
    
    if (pilihan == 1) {
        sort(tempVector.begin(), tempVector.end(), [](const Jadwal& a, const Jadwal& b) {
            return a.matkul < b.matkul;
        });
        cout << "\n--- Jadwal Diurutkan berdasarkan Mata Kuliah ---" << endl;
    } else if (pilihan == 2) {
        sort(tempVector.begin(), tempVector.end(), [](const Jadwal& a, const Jadwal& b) {
            return a.dosen < b.dosen;
        });
        cout << "\n--- Jadwal Diurutkan berdasarkan Dosen ---" << endl;
    } else if (pilihan == 3) {
        // Implementasi urutan hari kustom
        vector<string> urutanHari = {"senin", "selasa", "rabu", "kamis", "jumat", "sabtu", "minggu"};
        sort(tempVector.begin(), tempVector.end(), [&](const Jadwal& a, const Jadwal& b) {
            string hariA = a.hari;
            string hariB = b.hari;
            transform(hariA.begin(), hariA.end(), hariA.begin(), ::tolower);
            transform(hariB.begin(), hariB.end(), hariB.begin(), ::tolower);

            auto itA = find(urutanHari.begin(), urutanHari.end(), hariA);
            auto itB = find(urutanHari.begin(), urutanHari.end(), hariB);
            
            return distance(urutanHari.begin(), itA) < distance(urutanHari.begin(), itB);
        });
        cout << "\n--- Jadwal Diurutkan berdasarkan Hari ---" << endl;
    } else {
        cout << "Pilihan tidak valid." << endl;
        return;
    }

    tampilkanJadwal(tempVector);
}

// ==========================================================
// BAGIAN 5: FUNGSI MAIN (MENU UTAMA)
// ==========================================================

int main() {
    int pilihan;
    do {
        cout << "\n==============================================" << endl;
        cout << "   Aplikasi Manajemen Jadwal Kelas v2.0" << endl;
        cout << "==============================================" << endl;
        cout << "1. Tambah Jadwal" << endl;
        cout << "2. Tampilkan Semua Jadwal" << endl;
        cout << "3. Update Jadwal" << endl;
        cout << "4. Hapus Jadwal" << endl;
        cout << "5. Cari Jadwal" << endl;
        cout << "6. Urutkan Jadwal" << endl;
        cout << "0. Keluar" << endl;
        cout << "----------------------------------------------" << endl;
        cout << "Masukkan pilihan Anda: ";
        cin >> pilihan;

        switch (pilihan) {
            case 1:
                tambahJadwal();
                break;
            case 2:
                tampilkanJadwal();
                break;
            case 3:
                updateJadwal();
                break;
            case 4:
                hapusJadwal();
                break;
            case 5:
                cariJadwal();
                break;
            case 6:
                urutkanJadwal();
                break;
            case 0:
                cout << "Terima kasih telah menggunakan aplikasi ini." << endl;
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
                break;
        }
        if(pilihan != 0){
            cout << "\nTekan Enter untuk kembali ke menu...";
            cin.ignore();
            cin.get();
        }
        system("clear"); // Untuk Windows. Ganti dengan "clear" untuk Linux/Mac
    } while (pilihan != 0);

    return 0;
}