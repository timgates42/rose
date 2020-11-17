
#include <iostream>

#include "sage3basic.h"
#include "clang-to-dot-private.hpp"

#include "rose_config.h"

#if 0
// DQ (4/5/2017): nothing works since we need the version of Clang/LLVM that we are using to be3 compilied without the "-fno-rtti" option.
// DQ (3/1/2017): Trying to get rid of linker error.
#ifdef ROSE_USE_CLANG_FRONTEND
#include "typeinfo"
// clang::ASTConsumer::typeinfo() {};
// clang::PPCallbacks::typeinfo() {};
// extern "C" {
#if 0
namespace clang {
  namespace ASTConsumer {
     typeinfo() {};
  }
  namespace PPCallbacks {
     typeinfo() {};
  }
}
#endif

void clang::ASTConsumer::type_info() {};
void clang::PPCallbacks::type_info() {};

// }
#endif
#endif


extern bool roseInstallPrefix(std::string&);

// DQ (11/1/2020): Added to resolve types (e.g string)
using namespace std;

// File for output for generated graph.
std::ofstream CLANG_ROSE_Graph::file;

// DQ (11/1/2020): Added to resolve namespace variables.
using namespace CLANG_ROSE_Graph;

// DQ (10/23/2020): We need a list to make sure that we don't add nodes twice (once when traversing the source sequence lists and a second time when traversing the AST).
// We need to show the AST plus the edges that are specific to the source sequence lists (and which node's source sequence list they are associated with).
std::set<void*> graphNodeSet;

#define DEBUG_CLANG_DOT_GRAPH_SUPPPORT 0

#define DEBUG_HEADER_GRAPH_SUPPPORT 0


