#include <iostream>
#include "ОССП_Лабораторная_работа_10_HT.h"
using namespace std;
using namespace HT;

#define ADDRESS_1 "D:\\лабораторные работы\\сп\\HT_Space.ht"
#define PASSWORD_1 "key222"

int main() {
	
    HT::HTHANDLE* ht = nullptr;
    HT::HTHANDLE* ht2 = nullptr;

    try
    {
        ht = Create(1000, 3, 10, 256, ADDRESS_1);
        if (ht)
            cout << "Create: success" << endl;
        else throw "Create: error";

        if (Insert(ht, new Element(PASSWORD_1, 7, "payload", 8)))
            cout << "Insert:success" << endl;
        else throw "Insert:error";

        Element* hte = Get(ht, new Element(PASSWORD_1, 7));

        if (hte)
            cout << "Get: success" << endl;
        else throw "Get: error";

        print(hte);

        if (Snap(ht))
            cout << "Snap: success" << endl;
        else throw "Snap: error";

        if (Update(ht, hte, "newpayload", 11))
            cout << "Update:success" << endl;
        else throw "Update:error";

        Element* hte1 = Get(ht, new Element(PASSWORD_1, 7));
        if (hte1)
            cout << "Get: success" << endl;
        else throw "Get: error";

        print(hte1);

        if (Delete(ht, hte1)) cout << "Delete: success\n";
        else throw "Delete: error";

        if (Close(ht))
            cout << "Close: success\n";
        else throw "Close: error";

    }
    catch (const char* msg)
    {
        cout << msg << endl;
        if (ht != nullptr) cout << GetLastError(ht) << endl;
    }
    //2
    try
    {
        ht2 = Open(ADDRESS_1);
        if (ht2)
            cout << "Open: success" << endl;
        else throw " Open: error";

        if (Insert(ht, new Element(PASSWORD_1, 7, "payload", 8)))
            cout << "Insert: success\n";
        else throw "Insert: error";

        Element* hte = Get(ht2, new Element(PASSWORD_1, 7));
        if (hte)
            cout << "Get: success" << endl;
        else throw "Get: error";

        if (Close(ht2))
            cout << "Close: success" << endl;
        else throw "Close: error";

    }
    catch (const char* msg)
    {
        cout << msg << endl;
        if (ht2 != nullptr)
            cout << GetLastError(ht2) << endl;
    }

    return 0;

}