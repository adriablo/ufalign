#pragma once

int printAlignment(char *buffer, int bufSize,
                   const char* query, const char* target,
                   const EdlibAlignResult &ear, const EdlibAlignMode modeCode
                   );