// int clang_to_dot_main(int argc, char ** argv, SgSourceFile& sageFile) 
int clang_to_dot_main(int argc, char ** argv) 
   {
  // DQ (9/6/2013): Build a dot graph of the EDG AST.

  // Build filename...
     string filename = "clangGraph";

     string dot_header = filename;
     filename += ".dot";

#if DEBUG_EDG_DOT_GRAPH_SUPPPORT
     printf ("In clang_to_dot_main(): filename = %s \n",filename.c_str());
#endif

  // std::set<void*> graphNodeSet;
     if (graphNodeSet.empty() == false)
        {
#if 1
          printf ("In graph_edg_ast(): calling graphNodeSet.clear() to support multiple files \n");
#endif
          graphNodeSet.clear();
        }

  // Open file...(file is declared in the EDG_ROSE_Graph namespace).
     file.open(filename.c_str());

  // Output the opening header for a DOT file.
     file << "digraph \"" << dot_header << "\" {" << endl;

#if 0
  // DQ (11/1/2020): This is not defined for Clang (only EDG).

  // Calling recursive function to build dot file for scope, starting at the top level scope.
     graph (il_header.primary_scope,"EDG Global Scope");

  // Function "main" is stored seperately.
     if (il_header.main_routine != NULL)
        {
          graph (il_header.main_routine);
        }
#endif


  // 0 - Analyse Cmd Line

    std::vector<std::string> inc_dirs_list;
    std::vector<std::string> define_list;
    std::vector<std::string> inc_list;
    std::string input_file;

    for (int i = 0; i < argc; i++) {
        std::string current_arg(argv[i]);
        if (current_arg.find("-I") == 0) {
            if (current_arg.length() > 2) {
                inc_dirs_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argc)
                    inc_dirs_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-D") == 0) {
            if (current_arg.length() > 2) {
                define_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argc)
                    define_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-c") == 0) {}
        else if (current_arg.find("-o") == 0) {
            if (current_arg.length() == 2) {
                i++;
                if (i >= argc) break;
            }
        }
        else {
            // TODO -include
#if DEBUG_ARGS
            std::cerr << "argv[" << i << "] = " << current_arg << " is neither define or include dir. Use it as input file."  << std::endl;
#endif
            input_file = current_arg;
        }
    }

    ClangToDotTranslator::Language language = ClangToDotTranslator::unknown;

    size_t last_period = input_file.find_last_of(".");
    std::string extention(input_file.substr(last_period + 1));

    if (extention == "c") {
        language = ClangToDotTranslator::C;
    }
    else if (extention == "C" || extention == "cxx" || extention == "cpp" || extention == "cc") {
        language = ClangToDotTranslator::CPLUSPLUS;
    }
    else if (extention == "objc") {
        language = ClangToDotTranslator::OBJC;
    }
    else if (extention == "cu") {
        language = ClangToDotTranslator::CUDA;
    }
    else if (extention == "ocl" || extention == "cl") {
        language = ClangToDotTranslator::OPENCL;
    }

    ROSE_ASSERT(language != ClangToDotTranslator::unknown);

    const char * cxx_config_include_dirs_array [] = CXX_INCLUDE_STRING;
    const char * c_config_include_dirs_array   [] = C_INCLUDE_STRING;

    std::vector<std::string> cxx_config_include_dirs (
                                                       cxx_config_include_dirs_array,
                                                       cxx_config_include_dirs_array + sizeof(cxx_config_include_dirs_array) / sizeof(const char*)
                                                     );
    std::vector<std::string> c_config_include_dirs   (
                                                       c_config_include_dirs_array,
                                                       c_config_include_dirs_array + sizeof(c_config_include_dirs_array) / sizeof(const char*)
                                                     );

    std::string rose_include_path;
    bool in_install_tree = roseInstallPrefix(rose_include_path);
    if (in_install_tree) {
        rose_include_path += "/include/";
    }
    else {
        rose_include_path = std::string(ROSE_AUTOMAKE_TOP_BUILDDIR) + "/include-staging/";
    }

    std::vector<std::string>::iterator it;
    for (it = c_config_include_dirs.begin(); it != c_config_include_dirs.end(); it++)
        if (it->length() > 0 && (*it)[0] != '/')
            *it = rose_include_path + *it;
    for (it = cxx_config_include_dirs.begin(); it != cxx_config_include_dirs.end(); it++)
        if (it->length() > 0 && (*it)[0] != '/')
            *it = rose_include_path + *it;

    inc_dirs_list.push_back(rose_include_path + "clang/");


    // FIXME add ROSE path to gcc headers...
    switch (language) {
        case ClangToDotTranslator::C:
            inc_dirs_list.insert(inc_dirs_list.begin(), c_config_include_dirs.begin(), c_config_include_dirs.end());
            inc_list.push_back("clang-builtin-c.h");
            break;
        case ClangToDotTranslator::CPLUSPLUS:
            inc_dirs_list.insert(inc_dirs_list.begin(), cxx_config_include_dirs.begin(), cxx_config_include_dirs.end());
            inc_list.push_back("clang-builtin-cpp.hpp");
            break;
        case ClangToDotTranslator::CUDA:
            inc_dirs_list.insert(inc_dirs_list.begin(), cxx_config_include_dirs.begin(), cxx_config_include_dirs.end());
            inc_list.push_back("clang-builtin-cuda.hpp");
            break;
        case ClangToDotTranslator::OPENCL:
//          inc_dirs_list.insert(inc_dirs_list.begin(), c_config_include_dirs.begin(), c_config_include_dirs.end());
            // FIXME get the path right
            inc_list.push_back("clang-builtin-opencl.h");
            break;
        case ClangToDotTranslator::OBJC:
          {
         // DQ (10/23/2020): Added error message for Objective C language not supported in ROSE.
            printf ("Objective C langauge support is not available in ROSE \n");
            ROSE_ASSERT(false);
          }
        default:
          {
            printf ("Default reached in switch(language) support \n");
            ROSE_ASSERT(false);
          }
    }

    // FIXME should be handle by Clang ?
    define_list.push_back("__I__=_Complex_I");

    unsigned cnt = define_list.size() + inc_dirs_list.size() + inc_list.size();
    char ** args = new char*[cnt];
    std::vector<std::string>::iterator it_str;
    unsigned i = 0;
    for (it_str = define_list.begin(); it_str != define_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i][0] = '-';
        args[i][1] = 'D';
        strcpy(&(args[i][2]), it_str->c_str());
#if DEBUG_ARGS
        std::cerr << "args[" << i << "] = " << args[i] << std::endl;
#endif
        i++;
    }
    for (it_str = inc_dirs_list.begin(); it_str != inc_dirs_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i][0] = '-';
        args[i][1] = 'I';
        strcpy(&(args[i][2]), it_str->c_str());
#if DEBUG_ARGS
        std::cerr << "args[" << i << "] = " << args[i] << std::endl;
#endif
        i++;
    }
    for (it_str = inc_list.begin(); it_str != inc_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 9];
        args[i][0] = '-'; args[i][1] = 'i'; args[i][2] = 'n'; args[i][3] = 'c';
        args[i][4] = 'l'; args[i][5] = 'u'; args[i][6] = 'd'; args[i][7] = 'e';
        strcpy(&(args[i][8]), it_str->c_str());
