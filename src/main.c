#include "core.h"
#include "options.h"

int main(int argc, char const *argv[])
{
    parse_options(argc, argv);
    vectorize_image();
    return 0;
}
