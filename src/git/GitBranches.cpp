#include "GitBranches.h"

#include <GitBase.h>
#include <GitConfig.h>

#include <QLogger.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
#endif

using namespace QLogger;

GitBranches::GitBranches(const QSharedPointer<GitBase> &gitBase)
   : mGitBase(gitBase)
{
}

GitExecResult GitBranches::getBranches()
{
   QLog_Debug("Git", "Getting branches");

   const auto cmd = QString("git branch -a");

   QLog_Trace("Git", QString("Getting branches: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   return ret;
}

GitExecResult GitBranches::getDistanceBetweenBranches(const QString &right)
{
   QLog_Debug("Git", QString("Executing getDistanceBetweenBranches: {origin/%1} and {%1}").arg(right));

   QScopedPointer<GitConfig> gitConfig(new GitConfig(mGitBase));

   const auto ret = gitConfig->getRemoteForBranch(right);
   GitExecResult result;

   if (right == "master")
      result = GitExecResult { false, "Same branch" };
   else
   {
      const auto remote = ret.success ? ret.output.toString().append("/") : QString();
      QScopedPointer<GitBase> gitBase(new GitBase(mGitBase->getWorkingDir()));
      const auto gitCmd = QString("git rev-list --left-right --count %1%2...%2").arg(remote, right);

      QLog_Trace("Git", QString("Getting distance between branches: {%1}").arg(gitCmd));

      result = gitBase->run(gitCmd);
   }

   return result;
}

GitExecResult GitBranches::createBranchFromAnotherBranch(const QString &oldName, const QString &newName)
{
   QLog_Debug("Git", QString("Creating branch from another branch: {%1} and {%2}").arg(oldName, newName));

   const auto cmd = QString("git branch %1 %2").arg(newName, oldName);

   QLog_Trace("Git", QString("Creating branch from another branch: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   return ret;
}

GitExecResult GitBranches::createBranchAtCommit(const QString &commitSha, const QString &branchName)
{
   QLog_Debug("Git", QString("Creating a branch from a commit: {%1} at {%2}").arg(branchName, commitSha));

   const auto cmd = QString("git branch %1 %2").arg(branchName, commitSha);

   QLog_Trace("Git", QString("Creating a branch from a commit: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   return ret;
}

GitExecResult GitBranches::checkoutBranchFromCommit(const QString &commitSha, const QString &branchName)
{
   QLog_Debug("Git",
              QString("Creating and checking out a branch from a commit: {%1} at {%2}").arg(branchName, commitSha));

   const auto cmd = QString("git checkout -b %1 %2").arg(branchName, commitSha);

   QLog_Trace("Git", QString("Creating and checking out a branch from a commit: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   return ret;
}

GitExecResult GitBranches::checkoutLocalBranch(const QString &branchName)
{
   QLog_Debug("Git", QString("Checking out local branch: {%1}").arg(branchName));

   const auto cmd = QString("git checkout %1").arg(branchName);

   QLog_Trace("Git", QString("Checking out local branch: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   if (ret.success)
      mGitBase->updateCurrentBranch();

   return ret;
}

GitExecResult GitBranches::checkoutRemoteBranch(const QString &branchName)
{
   QLog_Debug("Git", QString("Checking out remote branch: {%1}").arg(branchName));

   auto localBranch = branchName;
   if (localBranch.startsWith("origin/"))
      localBranch.remove("origin/");

   const auto cmd = QString("git checkout -b %1 %2").arg(localBranch, branchName);

   QLog_Trace("Git", QString("Checking out remote branch: {%1}").arg(cmd));

   auto ret = mGitBase->run(cmd);
   const auto output = ret.output.toString();

   if (ret.success && !output.contains("fatal:"))
      mGitBase->updateCurrentBranch();
   else if (output.contains("already exists"))
   {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
      QRegExp rx("\'\\w+\'");
      rx.indexIn(ret.output.toString());
      auto value = rx.capturedTexts().constFirst();
#else
      QRegularExpression rx("\'\\w+\'");
      QRegularExpressionMatch matches = rx.match(ret.output.toString());
      Q_ASSERT(matches.hasMatch());
      auto value = matches.captured(1);
#endif
      value.remove("'");

      if (!value.isEmpty())
         ret = checkoutLocalBranch(value);
      else
         ret.success = false;
   }

   return ret;
}

GitExecResult GitBranches::checkoutNewLocalBranch(const QString &branchName)
{
   QLog_Debug("Git", QString("Checking out new local branch: {%1}").arg(branchName));

   const auto cmd = QString("git checkout -b %1").arg(branchName);

   QLog_Trace("Git", QString("Checking out new local branch: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   if (ret.success)
      mGitBase->updateCurrentBranch();

   return ret;
}

GitExecResult GitBranches::renameBranch(const QString &oldName, const QString &newName)
{
   QLog_Debug("Git", QString("Renaming branch: {%1} at {%2}").arg(oldName, newName));

   const auto cmd = QString("git branch -m %1 %2").arg(oldName, newName);

   QLog_Trace("Git", QString("Renaming branch: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   if (ret.success)
      mGitBase->updateCurrentBranch();

   return ret;
}

GitExecResult GitBranches::removeLocalBranch(const QString &branchName)
{
   QLog_Debug("Git", QString("Removing local branch: {%1}").arg(branchName));

   const auto cmd = QString("git branch -D %1").arg(branchName);

   QLog_Trace("Git", QString("Removing local branch: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   return ret;
}

GitExecResult GitBranches::removeRemoteBranch(const QString &branchName)
{
   auto branch = branchName;
   branch = branch.mid(branch.indexOf('/') + 1);

   QLog_Debug("Git", QString("Removing a remote branch: {%1}").arg(branch));

   QScopedPointer<GitConfig> gitConfig(new GitConfig(mGitBase));

   auto ret = gitConfig->getRemoteForBranch(branch);

   const auto cmd
       = QString("git push --delete %2 %1").arg(branch, ret.success ? ret.output.toString() : QString("origin"));

   QLog_Trace("Git", QString("Removing a remote branch: {%1}").arg(cmd));

   ret = mGitBase->run(cmd);

   return ret;
}

GitExecResult GitBranches::getLastCommitOfBranch(const QString &branch)
{
   QLog_Debug("Git", QString("Getting last commit of a branch: {%1}").arg(branch));

   const auto cmd = QString("git rev-parse %1").arg(branch);

   QLog_Trace("Git", QString("Getting last commit of a branch: {%1}").arg(cmd));

   auto ret = mGitBase->run(cmd);

   if (ret.success)
      ret.output = ret.output.toString().trimmed();

   return ret;
}

GitExecResult GitBranches::pushUpstream(const QString &branchName)
{
   QLog_Debug("Git", QString("Pushing upstream: {%1}").arg(branchName));

   const auto cmd = QString("git push --set-upstream origin %1").arg(branchName);

   QLog_Trace("Git", QString("Pushing upstream: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   return ret;
}
