#define	PY_SSIZE_T_CLEAN
#include <Python.h>
int random_python(int argc, char *argv[])
{
	wchar_t	*program = Py_DecodeLocale(argv[0],	NULL);
	if	(program == NULL)	{
		fprintf(stderr,	"Fatal	error:	cannot	decode	argv[0]\n");
		exit(1);
	}
	Py_SetProgramName(program);	
	Py_Initialize();
	PyRun_SimpleString("import random\n" 
		"print('Python random number generator!')\n"
		"print('A random number between 0 and 100 is '+str(random.randint(0, 100)))\n");
	if	(Py_FinalizeEx() < 0) {
		exit(120);
	}
	PyMem_RawFree(program);
	return	0;
}