#if DEBUG_ARGS
        std::cerr << "args[" << i << "] = " << args[i] << std::endl;
#endif
        i++;
    }


  // 2 - Create a compiler instance

    clang::CompilerInstance * compiler_instance = new clang::CompilerInstance();

    llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> DiagOpts = new clang::DiagnosticOptions();
    clang::TextDiagnosticPrinter * diag_printer = new clang::TextDiagnosticPrinter(llvm::errs(), &*DiagOpts);
    compiler_instance->createDiagnostics(diag_printer, true);

    clang::CompilerInvocation * invocation = new clang::CompilerInvocation();
    std::shared_ptr<clang::CompilerInvocation> invocation_shptr(std::move(invocation));
    llvm::ArrayRef<const char *> argsArrayRef(args, &(args[cnt]));
    clang::CompilerInvocation::CreateFromArgs(*invocation, argsArrayRef, compiler_instance->getDiagnostics());
    compiler_instance->setInvocation(invocation_shptr);

    clang::LangOptions & lang_opts = compiler_instance->getLangOpts();

    switch (language) {
        case ClangToDotTranslator::C:
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_C, );
            break;
        case ClangToDotTranslator::CPLUSPLUS:
            lang_opts.CPlusPlus = 1;
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_CXX, );
            break;
        case ClangToDotTranslator::CUDA:
            lang_opts.CUDA = 1;
//          lang_opts.CPlusPlus = 1;
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_CUDA,   clang::LangStandard::lang_cuda);
            break;
        case ClangToDotTranslator::OPENCL:
            lang_opts.OpenCL = 1;
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_OpenCL, clang::LangStandard::lang_opencl);
            break;
        case ClangToDotTranslator::OBJC:
            ROSE_ASSERT(!"Objective-C is not supported by ROSE Compiler.");
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_, );
        default:
            ROSE_ASSERT(false);
    }

    clang::TargetOptions target_options;
    target_options.Triple = llvm::sys::getDefaultTargetTriple();
    std::shared_ptr<clang::TargetOptions> targetOption_shptr = std::make_shared<clang::TargetOptions>(target_options);
    clang::TargetInfo * target_info = clang::TargetInfo::CreateTargetInfo(compiler_instance->getDiagnostics(), targetOption_shptr);
    compiler_instance->setTarget(target_info);

    compiler_instance->createFileManager();
    compiler_instance->createSourceManager(compiler_instance->getFileManager());

    llvm::ErrorOr<const clang::FileEntry *> ret  = compiler_instance->getFileManager().getFile(input_file);
    const clang::FileEntry * input_file_entry = ret.get(); 
    clang::FileID mainFileID = compiler_instance->getSourceManager().createFileID(input_file_entry, clang::SourceLocation(), compiler_instance->getSourceManager().getFileCharacteristic(clang::SourceLocation()));

    compiler_instance->getSourceManager().setMainFileID(mainFileID);

    if (!compiler_instance->hasPreprocessor()) compiler_instance->createPreprocessor(clang::TU_Complete);

    if (!compiler_instance->hasASTContext()) compiler_instance->createASTContext();

    ClangToDotTranslator translator(compiler_instance, language);
    compiler_instance->setASTConsumer(std::move(std::unique_ptr<clang::ASTConsumer>(&translator)));

    if (!compiler_instance->hasSema()) compiler_instance->createSema(clang::TU_Complete, NULL);

    ROSE_ASSERT (compiler_instance->hasDiagnostics());
    ROSE_ASSERT (compiler_instance->hasTarget());
    ROSE_ASSERT (compiler_instance->hasFileManager());
    ROSE_ASSERT (compiler_instance->hasSourceManager());
    ROSE_ASSERT (compiler_instance->hasPreprocessor());
    ROSE_ASSERT (compiler_instance->hasASTContext());
    ROSE_ASSERT (compiler_instance->hasSema());

  // 3 - Translate

    compiler_instance->getDiagnosticClient().BeginSourceFile(compiler_instance->getLangOpts(), &(compiler_instance->getPreprocessor()));
    clang::ParseAST(compiler_instance->getPreprocessor(), &translator, compiler_instance->getASTContext());
    compiler_instance->getDiagnosticClient().EndSourceFile();

