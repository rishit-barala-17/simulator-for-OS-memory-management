#include "C:/Users/asus/Documents/Rishit IITR Academics/ACM Project/src/04_vm/virtual_memory.h"
#include <iostream>

using namespace std;

int main()
{
    VirtualMemory vm(8, 3, 100);

    cout << "\n--- Sequential accesses (hits + faults) ---\n";
    vm.touch(10);
    vm.touch(120);
    vm.touch(250);
    vm.touch(30);
    vm.touch(180);

    cout << "\n--- FIFO eviction check ---\n";
    vm.touch(350);
    vm.touch(120);
    vm.touch(10);

    cout << "\n--- Invalid address test ---\n";
    vm.touch(900);
    vm.touch(-5);

    cout << "\n--- Edge case validation ---\n";
    vm.touch(100);
    vm.touch(799);
    vm.touch(101);
    vm.touch(102);

    cout << "\n--- Eviction stress test ---\n";
    vm.touch(400);
    vm.touch(500);
    vm.touch(600);

    cout << "\n--- translate() API test ---\n";
    bool fault;
    int phys;

    phys = vm.resolve(250, fault);
    cout << "translate(250) -> " << phys << " | fault=" << fault << endl;

    phys = vm.resolve(450, fault);
    cout << "translate(450) -> " << phys << " | fault=" << fault << endl;

    phys = vm.resolve(120, fault);
    cout << "translate(120) -> " << phys << " | fault=" << fault << endl;

    cout << "\n--- Final statistics ---\n";
    vm.printStats();

    return 0;
}
