#include "Header.h"
#include <Windows.h>
#include <iostream>
int main()
{
    HT::HTHANDLE* ht = nullptr;
    HT::HTHANDLE* ht1 = nullptr;

    try
    {
        ht = HT::Create(1000, 3, 10, 256, "test3.ht", 0); 
        if (ht) std::cout << "-- Create: success";
        else throw " -- Create: error";

        if (HT::Close(ht)) std::cout << "Close: success";
        else throw "-- Close: error";

        ht = HT::Open("test3.ht");
        if (ht) std::cout << "\nopen ht success\n";
        else throw " open ht error";

        if (HT::Insert(ht, new HT::Element("key222", 7, "payload", 8))) std::cout << "-- Insert:success";
        else throw "-- insert:error";

        HT::Element* hte = HT::Get(ht, new HT::Element("key222", 7));
        if (hte) std::cout << "Get: success";
        else throw "-- Get: error";

        HT::print(hte);

        if (HT::Snap(ht)) std::cout << "Snap: success";
        else throw "-- Snap: error";

        if (HT::Update(ht, hte, "newpayload", 11)) std::cout << "-- Update:success";
        else throw "-- Update:error";

        HT::Element* hte1 = HT::Get(ht, new HT::Element("key222", 7));
        if (hte1) std::cout << "Get: success";
        else throw "-- Get: error";

        HT::print(hte1);

        if (HT::Delete(ht, hte1)) std::cout << "Delete: success";
        else throw "-- Delete: error";


        ht1 = HT::Open("test3.ht");
        if (ht1) std::cout << "\nopen ht1 success\n";
        else throw " open ht1 error";

        if (HT::Insert(ht, new HT::Element("key224", 7, "data122", 8))) std::cout << "-- Insert ht:success";
        else throw "-- insert:error";

        if (HT::Insert(ht1, new HT::Element("keyht1", 7, "data123", 8))) std::cout << "-- Insert ht1:success";
        else throw "-- insert:error";

        hte = HT::Get(ht1, new HT::Element("keyht1", 7));
        if (hte) std::cout << "Getht1: success";
        else throw "-- Getht1: error";

        hte = HT::Get(ht, new HT::Element("key224", 7));
        if (hte) std::cout << "Get: success";
        else throw "-- Get: error";

        

        if (HT::Close(ht)) std::cout << "Close: success";
        else throw "-- Close: error";
        if (HT::Close(ht1)) std::cout << "Close: success";
        else throw "-- Close: error";


    }
    catch (char* msg)
    {
        std::cout << msg << "\n";
        //if (ht != nullptr) std::cout << HT::GetLastError(ht);
    }
}