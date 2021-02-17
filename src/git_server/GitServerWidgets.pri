INCLUDEPATH += $$PWD

FORMS += \
   $$PWD/AddCodeReviewDialog.ui \
   $$PWD/CreateIssueDlg.ui \
   $$PWD/CreatePullRequestDlg.ui \
   $$PWD/MergePullRequestDlg.ui \
   $$PWD/ServerConfigDlg.ui

HEADERS += \
   $$PWD/AddCodeReviewDialog.h \
   $$PWD/AGitServerItemList.h \
   $$PWD/AvatarHelper.h \
   $$PWD/CircularPixmap.h \
   $$PWD/IssueDetailedView.h \
   $$PWD/IssueItem.h \
   $$PWD/IssuesList.h \
   $$PWD/MergePullRequestDlg.h \
   $$PWD/PrChangeListItem.h \
   $$PWD/PrChangesList.h \
   $$PWD/PrCommitsList.h \
   $$PWD/PrList.h \
   $$PWD/ServerConfigDlg.h \ \
   $$PWD/SourceCodeReview.h \
   $$PWD/document.h

SOURCES += \
   $$PWD/AddCodeReviewDialog.cpp \
   $$PWD/AGitServerItemList.cpp \
   $$PWD/CircularPixmap.cpp \
   $$PWD/IssueDetailedView.cpp \
   $$PWD/IssueItem.cpp \
   $$PWD/IssuesList.cpp \
   $$PWD/MergePullRequestDlg.cpp \
   $$PWD/PrChangeListItem.cpp \
   $$PWD/PrChangesList.cpp \
   $$PWD/PrCommitsList.cpp \
   $$PWD/PrList.cpp \
   $$PWD/ServerConfigDlg.cpp \
   $$PWD/SourceCodeReview.cpp \
   $$PWD/document.cpp

equals(QT_MAJOR_VERSION, 5) {
   HEADERS += \
       $$PWD/CodeReviewComment.h \
       $$PWD/CreateIssueDlg.h \
       $$PWD/CreatePullRequestDlg.h \
       $$PWD/PrCommentsList.h \
       $$PWD/previewpage.h
    SOURCES += \
       $$PWD/CodeReviewComment.cpp \
       $$PWD/CreateIssueDlg.cpp \
       $$PWD/CreatePullRequestDlg.cpp \
       $$PWD/PrCommentsList.cpp \
       $$PWD/previewpage.cpp
}

