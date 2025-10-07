#include "Header.h"
#include <Windows.h>
#include <iostream>
int main()
{
    HT::HTHANDLE* ht = nullptr;
    try
    {
        ht = HT::Create(1000, 3, 10, 256, "test.ht", 0);  // создать HT-хранилище
        if (ht) std::cout << "\n-- Create: success";
        else throw " -- Create: error";

        if (HT::Insert(ht, new HT::Element("key222", 7, "payload", 8))) std::cout << "\n-- Insert:success\n";
        else throw "-- insert:error";

        if (HT::Insert(ht, new HT::Element("key223", 7, "data123", 8))) std::cout << "\n-- Insert:success\n";
        else throw "-- insert:error";

        HT::Element* hte = HT::Get(ht, new HT::Element("key222", 7));
        if (hte) std::cout << "\nGet: success\n";
        else throw "\n-- Get: error\n";

        HT::print(hte);

        if (HT::Snap(ht)) std::cout << "\nSnap: success";
        else throw "\n-- Snap: error";

        if (HT::Update(ht, hte, "newpayload", 11)) std::cout << "\n-- Update:success\n";
        else throw "-- Update:error";

        HT::Element* hte1 = HT::Get(ht, new HT::Element("key222", 7));
        if (hte1) std::cout << "\nGet: success\n";
        else throw "-- Get: error\n";

        HT::print(hte1);

        if (HT::Delete(ht, hte1)) std::cout << "\nDelete: success\n";
        else throw "-- Delete: error";

        if (HT::Close(ht)) std::cout << "\nClose: success\n";
        else throw "\n-- Close: error";

        if (HT::Open("test.ht")) std::cout << "\nOpen success\n";
        else throw "\n open error";

        hte1 = HT::Get(ht, new HT::Element("key223", 7));
        if (hte1) std::cout << "\nGet: success\n";
        else throw "-- Get: error\n";
        HT::print(hte1);

    }
    catch (char* msg)
    {
        std::cout << msg << "\n";
        //if (ht != nullptr) std::cout << HT::GetLastError(ht);
    }
}