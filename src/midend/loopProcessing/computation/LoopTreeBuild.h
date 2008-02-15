
#ifndef BUILD_LOOP_TREE
#define BUILD_LOOP_TREE

#include <LoopTree.h>
#include <ProcessAstTree.h>

class LoopTreeBuild : public ProcessAstTree
{
  LoopTreeCreate *lt;
  LoopTransformInterface* la;
  LoopTreeNode *cur;
 protected:
//Boolean ProcessLoop( AstInterface &fa, const AstNodePtr& loop ,
  int ProcessLoop( AstInterface &fa, const AstNodePtr& loop ,
                      const AstNodePtr& body,
                      AstInterface::TraversalVisitType t)
    {
       LoopTreeNode *root = lt->GetTreeRoot();
       if (t == AstInterface::PreVisit) {
          LoopTreeNode* result = lt->CreateLoopNode( *la, loop);
          result->Link(cur, LoopTreeNode::AsLastChild);
          cur = result;
        }
        else {
          if (cur != root)
             cur = cur->Parent();
        }
        return ProcessAstTree::ProcessLoop(fa, loop, body, t);
     }
//Boolean ProcessIf( AstInterface &fa, const AstNodePtr& s, const AstNodePtr& cond,
  void ProcessIf( AstInterface &fa, const AstNodePtr& s, const AstNodePtr& cond,
                     const AstNodePtr& body1, const AstNodePtr& body2,
                      AstInterface::TraversalVisitType t)
    { 
        if (t == AstInterface::PreVisit) return ProcessStmt(fa, s); 
         // return true;
    }

//Boolean ProcessGoto( AstInterface &fa, const AstNodePtr& start, 
  int ProcessGoto( AstInterface &fa, const AstNodePtr& start, 
                       const AstNodePtr& dest)
    { ProcessStmt(fa, start); return true; }

//Boolean ProcessStmt( AstInterface &fa, const AstNodePtr& start)
  void ProcessStmt( AstInterface &fa, const AstNodePtr& start)
    {
       std::cerr << "LoopTreeBuild::ProcessStmt " << start << std::endl;
       LoopTreeNode *stmtNode = lt->CreateStmtNode( start);
       stmtNode->Link(cur, LoopTreeNode::AsLastChild);
       ProcessAstTree::ProcessStmt(fa, start);
    } 
 public:
// Boolean operator ()( AstInterface& fa, const AstNodePtr& top, 
   int operator ()( AstInterface& fa, const AstNodePtr& top, 
                        LoopTreeCreate *ltc, LoopTransformInterface* _la,
                        LoopTreeNode* root = 0)
     { 
        lt = ltc;
        la = _la;
        cur = (root)? root: lt->GetTreeRoot();
        return ReadAstTraverse(fa, top, *this, AstInterface::PreAndPostOrder);
     }

   LoopTreeNode* LastProcessedNode()
     {
        if (cur->ChildCount() > 0)
           return cur->LastChild();
        else
           return cur;
     }
};

#endif

