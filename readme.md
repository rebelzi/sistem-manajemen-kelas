# 🎓 Sistem Manajemen Jadwal Kelas

> **Aplikasi Manajemen Jadwal Kelas Berbasis Struktur Data**  
> Menggunakan Linked List dan Graf untuk deteksi konflik otomatis

---

## 📋 Deskripsi Proyek

Aplikasi ini adalah sistem manajemen penjadwalan ruangan kelas yang dirancang untuk mengelola jadwal mata kuliah secara efisien. Sistem ini dapat mencegah terjadinya konflik jadwal (tumpang tindih) menggunakan implementasi struktur data Linked List dan Graf.

### 🎯 Tujuan Utama
- Mengelola penjadwalan ruangan kelas secara efisien
- Mencegah konflik jadwal dosen dan ruangan
- Mengimplementasikan struktur data untuk optimasi operasi
- Menyediakan interface yang user-friendly untuk manajemen jadwal

---

## 🏗️ Struktur Data yang Digunakan

### 1. **Linked List** (Penyimpanan Utama)
```cpp
struct Node {
    Jadwal data;
    Node *next;
};
```
- **Fungsi**: Menyimpan seluruh data jadwal secara dinamis
- **Keunggulan**: Fleksibel dalam penambahan/penghapusan data
- **Operasi**: Insert, Delete, Traversal, Search

### 2. **Graf** (Deteksi Konflik)
```cpp
struct GraphNode {
    int jadwalId;
    string resourceKey;
    GraphNode **conflicts;  // Array pointer untuk edges
    int conflictCount;
};
```
- **Fungsi**: Mendeteksi dan menganalisis konflik jadwal
- **Vertices**: Resource (dosen/ruangan) pada waktu tertentu
- **Edges**: Hubungan konflik antar jadwal
- **Algoritma**: Interval overlap detection

### 3. **Map** (Akses Cepat)
```cpp
map<int, Jadwal> jadwalMap;        // Quick access by ID
map<string, GraphNode*> conflictGraph;  // Graf structure
```
- **Fungsi**: Optimasi pencarian dan akses data
- **Kompleksitas**: O(log n) untuk operasi search/insert

### 4. **Set** (Unique Storage)
```cpp
set<pair<int, int> > konflikPairs;
```
- **Fungsi**: Menyimpan pasangan konflik yang unik
- **Keunggulan**: Otomatis sorted dan no duplicates

---

## 🚀 Fitur Utama

### 📅 **1. Tambah Jadwal Baru**
- Input data mata kuliah, dosen, ruangan, hari, dan jam
- **Deteksi konflik otomatis** sebelum menyimpan
- Validasi format waktu (HH.MM-HH.MM)

### 📋 **2. Tampilkan Semua Jadwal**
- Menampilkan seluruh jadwal yang tersimpan
- Format output yang terstruktur dan mudah dibaca

### ✏️ **3. Update Jadwal**
- Mengubah data jadwal berdasarkan ID
- **Re-validasi konflik** setelah perubahan
- Rollback otomatis jika terjadi konflik

### 🗑️ **4. Hapus Jadwal**
- Menghapus jadwal berdasarkan ID
- Cleanup otomatis dari semua struktur data

### 🔍 **5. Cari Jadwal (Search Manual)**
- Pencarian berdasarkan keyword (mata kuliah, dosen, ruangan, hari)
- **Implementasi manual** tanpa menggunakan library vector
- Case-insensitive search
- Menampilkan semua hasil yang match

### 📊 **6. Urutkan Jadwal (Sorting Manual)**
- **Bubble Sort** implementasi manual pada Linked List
- Pilihan sorting:
  - Berdasarkan Mata Kuliah (A-Z)
  - Berdasarkan Dosen (A-Z)  
  - Berdasarkan Hari (Senin → Minggu)
- **Tanpa menggunakan vector** - pure linked list operations

### 🔗 **7. Analisis Konflik (Graf)**
- Analisis mendalam menggunakan struktur graf
- Deteksi semua konflik yang ada dalam sistem
- Visualisasi hubungan antar jadwal yang bertentangan

---

## 🛠️ Teknologi dan Library

```cpp
#include <iostream>    // Input/Output operations
#include <string>      // String operations  
#include <algorithm>   // transform(), min(), max()
#include <map>         // Associative containers
#include <set>         // Unique sorted containers
#include <sstream>     // String stream operations (C++98 compatible)
```

### 📌 **Kompatibilitas**: C++98/C++03
- Tidak menggunakan fitur C++11 (auto, range-based for, nullptr)
- Compatible dengan compiler lama
- Menggunakan stringstream sebagai pengganti stoi()

---

## 🔧 Instalasi dan Kompilasi

### Prerequisites
- G++ Compiler
- C++98/C++03 compatible environment

### Langkah Kompilasi
```bash
# Clone atau download project
cd "/path/to/project/FP"

# Kompilasi program
g++ -o manajemen-kelas manajemen-kelas.cpp

# Jalankan program
./manajemen-kelas
```

### Untuk Windows
```cmd
g++ -o manajemen-kelas.exe manajemen-kelas.cpp
manajemen-kelas.exe
```

---

## 💡 Cara Penggunaan

### 🎮 Menu Utama
```
======================================================================
              SISTEM MANAJEMEN JADWAL KELAS
        Menggunakan Linked List dan Graf untuk Deteksi Konflik
======================================================================
1. 📅 Tambah Jadwal Baru
2. 📋 Tampilkan Semua Jadwal  
3. ✏️  Update Jadwal
4. 🗑️  Hapus Jadwal
5. 🔍 Cari Jadwal
6. 📊 Urutkan Jadwal
7. 🔗 Analisis Konflik (Graf)
0. 🚪 Keluar
```

