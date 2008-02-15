AC_DEFUN([GET_COMPILER_SPECIFIC_DEFINES],
dnl First written by Bobby Philip, 11/25/2001
dnl define GET_COMPILER_SPECIFIC_DEFINES.
dnl this macro tries to automatically
dnl determine the macros that are
dnl predefined by a specific compiler
[
# AC_LANG_SAVE
# AC_LANG_CPLUSPLUS
  AC_LANG_PUSH(C++)
  changequote(<<, >>) dnl For some reason the quotes '' below are being thrown away, so do this
# Use the selected back-end C++ compiler instead of $CXX
# case $CXX in

# DQ (12/2/2006): This macro is required even because it sets up the CXX_SPEC_DEF and C_SPEC_DEF macros in rose_config.h (generated by autoheader).
# echo "This GET COMPILER SPECIFIC DEFINES macro is no longer used, see create_system_headers file instead!"
# exit 1

# DQ (4/1/2005): Modified to get compiler name from path
  compilerName=`basename $BACKEND_CXX_COMPILER`
  echo "Using back-end C++ compiler = \"$BACKEND_CXX_COMPILER\" compiler name = \"$compilerName\" for processing of unparsed source files from ROSE preprocessors."

# case $BACKEND_CXX_COMPILER in
  case $compilerName in

  # Support for GNU gcc or g++ as a backend for compiling ROSE generated code
    g++|gcc)
             tmpFile="/tmp/tmp`uname -n`$$.C"
             echo "int main(int argc, char **argv){return 0;}" > "$tmpFile"
             macroString=`"$BACKEND_CXX_COMPILER" -v -E "$tmpFile" 2>&1 | awk '{for(i=1; i<=NF; i++){if ($i ~ /-D[^ ]/){mcStr=mcStr" " $i}}} END{ print mcStr}'`

#            macroString=$macroString" -D__builtin_va_list=void* -U__GNUG__ -U__GNUC__"
#            macroString=$macroString" -U__GNUG__ -U__GNUC__"

#            DQ (4/4/2005): Fooling aroung with these in an attempt to get g++ version 3.4.3 to work 
#            (commented out this line).  These macros (since they are specified explicitly, could have 
#            been specified in the rose-g++-headerfilefixup.h file.  Note: if this is changed then the 
#            ROSE/src/ROSETTA/Grammar/Support.code should be touched so that that part of ROSE will be 
#            recompiled!
#            DQ (4/6/2005): Fixing up ROSE to better use the linux (system) and GNU header files (which we copy and modifiy for
#            use with ROSE). These need to be specified on the command line to the EDG front-end since there use can occure in 
#            /usr/include files included before any GNU header files are included (which we have modified for use with ROSE).
#            macroString=$macroString" -U__GNUG__ -U__GNUC__ -D__extension__= -D__const=const -D__attribute__(arg)= -D__restrict= -D__inline= "
#            DQ (4/6/2005): This appears to be required for the 3.4.3 g++ compiler
#            macroString=$macroString" -D__GNUG__=3 -D__GNUC__=3 -D__GNUC_MINOR__=4 -D__GNUC_PATCHLEVEL__=3 -D_GNU_SOURCE -U__GNUG__ -U__GNUC__ -D__extension__= -D__const=const -D__attribute__(arg)= -D__restrict= -D__inline= -D_GLIBCXX_HAVE_STDINT_H "
#            DQ (4/7/2005): Added additional macros to turn off processing of gnu lib "extern template" mechanism
#            previously in the g++ 3.3.2 header files it was sufficent to edit "extern template" to generate "template" but this
#            is insufficient for the g++ 3.4 header files so we have defined _GLIBCXX_EXTERN_TEMPLATE=0 to turn off processing of
#            such parts of the g++ 3.4 headers.
#            macroString=$macroString" -D__GNUG__=3 -D__GNUC__=3 -D__GNUC_MINOR__=4 -D__GNUC_PATCHLEVEL__=3 -D_GNU_SOURCE -U__GNUG__ -U__GNUC__ -U__GNUG__ -U__GNUC__ -D__extension__= -D__const=const -D__attribute__(arg)= -D__restrict= -D__inline= -D_GLIBCXX_HAVE_STDINT_H -D_GLIBCXX_EXTERN_TEMPLATE=0"

             BACKEND_GCC_MAJOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f1`
             BACKEND_GCC_MINOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f2`
             BACKEND_GCC_PATCHLEVEL=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f3`

#            Once we get the EDG to emulate the correct version of g++ in the header files __null must be defined
#            macroString=$macroString" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL -D_GNU_SOURCE -D__extension__= -D__const=const -D__attribute__(arg)= -D__restrict= -D__inline= -D_GLIBCXX_HAVE_STDINT_H -D_GLIBCXX_EXTERN_TEMPLATE=0 -D__null=0 -D__builtin_expect(x,y)=(x) -D__PRETTY_FUNCTION__=0 "
# EDG messgae: Command-line error: language modes specified are incompatible
#             macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL -D_GNU_SOURCE --gcc --g++ -D__extension__= -D__const=const -D__attribute__(arg)= -D__restrict= -D__inline= -D_GLIBCXX_HAVE_STDINT_H -D_GLIBCXX_EXTERN_TEMPLATE=0 -D__null=0 -D__builtin_expect(x,y)=(x) -D__PRETTY_FUNCTION__=0 "
#            macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL -D_GNU_SOURCE --include_directory /usr/include --include_directory /usr/include/linux -D__extension__= -D__const=const -D__attribute__(arg)= -D__restrict= -D__inline= -D_GLIBCXX_HAVE_STDINT_H -D_GLIBCXX_EXTERN_TEMPLATE=0 -D__null=0 -D__builtin_expect(x,y)=(x) -D__PRETTY_FUNCTION__=0 "
#            macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL -D_GNU_SOURCE --preinclude rose_edg_macros_and_functions_required_for_gnu.h -D__extension__= -D__const=const -D__attribute__(arg)= -D__restrict= -D__inline= -D_GLIBCXX_HAVE_STDINT_H -D_GLIBCXX_EXTERN_TEMPLATE=0 -D__null=0 -D__builtin_expect(x,y)=(x) -D__PRETTY_FUNCTION__=0 "

#            macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL -D_GNU_SOURCE --preinclude rose_edg_macros_and_functions_required_for_gnu.h "
#            macroString=" -DQUINLAN --preinclude rose_edg_macros_and_functions_required_for_gnu.h "
#            macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL --preinclude rose_edg_macros_and_functions_required_for_gnu.h "
             macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL --preinclude rose_edg_required_macros_and_functions.h "

             rm "$tmpFile" 2>/dev/null;;

  # Support for Intel icc as a backend for compiling ROSE generated code
    icpc|icc)

             tmpFile="/tmp/tmpICCMacroExtraction`uname -n`$$.C"
             echo "int main(int argc, char **argv){return 0;}" > "$tmpFile"
             #ICC_MACRO_DEFS=`icpc -# "$tmpFile" 2>&1 | grep "\-D" | sed "s/.$//" | sed "s/"/\\"/" | paste -s`
             #ICC_MACRO_DEFS=`icpc -# "$tmpFile" 2>&1 | grep "\-D" | sed "s/.$//" | tr -s ' ' ' ' | paste -s`
             #ICC_MACRO_DEFS=`icpc -# "$tmpFile" 2>&1 | grep "\-D" | sed "s/.$//" |grep -v "\"" |tr -s ' ' ' ' | paste -s`
             #ICC_MACRO_DEFS=`icpc -# "$tmpFile" 2>&1 | grep "\-D" | grep -v 'i386' | grep -v 'linux' | grep -v 'unix' | grep -v 'GXX' | grep -v 'NO_STRING_INLINES' | grep -v "INLINE" | grep -v "ELF" | grep -v "__extension" | grep -v '__USER_LABEL_PREFIX__' | grep -v 'REGISTER_PREFIX'| grep -v "OPTIMIZE" | grep -v '__EXCEPTIONS' | sed "s/.$//" |grep -v "\""| paste -s  |tr -s ' ' ' '  `
             #macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL -D_GNU_SOURCE --preinclude rose_edg_macros_and_functions_required_for_gnu.h "
             #macroString="-D__ICC=900 -D__INTEL_COMPILER=900 -D__INTEL_COMPILER_BUILD_DATE=20050430 -D__GNUC__=3 -D__GNUC_MINOR__=3 -D__GNUC_PATCHLEVEL__=2 -D_GNU_SOURCE=1 -D__GNUG__=3 -D__PTRDIFF_TYPE__=int -D__SIZE_TYPE__=unsignedd \"-D__WCHAR_TYPE__=long int\" \"-D__WINT_TYPE__=unsigned int\" --preinclude rose_edg_macros_and_functions_required_for_gnu.h "
             #macroString=" --preinclude rose_edg_macros_and_functions_required_for_icc.h "

           ##             extra_icc_defines=`"icpc" -# "$tmpFile" 2>&1 |  sed "s/.$//" | grep "\-D" | grep "GNUG\|__GNUC_PATCHLEVEL__\|__GNUC_MINOR__\|__GNUC__" `
#             extra_icc_defines=`"icpc" -# "$tmpFile" 2>&1 | | grep "\-D" | grep "GNUG\|__GNUC_PATCHLEVEL__\|__GNUC_MINOR__\|__GNUC__" |sed ':a; /\\$/N; s/\\\n//; ta'sed ':a;/\\$/N; s/\\\n//; ta' `
           # extra_icc_defines=`"icpc" -# "test.C" 2>&1 |  grep "\-D" | grep "GNUG\|__GNUC_PATCHLEVEL__\|__GNUC_MINOR__\|__GNUC__"  |  sed "s/.$//" | sed 's/\\\n//'   `
           # echo $extra_icc_defines
             extra_icc_defines=`"icpc" -# "test.C" 2>&1 |  grep "\-D" | grep "GNUG\|__GNUC_PATCHLEVEL__\|__GNUC_MINOR__\|__GNUC__"  | sed ':a; /\\$/N; s/\\\n//; ta' | sed 's/\\\//' `
             tmp_macro=""
             for macro_i in $extra_icc_defines
                 do
                    echo $macro_i
                    tmp_macro=" $macro_i $tmp_macro"
                    echo " tmp_macro  $tmp_macro"
                 done
           # macroString=" -D__PURE_INTEL_C99_HEADERS__ ${tmp_macro} --preinclude rose_edg_macros_and_functions_required_for_icc.h "
             macroString=" -D__PURE_INTEL_C99_HEADERS__ ${tmp_macro} --preinclude rose_edg_required_macros_and_functions.h "
              echo "ICC MACRO DEFS: $ICC_MACRO_DEFS"
             echo "macroString: $macroString"

             #BACKEND_GCC_MAJOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f1`
             #BACKEND_GCC_MINOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f2`
             #BACKEND_GCC_PATCHLEVEL=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f3` 
             ;;

  # Support for ROSE "roseTranslator" as a backend for compiling ROSE generated code
    roseTranslator)
           # macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL -D_GNU_SOURCE --preinclude rose_edg_macros_and_functions_required_for_gnu.h "
             macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL -D_GNU_SOURCE --preinclude rose_edg_required_macros_and_functions.h "
             BACKEND_GCC_MAJOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f1`
             BACKEND_GCC_MINOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f2`
             BACKEND_GCC_PATCHLEVEL=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f3` ;;

  # Support for KAI KCC as a backend for compiling ROSE generated code (KAI was bought by Intel and KCC was discontinued)
    KCC)
             macroString=`"$BACKEND_CXX_COMPILER" -v 2>&1 | awk '{for(i=1; i<=NF; i++){if ($i ~ /^-D[^ ]/){mcStr=mcStr" " $i}}} END{ print mcStr}'`;;

  # Support for SUN CC as a backend for compiling ROSE generated code
  # for CC we assume for now that the hardware is sparc, later we can deal with x86 if need be
  # I am also not treating the compat=4,5 issue correctly at this time
  # CC unfortunately gives no cute option to output a list of predefined macros, so
  # we resort to explicitly specifying almost all of them
  # -D__DATE__ -D__FILE__ -D__LINE__ -D__TIME__ -D__SUNPRO_CC=0x500 were originally in there but we remove them for now
    CC)
             macroString="-D__BUILTIN_VA_ARG_INCR -D__cplusplus -D__STDC__=0 -D__SVR4"
             macroString=$macroString" -D__sun -Dsun -D__`uname -s`_`uname -r | sed 'y/-./__/'`"
             macroString=$macroString" -D__unix -Dunix -D_WCHAR_T -D__ARRAYNEW -D__sparc -Dsparc";;

    *)
             echo "we reached here for some reason (cannot identify back-end C++ compiler \"$BACKEND_CXX_COMPILER\")";
             exit 1;
             macroString="";;
  esac

  changequote([, ])
  AC_DEFINE_UNQUOTED([CXX_SPEC_DEF],"$macroString",[-D options to hand to EDG C++ front-end.])

# This is now setup in a separate macro and can be specified on the command line
# AC_DEFINE_UNQUOTED(CXX_COMPILER_NAME, "$CXX")

# This string has all compiler specific predefined macros listed
  echo "Backend compiler specific macroString = $macroString"

# AC_LANG_RESTORE
  AC_LANG_POP(C++)
])












