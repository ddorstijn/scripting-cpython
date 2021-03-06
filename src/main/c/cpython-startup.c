/*
 * #%L
 * JSR-223-compliant Python scripting language plugin linking to CPython
 * %%
 * Copyright (C) 2014 Board of Regents of the University of
 * Wisconsin-Madison, Broad Institute of MIT and Harvard, and Max Planck
 * Institute of Molecular Cell Biology and Genetics.
 * %%
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * #L%
 */

#include "org_scijava_plugins_scripting_cpython_CPythonStartup.h"
#include <python.h>

JNIEXPORT void JNICALL Java_org_scijava_plugins_scripting_cpython_CPythonStartup_initializePythonThread(JNIEnv *env, jclass clazz, jstring pythonCode)
{
	PyGILState_STATE state;
	const char *python_code;
	PyObject *err;

	Py_Initialize();
	state = PyGILState_Ensure();
	python_code = (*env)->GetStringUTFChars(env, pythonCode, NULL);
	PyRun_SimpleString(python_code);
        (*env)->ReleaseStringUTFChars(env, pythonCode, python_code);
	err = PyErr_Occurred();
	if (err) {
		PyErr_Print();
		PyErr_Clear();
	}
	PyGILState_Release(state);
	if (err) {
		(*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/RuntimeException"), "Python script raised an exception");
	}

	/*
	 * We cannot really call Py_Finalize(); here: multiple SciJava contexts
	 * can have their individual CPythonScriptLanguage instances, so we
	 * cannot call it after tearing down the context, either, so we have to
	 * introduce a reference counter in the C library (TODO).
	 */
}