#if 0
    SgGlobal * global_scope = translator.getGlobalScope();

#if 0
  // 4 - Attach to the file

    if (sageFile.get_globalScope() != NULL) SageInterface::deleteAST(sageFile.get_globalScope());

    sageFile.set_globalScope(global_scope);

    global_scope->set_parent(&sageFile);
#endif

    std::string file_name(input_file);

    Sg_File_Info * start_fi = new Sg_File_Info(file_name, 0, 0);
    Sg_File_Info * end_fi   = new Sg_File_Info(file_name, 0, 0);

    global_scope->set_startOfConstruct(start_fi);

    global_scope->set_endOfConstruct(end_fi);
#endif

  // 5 - Finish the AST (fixup phase)

    printf ("Calling finishSageAST(): needs to call DOT graph generator \n");

 // finishSageAST(translator);

  // DOT specific code to close off the file.
  // Close off the DOT file.
     file << endl;
     file << "} " << endl;
     file.close();

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

    return 0;
}

#if 0
// We don't need this for for the generation of the dot file.
void finishSageAST(ClangToDotTranslator & translator) {
    SgGlobal * global_scope = translator.getGlobalScope();

 // 1 - Label Statements: Move sub-statement after the label statement.

    std::vector<SgLabelStatement *> label_stmts = SageInterface::querySubTree<SgLabelStatement>(global_scope, V_SgLabelStatement);
    std::vector<SgLabelStatement *>::iterator label_stmt_it;
    for (label_stmt_it = label_stmts.begin(); label_stmt_it != label_stmts.end(); label_stmt_it++) {
        SgStatement * sub_stmt = (*label_stmt_it)->get_statement();
        if (!isSgNullStatement(sub_stmt)) {
            SgNullStatement * null_stmt = SageBuilder::buildNullStatement_nfi();
            translator.setCompilerGeneratedFileInfo(null_stmt);
            (*label_stmt_it)->set_statement(null_stmt);
            null_stmt->set_parent(*label_stmt_it);
            SageInterface::insertStatementAfter(*label_stmt_it, sub_stmt);
        }
    }

 // 2 - Place Preprocessor informations
}
#endif

#if 0
SgGlobal * ClangToDotTranslator::getGlobalScope() { return p_global_scope; }
#endif

#if 1
ClangToDotTranslator::ClangToDotTranslator(clang::CompilerInstance * compiler_instance, Language language_) :
    clang::ASTConsumer(),
    p_compiler_instance(compiler_instance),
    p_decl_translation_map(),
    p_stmt_translation_map(),
    p_type_translation_map(),
    p_node_desc(),
 // DQ (11/3/2020): Added data member.
    p_sage_preprocessor_recorder(new ClangToDotPreprocessorRecord(&(p_compiler_instance->getSourceManager()))),
    language(language_),
    ident_cnt(0)
{}
#endif

ClangToDotTranslator::~ClangToDotTranslator() {}

std::string ClangToDotTranslator::genNextIdent() {
    std::ostringstream oss;
    oss << "ident" << ident_cnt++;
    return oss.str();
}

#if 0
ClangToDotTranslator::ClangToDotTranslator(clang::CompilerInstance * compiler_instance, Language language_) :
    clang::ASTConsumer(),
    p_decl_translation_map(),
    p_stmt_translation_map(),
    p_type_translation_map(),
 // p_global_scope(NULL),
 // p_class_type_decl_first_see_in_type(),
 // p_enum_type_decl_first_see_in_type(),
    p_compiler_instance(compiler_instance),
 // p_sage_preprocessor_recorder(new ClangToDotPreprocessorRecord(&(p_compiler_instance->getSourceManager()))),
    language(language_)
{}

ClangToDotTranslator::~ClangToDotTranslator() 
   {
  // delete p_sage_preprocessor_recorder;
   }
#endif

/* (protected) Helper methods */

