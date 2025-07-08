
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


