
void CopyParm(int len, char *param);
#ifdef VELMAN_UNIX
int FortranRead(ifstream &file, char *data, int size);
void FortranWrite(ofstream &file, char *data, int size);
#else
int FortranRead(std::ifstream &file, char *data, int size);
void FortranWrite(std::ofstream &file, char *data, int size);
#endif
void zero_mem(char *ptr, int length);
void UnPad(char *string, int length);
void Pad(char *string, int length);
void InvertData(char *data, int section[]);