### 📝 **Contoh Input Jadwal**
```
Nama Mata Kuliah: Struktur Data
Nama Dosen: Dr. Ahmad Subali
Kode Ruangan: Lab A101
Hari: Senin
Jam: 08.00-09.40
```

### ⚠️ **Contoh Deteksi Konflik**
```
[GAGAL] Tidak dapat menambahkan jadwal karena terjadi konflik:
  • KONFLIK DOSEN: Dr. Ahmad Subali sudah mengajar Algoritma 
    di ruangan Lab B102 pada 08.00-09.40
  • KONFLIK RUANGAN: Lab A101 sudah digunakan untuk Pemrograman 
    oleh Dr. Budi Santoso pada 08.30-10.10
```

---

## 🧮 Algoritma yang Diimplementasikan

### 1. **Deteksi Konflik Waktu**
```cpp
bool cekTumpangTindihWaktu(const string &jam1, const string &jam2) {
    // Interval overlap: (mulai1 < selesai2) && (mulai2 < selesai1)
    return (mulai1 < selesai2) && (mulai2 < selesai1);
}
```
- **Kompleksitas**: O(1)
- **Formula**: Interval overlap detection

### 2. **Bubble Sort untuk Linked List**
```cpp
void urutkanJadwal(int pilihan) {
    // Bubble sort implementation pada linked list
    // Swap data antar node berdasarkan kriteria
}
```
- **Kompleksitas**: O(n²)
- **Implementasi**: Manual tanpa vector

### 3. **Linear Search pada Linked List**
```cpp
void cariJadwal(const string &keyword) {
    // Traversal linear dengan pattern matching
    // Case-insensitive search
}
```
- **Kompleksitas**: O(n)
- **Implementasi**: Manual dengan temporary linked list

### 4. **Graph Traversal untuk Analisis Konflik**
```cpp
void analisisKonflik() {
    // DFS-like traversal untuk menemukan semua konflik
    // Menggunakan set untuk menghindari duplikasi
}
```
- **Kompleksitas**: O(V + E) dimana V = vertices, E = edges

---

## 📊 Kompleksitas dan Performa

| Operasi | Struktur Data | Kompleksitas |
|---------|---------------|--------------|
| Insert | Linked List | O(n) |
| Delete | Linked List | O(n) |
| Search by ID | Map | O(log n) |
| Search by Keyword | Linked List | O(n) |
| Conflict Detection | Graf + List | O(n) |
| Sorting | Linked List | O(n²) |
| Display All | Linked List | O(n) |

---

## 🎯 Keunggulan Sistem

### ✅ **Deteksi Konflik Otomatis**
- Mencegah double booking dosen
- Mencegah double booking ruangan  
- Validasi overlap waktu yang akurat

### ✅ **Struktur Data Efisien**
- Linked List untuk storage fleksibel
- Graf untuk relationship mapping
- Map untuk quick access
- Set untuk unique constraint

### ✅ **Search & Sort Manual**
- Implementasi algoritma tanpa library vector
- Educational value untuk pemahaman struktur data
- Custom bubble sort pada linked list

### ✅ **User Experience**
- Interface menu yang intuitif
- Error handling yang informatif
- Clear output formatting

### ✅ **Memory Management**
- Proper cleanup dengan destructor
- No memory leaks
- Dynamic allocation/deallocation

---

## 🏆 Manfaat Proyek

### 🎓 **Akademis**
- Implementasi nyata konsep struktur data
- Pemahaman Graf dan Linked List
- Practice algoritma searching & sorting

### 🏢 **Praktis**
- Solusi real-world untuk scheduling problem
- Optimasi penggunaan resource
- Mengurangi human error dalam penjadwalan

### 💻 **Technical Skills**
- C++ programming dengan standar lama
- Memory management
- Algorithm implementation
- Data structure design

---

## 📝 Contoh Use Case

### **Skenario 1: Tambah Jadwal Normal**
```
Input: Matematika Diskrit | Dr. Andi | R205 | Selasa | 10.00-11.40
Output: [SUKSES] Jadwal berhasil ditambahkan dengan ID: 1
```

### **Skenario 2: Konflik Dosen**
```
Input: Fisika Dasar | Dr. Andi | R301 | Selasa | 10.30-12.10  
Output: [KONFLIK] Dr. Andi sudah mengajar pada rentang waktu tersebut!
```

### **Skenario 3: Pencarian**
```
Input Search: "Andi"
Output: Menampilkan semua jadwal yang mengandung "Andi"
```

### **Skenario 4: Sorting**
```
Input: Pilihan 1 (Sort by Mata Kuliah)
Output: Jadwal diurutkan A-Z berdasarkan nama mata kuliah
```

---

## 👥 Kontributor

**Nama**: [Nama Mahasiswa]  
**NIM**: [NIM]  
**Mata Kuliah**: Struktur Data  
**Dosen**: [Nama Dosen]  

---

## 📄 Lisensi

Project ini dibuat untuk keperluan akademis dalam mata kuliah Struktur Data.

---

## 🔮 Future Improvements

- [ ] GUI implementation dengan Qt/GTK
- [ ] Database integration (SQLite)
- [ ] Export jadwal ke format PDF/Excel
- [ ] Multi-user access dengan login system
- [ ] Advanced conflict resolution algorithms
- [ ] REST API untuk integrasi dengan sistem lain

---

*Terima kasih telah menggunakan Sistem Manajemen Jadwal Kelas!* 🎓✨
