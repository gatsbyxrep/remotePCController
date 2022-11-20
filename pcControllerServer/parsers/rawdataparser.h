#ifndef RAWDATAPARSER_H
#define RAWDATAPARSER_H

void RAWDataParser_parse(const char* data) {
   switch(data[0]) {
        case 0: break; // Mouse command
        case 1: break; // Keyboard controll
    }
}

#endif // RAWDATAPARSER_H