#if 0
void ClangToDotTranslator::applySourceRange(SgNode * node, clang::SourceRange source_range)  {
    SgLocatedNode * located_node = isSgLocatedNode(node);
    SgInitializedName * init_name = isSgInitializedName(node);

#if DEBUG_SOURCE_LOCATION
    std::cerr << "Set File_Info for " << node << " of type " << node->class_name() << std::endl;
#endif

    if (located_node == NULL && init_name == NULL) {
        std::cerr << "Consistency error: try to apply a source range to a Sage node which is not a SgLocatedNode or a SgInitializedName." << std::endl;
        exit(-1);
    }
    else if (located_node != NULL) {
        Sg_File_Info * fi = located_node->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = located_node->get_endOfConstruct();
        if (fi != NULL) delete fi;
    }
    else if (init_name != NULL) {
        Sg_File_Info * fi = init_name->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = init_name->get_endOfConstruct();
        if (fi != NULL) delete fi;
    }

    Sg_File_Info * start_fi = NULL;
    Sg_File_Info * end_fi = NULL;

    if (source_range.isValid()) {
        clang::SourceLocation begin  = source_range.getBegin();
        clang::SourceLocation end    = source_range.getEnd();

        if (begin.isValid() && end.isValid()) {
            if (begin.isMacroID()) {
#if DEBUG_SOURCE_LOCATION
                std::cerr << "\tDump SourceLocation begin as it is a MacroID: ";
                begin.dump(p_compiler_instance->getSourceManager());
                std::cerr << std::endl;
#endif
                begin = p_compiler_instance->getSourceManager().getExpansionLoc(begin);
                ROSE_ASSERT(begin.isValid());
            }

            if (end.isMacroID()) {
#if DEBUG_SOURCE_LOCATION
                std::cerr << "\tDump SourceLocation end as it is a MacroID: ";
                end.dump(p_compiler_instance->getSourceManager());
                std::cerr << std::endl;
#endif
                end = p_compiler_instance->getSourceManager().getExpansionLoc(end);
                ROSE_ASSERT(end.isValid());
            }

            clang::FileID file_begin = p_compiler_instance->getSourceManager().getFileID(begin);
            clang::FileID file_end   = p_compiler_instance->getSourceManager().getFileID(end);

            bool inv_begin_line;
            bool inv_begin_col;
            bool inv_end_line;
            bool inv_end_col;

            unsigned ls = p_compiler_instance->getSourceManager().getSpellingLineNumber(begin, &inv_begin_line);
            unsigned cs = p_compiler_instance->getSourceManager().getSpellingColumnNumber(begin, &inv_begin_col);
            unsigned le = p_compiler_instance->getSourceManager().getSpellingLineNumber(end, &inv_end_line);
            unsigned ce = p_compiler_instance->getSourceManager().getSpellingColumnNumber(end, &inv_end_col);

            if (file_begin.isInvalid() || file_end.isInvalid() || inv_begin_line || inv_begin_col || inv_end_line || inv_end_col) {
                ROSE_ASSERT(!"Should not happen as everything have been check before...");
            }

            if (p_compiler_instance->getSourceManager().getFileEntryForID(file_begin) != NULL) {
                std::string file = p_compiler_instance->getSourceManager().getFileEntryForID(file_begin)->getName();

                start_fi = new Sg_File_Info(file, ls, cs);
                end_fi   = new Sg_File_Info(file, le, ce);
#if DEBUG_SOURCE_LOCATION
                std::cerr << "\tCreate FI for node in " << file << ":" << ls << ":" << cs << std::endl;
#endif
            }
#if DEBUG_SOURCE_LOCATION
            else {
                std::cerr << "\tDump SourceLocation for \"Invalid FileID\": " << std::endl << "\t";
                begin.dump(p_compiler_instance->getSourceManager());
                std::cerr << std::endl << "\t";
                end.dump(p_compiler_instance->getSourceManager());
                std::cerr << std::endl;
            }
#endif
        }
    }

    if (start_fi == NULL && end_fi == NULL) {
        start_fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
        end_fi   = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();

        start_fi->setCompilerGenerated();
        end_fi->setCompilerGenerated();
#if DEBUG_SOURCE_LOCATION
        std::cerr << "Create FI for compiler generated node" << std::endl;
#endif
    }
    else if (start_fi == NULL || end_fi == NULL) {
        ROSE_ASSERT(!"start_fi == NULL xor end_fi == NULL");
    }

    if (located_node != NULL) {
        located_node->set_startOfConstruct(start_fi);
        located_node->set_endOfConstruct(end_fi);
    }
    else if (init_name != NULL) {
        init_name->set_startOfConstruct(start_fi);
        init_name->set_endOfConstruct(end_fi);
    }

}
#endif

