#include "vura-helpers.h"


void VuraHelpers::simulateApplicationCrash()
{
    int *ptr = nullptr;
    *ptr = 42; // Accessing address 0 causes a SIGSEGV/Access Violation
}
