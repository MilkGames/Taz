#include <babel.h>

#if defined _DEBUG || defined BASSERTIONS

int bError = 0;
int bBreakDisable = 0;
int bShowMultiple = 0;
int bMultiple = 0;

void bkAssert(int erro, int test, char* format, ...){
	static char strbuf[0x200] = {0};
	va_list args;
	if(test == 0){
		if(bShowMultiple == 0){
			if((bError == erro) && (erro != -1)){
				if(bMultiple != 0){
					return;
				}
				bMultiple = 1;
				bkPrintf("^-- multiple asserts of last type\n");
				return;
			}
			bMultiple = 0;
		}
		va_start(args, format);
		vsprintf(strbuf, format, args);
		va_end(args);
		bkPrintf("[ASSERT FAILED]");
		bkPrintf(strbuf);
		bkPrintf("\n");

		if(bBreakDisable == 0) {
			DebugBreak();
		}
		bError = erro;
	}

}

#endif