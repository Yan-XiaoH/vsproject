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
#include <QFutureWatcher>
#include <QFuture>
#include <QProgressDialog>
#include <QTimer>
#include <QMutex>
#include <QAtomicBool>
#include "hginputsearchconditionwidget.h"
#include "rwDb.h"

// 搜索结果结构体
struct SearchResult {
    std::vector<std::map<std::string,std::string>> data;
    int totalCount = 0;
    bool success = false;
    QString errorMessage;
    qint64 elapsedMs = 0;
};

// 自定义表格项，支持富文本高亮
class HighlightedTableItem : public QTableWidgetItem {
public:
    HighlightedTableItem(const QString& text, const QString& keyword = QString());
    void setHighlightedText(const QString& text, const QString& keyword);
};

class HGLogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HGLogWidget(std::string,QWidget *parent = nullptr);
    bool closeWindow();
    ~HGLogWidget();

signals:

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
    
    // 异步搜索相关槽函数
    void onSearchFinished();
    void onSearchCancelled();
    void onSearchTimeout();
    void updateSearchProgress();

private:
    void fnReadDB(const std::string &tableName);
    int getTableNameIndex(const std::string& dbName);
    
    // 高性能搜索相关函数
    void performGlobalSearch();  // 执行全库搜索（新接口）
    void performGlobalSearchAsync();  // 异步执行全库搜索
    void displaySearchResults();  // 显示当前页搜索结果
    void performRunLogSearch();  // 运行日志搜索
    void performRunLogSearchAsync();  // 异步执行运行日志搜索
    void displayRunLogSearchResults();  // 显示运行日志搜索结果
    
    // 获取搜索参数
    RWDb::AuditLogQueryParam buildSearchParam();
    
    // 格式化时间字符串（用于SQL查询）
    std::string formatTimeForSql(const std::string& timeStr);
    
    // 异步搜索工作函数（在后台线程执行）
    static SearchResult doAsyncSearch(const RWDb::AuditLogQueryParam& param, QAtomicBool* cancelFlag);
    static SearchResult doAsyncRunLogSearch(const SearchCondition& condition, QAtomicBool* cancelFlag);
    
    // 显示搜索错误
    void showSearchError(const QString& error);
    
    // 清理搜索资源
    void cleanupSearchResources();

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
    
    // 搜索结果相关（新实现）
    std::vector<std::map<std::string,std::string>> m_currentPageResults;  // 当前页结果
    int m_searchTotalCount;      // 搜索结果总数
    int m_searchCurrentPage;     // 当前页码
    static const int SEARCH_PAGE_SIZE = 100;  // 每页显示条数
    bool m_isSearchMode;         // 是否为搜索模式
    QString m_currentKeyword;    // 当前搜索关键词（用于高亮）
    
    // 异步搜索相关成员
    QFutureWatcher<SearchResult>* m_searchWatcher = nullptr;  // 异步搜索监视器
    QProgressDialog* m_progressDialog = nullptr;              // 进度对话框
    QTimer* m_searchTimeoutTimer = nullptr;                   // 搜索超时定时器
    static const int SEARCH_TIMEOUT_MS = 30000;               // 搜索超时时间（30秒）
    QAtomicBool m_searchCancelled;                            // 搜索取消标志
    QMutex m_searchMutex;                                     // 搜索互斥锁
};

#endif // HGLOGWIDGET_H
