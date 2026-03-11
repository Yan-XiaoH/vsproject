#ifndef HGLOGWIDGET_H
#define HGLOGWIDGET_H

#include <QWidget>
#include "hgqlabel.h"
#include <QSplitter>
#include <QPushButton>
#include <QTableWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QComboBox>
#include <QLineEdit>
#include "hginputsearchconditionwidget.h"
#include <QThread>
#include <QProgressBar>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

class HGLogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HGLogWidget(std::string,QWidget *parent = nullptr);
    bool closeWindow();
    ~HGLogWidget();

signals:
    void signalSearchStarted();
    void signalSearchFinished();

private slots:
    void slotLogTypeChanged(int);
    void slotKeyWord(QString);
    void slotTimeFrom(QString);
    void slotTimeTo(QString);
    void slotSearch();
    void slotSaveSearchLog();
    void slotNext();
    void slotPre();
    void slotClearSearch();
    void onSearchStarted();
    void onSearchFinished();
    void onSearchCanceled();

private:
    void fnReadDB(const std::string &tableName);
    int getTableNameIndex(const std::string& dbName);

private:
    QLabel* m_pageLabel;
    HGQLabel *m_saveLabel, *m_exportLabel;
    HGQLabel* m_nextLabel, *m_preLabel;
    QGroupBox *m_manipulateGroup;
    QGridLayout *m_manipulateLayout, *m_layout;
    QLabel* m_logTypeLabel;
    QComboBox* m_logTypeComboBox;
    HGInputSearchConditionWidget* m_inputsearchConditionW;
    QTableWidget* m_tableW;
    std::string m_lang;

    SearchCondition m_searchCondition;
    
    std::map<std::string, int> m_logContentMap;
    int m_curDisplayIndex;
    std::vector<std::string> m_auditLogTableNames;
    
    // Search results pagination
    std::vector<std::map<std::string,std::string>> m_searchResults;
    int m_curSearchPage;
    int m_totalSearchPages;
    const int PAGE_SIZE = 1000;
    
    // Asynchronous processing
    QProgressBar* m_progressBar;
    QFuture<std::vector<std::map<std::string,std::string>>> m_searchFuture;
    QFutureWatcher<std::vector<std::map<std::string,std::string>>> m_searchWatcher;
};

#endif // HGLOGWIDGET_H