#if 0
void ClangToDotTranslator::setCompilerGeneratedFileInfo(SgNode * node, bool to_be_unparse) {
    Sg_File_Info * start_fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
    Sg_File_Info * end_fi   = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();

    start_fi->setCompilerGenerated();
    end_fi->setCompilerGenerated();

    if (to_be_unparse) {
        start_fi->setOutputInCodeGeneration();
        end_fi->setOutputInCodeGeneration();
    }

    ROSE_ASSERT(start_fi != NULL && end_fi != NULL);

#if DEBUG_SOURCE_LOCATION
    std::cerr << "Set File_Info for " << node << " of type " << node->class_name() << std::endl;
#endif

    SgLocatedNode * located_node = isSgLocatedNode(node);
    SgInitializedName * init_name = isSgInitializedName(node);

    if (located_node == NULL && init_name == NULL) {
        std::cerr << "Consistency error: try to set a Sage node which is not a SgLocatedNode or a SgInitializedName as compiler generated" << std::endl;
        exit(-1);
    }
    else if (located_node != NULL) {
        Sg_File_Info * fi = located_node->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = located_node->get_endOfConstruct();
        if (fi != NULL) delete fi;

        located_node->set_startOfConstruct(start_fi);
        located_node->set_endOfConstruct(end_fi);
    }
    else if (init_name != NULL) {
        Sg_File_Info * fi = init_name->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = init_name->get_endOfConstruct();
        if (fi != NULL) delete fi;

        init_name->set_startOfConstruct(start_fi);
        init_name->set_endOfConstruct(end_fi);
    }
}
#endif

/* Overload of ASTConsumer::HandleTranslationUnit, it is the "entry point" */

