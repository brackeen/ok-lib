#define OK_LIB_DEFINE
#include "ok_map.h"
#include <iostream>

/*
 Example using the C++ wrapper for ok_lib's hashmap.
 */
int main() {
    std::cout << "Hello from C++!" << std::endl;

    //ok::map<std::string, std::string> map; // Disabled for now, failing in older versions of gcc/clang.
    ok::map<const char *, const char *> map;

    // Put method
    map.put("dave", "bananas");
    map.put("mary", "grapes");

    // Subscript operator
    map["sue"] = "beans";
    map["john"] = "fries";

    // Overwrite existing value
    map["john"] = "salad";

    // When the mapping exists, subscript[] and get() are the same.
    std::cout << "john: " << map["john"] << "." << std::endl;
    std::cout << "mary: " << map.get("mary") << "." << std::endl;

    // Overwrite again, via the pointer
    auto *p = map.get_ptr("john");
    *p = "fries";
    std::cout << "john: no, wait, " << map.get("john") << "!" << std::endl;

    // Examples when key doesn't exist
    std::cout << "Map size: " << map.size() << std::endl;
    map.get("cyrus"); // Returns NULL
    std::cout << "Map size before map[\"cyrus\"]: " << map.size() << std::endl;
    map["cyrus"]; // Mapping created - but value undefined!
    std::cout << "Map size after map[\"cyrus\"]: " << map.size() << std::endl;
    map["cyrus"] = "(who knows)"; // Define the value so we don't crash

    // Iterator
    for (auto& pair : map) {
        std::cout << "> " << pair.key << " wants " << pair.value << "." << std::endl;
    }

    return 0;
}