void ClangToDotTranslator::HandleTranslationUnit(clang::ASTContext & ast_context) {

#if 1
     printf ("In ClangToDotTranslator::HandleTranslationUnit(): calling Traverse(ast_context.getTranslationUnitDecl()); \n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

    Traverse(ast_context.getTranslationUnitDecl());
}

/* Preprocessor Stack */

#if 1
std::pair<Sg_File_Info *, PreprocessingInfo *> ClangToDotTranslator::preprocessor_top() {
    return p_sage_preprocessor_recorder->top();
}
#endif
#if 1
bool ClangToDotTranslator::preprocessor_pop() {
    return p_sage_preprocessor_recorder->pop();
}
#endif

// struct NextPreprocessorToInsert

ClangToDotNextPreprocessorToInsert::ClangToDotNextPreprocessorToInsert(ClangToDotTranslator & translator_) :
  cursor(NULL),
  candidat(NULL),
  next_to_insert(NULL),
  translator(translator_)
{}

ClangToDotNextPreprocessorToInsert * ClangToDotNextPreprocessorToInsert::next() {
    if (!translator.preprocessor_pop()) return NULL;

    ClangToDotNextPreprocessorToInsert * res = new ClangToDotNextPreprocessorToInsert(translator);

    std::pair<Sg_File_Info *, PreprocessingInfo *> next = translator.preprocessor_top();
    res->cursor = next.first;
    res->next_to_insert = next.second;
    res->candidat = candidat;
}

// class

ClangToDotNextPreprocessorToInsert * ClangToDotPreprocessorInserter::evaluateInheritedAttribute(SgNode * astNode, ClangToDotNextPreprocessorToInsert * inheritedValue) {
    SgLocatedNode * loc_node = isSgLocatedNode(astNode);
    if (loc_node == NULL) return inheritedValue;

    Sg_File_Info * current_pos = loc_node->get_startOfConstruct();

    bool passed_cursor = *current_pos > *(inheritedValue->cursor);

    if (passed_cursor) {
        // TODO insert on inheritedValue->candidat
        return inheritedValue->next();
    }

    
}

// class SagePreprocessorRecord

ClangToDotPreprocessorRecord::ClangToDotPreprocessorRecord(clang::SourceManager * source_manager) :
  p_source_manager(source_manager),
  p_preprocessor_record_list()
{}

void ClangToDotPreprocessorRecord::InclusionDirective(clang::SourceLocation HashLoc, const clang::Token & IncludeTok, llvm::StringRef FileName, bool IsAngled,
                                                const clang::FileEntry * File, clang::SourceLocation EndLoc, llvm::StringRef SearchPath, llvm::StringRef RelativePath) {
    std::cerr << "InclusionDirective" << std::endl;

    bool inv_begin_line;
    bool inv_begin_col;

    unsigned ls = p_source_manager->getSpellingLineNumber(HashLoc, &inv_begin_line);
    unsigned cs = p_source_manager->getSpellingColumnNumber(HashLoc, &inv_begin_col);

    std::string file = p_source_manager->getFileEntryForID(p_source_manager->getFileID(HashLoc))->getName();

    std::cerr << "    In file  : " << file << std::endl;
    std::cerr << "    From     : " << ls << ":" << cs << std::endl;
    std::cerr << "    Included : " << FileName.str() << std::endl;
    std::cerr << "    Is angled: " << (IsAngled ? "T" : "F") << std::endl;

    Sg_File_Info * file_info = new Sg_File_Info(file, ls, cs);
    PreprocessingInfo * preproc_info = new PreprocessingInfo(
                                                  PreprocessingInfo::CpreprocessorIncludeDeclaration,
                                                  FileName.str(),
                                                  file,
                                                  ls,
                                                  cs,
                                                  0,
                                                  PreprocessingInfo::before
                                           );

    p_preprocessor_record_list.push_back(std::pair<Sg_File_Info *, PreprocessingInfo *>(file_info, preproc_info));
}

void ClangToDotPreprocessorRecord::EndOfMainFile() {
    std::cerr << "EndOfMainFile" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::Ident(clang::SourceLocation Loc, const std::string & str) {
    std::cerr << "Ident" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::PragmaComment(clang::SourceLocation Loc, const clang::IdentifierInfo * Kind, const std::string & Str) {
    std::cerr << "PragmaComment" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::PragmaMessage(clang::SourceLocation Loc, llvm::StringRef Str) {
    std::cerr << "PragmaMessage" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::PragmaDiagnosticPush(clang::SourceLocation Loc, llvm::StringRef Namespace) {
    std::cerr << "PragmaDiagnosticPush" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::PragmaDiagnosticPop(clang::SourceLocation Loc, llvm::StringRef Namespace) {
    std::cerr << "PragmaDiagnosticPop" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::PragmaDiagnostic(clang::SourceLocation Loc, llvm::StringRef Namespace, clang::diag::Severity Severity, llvm::StringRef Str) {
    std::cerr << "PragmaDiagnostic" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::MacroExpands(const clang::Token & MacroNameTok, const clang::MacroInfo * MI, clang::SourceRange Range) {
    std::cerr << "MacroExpands" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::MacroDefined(const clang::Token & MacroNameTok, const clang::MacroInfo * MI) {
    std::cerr << "" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::MacroUndefined(const clang::Token & MacroNameTok, const clang::MacroInfo * MI) {
    std::cerr << "MacroUndefined" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::Defined(const clang::Token & MacroNameTok) {
    std::cerr << "Defined" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::SourceRangeSkipped(clang::SourceRange Range) {
    std::cerr << "SourceRangeSkipped" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::If(clang::SourceRange Range) {
    std::cerr << "If" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::Elif(clang::SourceRange Range) {
    std::cerr << "Elif" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::Ifdef(const clang::Token & MacroNameTok) {
    std::cerr << "Ifdef" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::Ifndef(const clang::Token & MacroNameTok) {
    std::cerr << "Ifndef" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::Else() {
    std::cerr << "Else" << std::endl;
    ROSE_ASSERT(false);
}

void ClangToDotPreprocessorRecord::Endif() {
    std::cerr << "Endif" << std::endl;
    ROSE_ASSERT(false);
}

std::pair<Sg_File_Info *, PreprocessingInfo *> ClangToDotPreprocessorRecord::top() {
    return p_preprocessor_record_list.front();
}

bool ClangToDotPreprocessorRecord::pop() {
    p_preprocessor_record_list.erase(p_preprocessor_record_list.begin());
    return !p_preprocessor_record_list.empty();
}



ClangToDotTranslator::NodeDescriptor::NodeDescriptor(std::string ident_) :
    ident(ident_),
    kind_hierarchy(),
    successors(),
    attributes()
{}
